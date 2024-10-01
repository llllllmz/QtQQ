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
// ��̬��Ա������ʼ��
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

	tcpSocket->abort(); // ȷ��û��δ��ɵ����ӳ���
	tcpSocket->connectToHost(hostName, port);

	if (!tcpSocket->waitForConnected(5000)) {
		QMessageBox::critical(nullptr, QString::fromLocal8Bit("���ӷ���������!"), tcpSocket->errorString());
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
	// ����һ�����������ݰ�
	QByteArray fullPacket;

	// ����ͷ��ʶ����ӵ����ݰ���
	fullPacket.append("abcd");

	// ���¼����ͣ�2 �ֽڣ���ӵ����ݰ���
	quint16 temeventType = (quint16)eventType; // ʾ���¼�����
	quint16 et = qToBigEndian<quint16>(temeventType);
	fullPacket.append((char*)&et, sizeof(et));

	// �����峤�ȣ�4 �ֽڣ���ӵ����ݰ���
	quint32 tembodyLength = data.length();
	quint32 bodyLength = qToBigEndian<quint32>(tembodyLength);//��datastream���Բ����ֽ���ת����
	fullPacket.append((char*)&bodyLength, sizeof(bodyLength));

	// �� JSON ������ӵ����ݰ���
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
	//todo��������������ź�
	//todo�Ȳ�������ճ���ͽ������
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
		//length = qFromBigEndian<int>(length);//?Ϊʲô����ת��
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
			// ����������
			if (jsonError.error != QJsonParseError::NoError) {
				qWarning() << "Failed to parse JSON data:" << jsonError.errorString();
			}

			// ����Ƿ�Ϊ����
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
				// ���� QByteArray
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

				
				// ��������
				for (const QJsonValue &value : array) {
					if (value.isObject()) {
						QJsonObject groupObj = value.toObject();
						qDebug() << "Group ID:" << groupObj.value("userGroupId").toInt();
						qDebug() << "Group Name:" << groupObj.value("groupName").toString();
						// ���� UserGroup ����
						UserGroup *group = new UserGroup;
						group->from_json(groupObj); // ʹ�� from_json ����������

						// �洢�� QMap ��
						int groupId = groupObj.value("userGroupId").toInt();
						map.insert(groupId, group);
					}
				}
				emit user_group_response(map);
			case ET_UserListResponse:
				obj = jsonDoc.object();
				array = obj["friendships"].toArray();
				// ������һ�� QJsonArray array ������� QJsonValue ����ÿ���������һ�� FriendshipDTO
				

				for (const QJsonValue &value : array) {
					if (value.isObject()) {
						QJsonObject dtoObj = value.toObject();

						// �� JSON ��������ȡ User �������Ϣ
						QJsonObject userObj = dtoObj.value("user").toObject();

						//// ���� User ����
						//User *user = new User;
						//user->from_json(userObj); // ʹ�� from_json ������� User ����

						// ���� FriendshipDTO ����
						FriendshipDTO *friendshipDTO = new FriendshipDTO;
						friendshipDTO->from_json(dtoObj); // ʹ�� from_json ������� FriendshipDTO ����
						//friendshipDTO->setUser(*user); // ���� FriendshipDTO �� User ��Ա

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