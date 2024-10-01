#include "ChatSocket.h"
#include <QMessageBox>
#include <QtEndian>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include "User.h"
#include "UserGroup.h"
#include "FriendshipDTO.h"

Q_GLOBAL_STATIC(ChatSocket, theInstance);
// 静态成员变量初始化
ChatSocket *ChatSocket::m_instance = nullptr;

ChatSocket::ChatSocket(QObject *parent)
	: QObject(parent), tcpSocket(new QTcpSocket(this))
{
	connect(tcpSocket, &QTcpSocket::connected, this, &ChatSocket::onConnected);
	connect(tcpSocket, &QTcpSocket::disconnected, this, &ChatSocket::onDisconnected);
	connect(tcpSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &ChatSocket::onError);
	connect(tcpSocket, &QTcpSocket::readyRead, this, &ChatSocket::onReadyRead);
}

ChatSocket::~ChatSocket()
{
	delete tcpSocket;
}

ChatSocket *ChatSocket::instance()
{
	return theInstance();
}

void ChatSocket::connectToHost(const QString &hostName, quint16 port)
{
	if (isConnected())
		return;

	tcpSocket->abort(); // 确保没有未完成的连接尝试
	tcpSocket->connectToHost(hostName, port);

	if (!tcpSocket->waitForConnected(5000)) {
		QMessageBox::critical(nullptr, QString::fromLocal8Bit("连接服务器出错!"), tcpSocket->errorString());
		return;
	}
}

void ChatSocket::disconnectFromHost()
{
	if (isConnected())
		tcpSocket->disconnectFromHost();
}

bool ChatSocket::isConnected() const
{
	return tcpSocket->state() == QAbstractSocket::ConnectedState;
}

void ChatSocket::sendText(EventType eventType, const QByteArray &data)
{
	// 创建一个完整的数据包
	QByteArray fullPacket;

	// 将包头标识符添加到数据包中
	fullPacket.append("abcd");

	// 将事件类型（2 字节）添加到数据包中
	quint16 temeventType = (quint16)eventType; // 示例事件类型
	quint16 et = qToBigEndian<quint16>(temeventType);
	fullPacket.append((char*)&et, sizeof(et));

	// 将包体长度（4 字节）添加到数据包中
	quint32 tembodyLength = data.length();
	quint32 bodyLength = qToBigEndian<quint32>(tembodyLength);//用datastream可以不用字节序转换？
	fullPacket.append((char*)&bodyLength, sizeof(bodyLength));

	// 将 JSON 数据添加到数据包中
	fullPacket.append(data);
	tcpSocket->write(fullPacket);
}

void ChatSocket::onConnected()
{
	emit connected();
}

void ChatSocket::onDisconnected()
{
	emit disconnected();
}

void ChatSocket::onError(QAbstractSocket::SocketError error)
{
	emit errorOccurred(error);
}

void ChatSocket::onReadyRead()
{
	qDebug() << "onReadyRead";

	//emit readyRead();
	//todo解析并发送相关信号
	//todo先不考虑是粘包和解包问题
	if (tcpSocket->bytesAvailable() >= lHeaderTotal)
	{
		
		QByteArray data = tcpSocket->peek(lHeaderTotal);
		QDataStream in(data);
		short eventType;
		int length;
		char magicBuf[5] = { 0 };
		in.readRawData(magicBuf,4);
		in >> eventType;
		in >> length;

		//eventType = qFromBigEndian<short>(eventType);
		//length = qFromBigEndian<int>(length);//?为什么不用转换
		qDebug() << eventType;
		qDebug() << length;

		if (tcpSocket->bytesAvailable() >= length + lHeaderTotal)
		{
			//qDebug() << "tcpSocket->bytesAvailable() >= length + lHeaderTotal";
			//qDebug() << eventType;
			tcpSocket->read(lHeaderTotal);
			QByteArray body = tcpSocket->read(length);
			qDebug() << body;

			QJsonParseError jsonError;
			QJsonDocument jsonDoc = QJsonDocument::fromJson(body, &jsonError);
			// 检查解析错误
			if (jsonError.error != QJsonParseError::NoError) {
				qWarning() << "Failed to parse JSON data:" << jsonError.errorString();
			}

			// 检查是否为对象
			if (!jsonDoc.isObject()) {
				qWarning() << "JSON document is not an object.";
			}
			QJsonObject obj;
			QJsonArray array;
			QMap<int, UserGroup*> map;
			QList<FriendshipDTO*> friendships;
			switch (eventType)
			{
			case ET_LoginResponse:
				// 解析 QByteArray
				obj = jsonDoc.object();
				if (obj["result"].toBool())
				{
					QJsonObject userObj = obj["user"].toObject();
					User* user = new User;
					user->setId(userObj["id"].toInt());
					user->setUsername(userObj["username"].toString());
					user->setPassword(userObj["password"].toString());
					user->setSignName(userObj["signname"].toString());
					emit login_response(true,"",user);
				}
				else
				{
					QString reason = obj["reason"].toString();
					emit login_response(false, reason,nullptr);
				}
				break;
			case ET_UserGroupResponse:
				obj = jsonDoc.object();
				array = obj["groups"].toArray();

				
				// 遍历数组
				for (const QJsonValue &value : array) {
					if (value.isObject()) {
						QJsonObject groupObj = value.toObject();
						qDebug() << "Group ID:" << groupObj.value("userGroupId").toInt();
						qDebug() << "Group Name:" << groupObj.value("groupName").toString();
						// 创建 UserGroup 对象
						UserGroup *group = new UserGroup;
						group->from_json(groupObj); // 使用 from_json 方法填充对象

						// 存储到 QMap 中
						int groupId = groupObj.value("userGroupId").toInt();
						map.insert(groupId, group);
					}
				}
				emit user_group_response(map);
			case ET_UserListResponse:
				obj = jsonDoc.object();
				array = obj["friendships"].toArray();
				// 假设有一个 QJsonArray array 包含多个 QJsonValue 对象，每个对象代表一个 FriendshipDTO
				

				for (const QJsonValue &value : array) {
					if (value.isObject()) {
						QJsonObject dtoObj = value.toObject();

						// 从 JSON 对象中提取 User 对象的信息
						QJsonObject userObj = dtoObj.value("user").toObject();

						//// 创建 User 对象
						//User *user = new User;
						//user->from_json(userObj); // 使用 from_json 方法填充 User 对象

						// 创建 FriendshipDTO 对象
						FriendshipDTO *friendshipDTO = new FriendshipDTO;
						friendshipDTO->from_json(dtoObj); // 使用 from_json 方法填充 FriendshipDTO 对象
						//friendshipDTO->setUser(*user); // 设置 FriendshipDTO 的 User 成员

						//
						friendships.append(friendshipDTO);
					}
				}
				emit friend_ship_response(friendships);

			default:
				break;
			}
		}
	}
}