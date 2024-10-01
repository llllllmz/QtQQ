#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include "UseMySQL.h"
#include "CustomProxyStyle.h"
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include "WindowManager.h"
#include "RecvFile.h"
extern QString LoginUserAccount;       //定义于CCMainWindow.cpp

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent), emotionWindow(nullptr)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	init();
}

TalkWindowShell::~TalkWindowShell()
{
	if (emotionWindow) emotionWindow->deleteLater();
}

void TalkWindowShell::init()
{
	initStyle();
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onListItemClicked);
	initEmotionWindow();
	initJsFile();
	initTcpSocket();
	initUdpSocket();
}

void TalkWindowShell::initStyle()
{
	loadStyleSheet("TalkWindow");
	ui.listWidget->setStyle(new CustomProxyStyle());
	ui.splitter->setSizes(QList<int>{154, width() - 154});
}

void TalkWindowShell::initEmotionWindow()
{
	emotionWindow = new EmotionWindow;
	emotionWindow->hide();
	connect(emotionWindow, &EmotionWindow::addEmotion, this, &TalkWindowShell::onAddEmotion);
}

void TalkWindowShell::initJsFile()
{
	//读取msgPageTmp.txt
	QFile tmpFile(":/Resources/MainWindow/MsgHtml/msgPageTmp.txt");
	QString tmpFileContent;
	if (tmpFile.open(QFile::ReadOnly))
	{
		tmpFileContent = tmpFile.readAll();
		tmpFile.close();
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Read msgPageTmp.txt Error.");
		return;
	}

	if (!replaceJsTxt(tmpFileContent)) return;

	/*
	    qrc文件在编译时被嵌入到应用程序的二进制文件中，因此在运行时它们是不可变的
		qrc文件不能被修改，只能用只读方式打开，所以这里用相对路径
		相对路径是相对于工作目录：项目属性->调试->工作目录
		更改msgPage.html中引用的文件
	*/
	QFile js("./Resources/MainWindow/MsgHtml/msghandling.js");
	if (js.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream stream(&js);
		stream << tmpFileContent;
		js.close();
		return;
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Write msghandling.js Error.");
		return;
	}
}

/*
* @brief: 替换js.txt文件内容
* @param: fileContent: 文件内容
* @return: bool: 是否替换成功
*/
bool TalkWindowShell::replaceJsTxt(QString& fileContent)
{
	//需要替换的文本
	QString replaceVar = "var external = null;";                      //变量定义
	QString replaceObj = "external = channel.objects.external;";      //共享对象赋值
	QString replaceRecv;                                              //recv函数
	QFile recvFile(":/Resources/MainWindow/MsgHtml/recvHtmlTmp.txt");
	if (recvFile.open(QFile::ReadOnly))
	{
		replaceRecv = recvFile.readAll();
		recvFile.close();
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Read recvHtmlTmp.txt Error.");
		return false;
	}

	//获取用户列表
	QList<int> userList = UseMySQL::instance()->getUserList();

	//替换文本内容
	QString var, obj, recv;
	for (int id : userList)     //每次循环从userList中获取一个用户id
	{
		QString tmpVar = replaceVar;
		tmpVar.replace("external", QString("external_%1").arg(id));
		var += (tmpVar + "\n");

		QString tmpObj = replaceObj;
		tmpObj.replace("external", QString("external_%1").arg(id));
		obj += (tmpObj + "\n");

		QString tmpRecv = replaceRecv;
		tmpRecv.replace("recvHtml", QString("recvHtml_%1").arg(id));
		tmpRecv.replace("external", QString("external_%1").arg(id));
		recv += (tmpRecv + "\n");
	}

	fileContent.replace(replaceVar, var);
	fileContent.replace(replaceObj, obj);
	fileContent.replace(replaceRecv, recv);

	return true;
}

void TalkWindowShell::initTcpSocket()
{
	tcp = new QTcpSocket(this);
	tcp->connectToHost("127.0.0.1", tcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	udp = new QUdpSocket(this);

	//udp绑定端口号，允许多个套接字共享同一个地址和端口
	if (!udp->bind(udpPort, QAbstractSocket::ReuseAddressHint))
	{
		QMessageBox::information(nullptr, "error", "Udp socket bind Error.");
		return;
	}

	connect(udp, &QUdpSocket::readyRead, this, &TalkWindowShell::readMsgFromServer);
}

void TalkWindowShell::addTalkWindow(TalkWindow* window, TalkWindowItem* windowItem)
{
	ui.rightStackedWidget->addWidget(window);    //聊天窗口添加到堆栈窗体
	ui.rightStackedWidget->setCurrentWidget(window);

	QListWidgetItem* item = new QListWidgetItem(ui.listWidget);
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, windowItem);     //窗口项添加到列表
	item->setSelected(true);         //对应列表项设置为选中状态

	map.insert(item, window);        //添加到映射

	connect(windowItem, &TalkWindowItem::signalCloseBtnClicked, this, [window, windowItem, item, this]()
	{
		map.remove(item);      //删除容器中key为item的项

		//listWidget
		ui.listWidget->takeItem(ui.listWidget->row(item));      //移除给定行的项
		delete item;
		windowItem->setParent(nullptr);
		windowItem->deleteLater();

		//stackedWidget
		ui.rightStackedWidget->removeWidget(window);
		window->setParent(nullptr);
		window->deleteLater();

		if (ui.rightStackedWidget->count() == 0)
		{
			this->close();          //关闭talkWindowShell
		}
	});
}

void TalkWindowShell::updateCurrentWindow(QWidget* window)
{
	QListWidgetItem* item = map.key(window, nullptr);
	if (item)
	{
		item->setSelected(true);
		ui.rightStackedWidget->setCurrentWidget(window);
	}
}

void TalkWindowShell::onEmotionBtnClicked()
{
	//表情按钮点击之后，表情窗口显示/隐藏
	emotionWindow->setVisible(!emotionWindow->isVisible());

	//显示表情窗口
	QPoint pos = this->mapToGlobal(QPoint(0, 0));       //将窗体的左上角坐标转换为全局坐标
	QPoint movePos = pos + QPoint(170, 220);
	emotionWindow->move(movePos);
}

void TalkWindowShell::onAddEmotion(int name)
{
	emotionWindow->hide();     //隐藏表情窗口
	//获取TalkWindow
	TalkWindow* window = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (window)
	{
		window->addEmotionImage(name);
	}
}

void TalkWindowShell::onListItemClicked(QListWidgetItem* item)
{
	/*
	* 如果存在，则返回窗口，否则返回nullptr
	*/
	QWidget* widget = map.value(item, nullptr);
	if (widget)
	{
		ui.rightStackedWidget->setCurrentWidget(widget);
	}
}

void TalkWindowShell::sendMsgToServer(QJsonValue data)
{
	//获取活动窗口
	TalkWindow* window = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	const QString talkID = QString::number(window->getTalkId());

	QJsonObject msgObj;     //消息对象

	//聊天窗口类型
	//if (talkID.length() <= GROUP_ID_LENGTH)                      //群聊
	//	msgObj.insert(TYPE, TYPE_GROUP);
	//else                                                         //单聊
		msgObj.insert(TYPE, TYPE_SINGLE);

	//发送者/接收者（群号/员工ID）
	msgObj.insert(SEND, LoginUserAccount);
	msgObj.insert(RECV, talkID);

	//消息内容
	switch (data.type())
	{
	case QJsonValue::Array:         //普通消息
	{
		msgObj.insert(WHAT, WHAT_MSG);
		msgObj.insert(WHAT_MSG, data);
		break;
	}
	case QJsonValue::Object:        //文件消息
	{
		msgObj.insert(WHAT, WHAT_FILE);
		msgObj.insert(WHAT_FILE, data);
		break;
	}
	default:
	{
		QMessageBox::information(nullptr, "error", "Other Msg Type.");
		return;
	}
	}

	//发送消息给服务器
	QString msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);         //将QJsonObject转换为紧凑格式的JSON字符串

	qDebug() << msg;

	if (tcp->state() != QAbstractSocket::ConnectedState)      //未连接状态
	{
		QMessageBox::information(nullptr, "error", "TCP socket connect Error.");
		return;
	}
	tcp->write(msg.toUtf8());
}

void TalkWindowShell::readMsgFromServer()
{
	//端口中有未处理的数据
	while (udp->hasPendingDatagrams())    
	{
		QByteArray msg(udp->pendingDatagramSize(), 0);
		udp->readDatagram(msg.data(), msg.size());       //接收不大于msg.size()字节的数据报，并将其存储在data中

		//将消息字符串传为json对象
		QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
		if (jsonDoc.isNull() || !jsonDoc.isObject()) return;       //json格式错误
		QJsonObject jsonObj = jsonDoc.object();          //返回文档中包含的QJsonObject

		/*      解析json对象        */

		QString sender = jsonObj[SEND].toString();       //发送者
		if (sender == LoginUserAccount) return;          //自己发送的消息不做处理

		QString type = jsonObj[TYPE].toString();         //聊天窗口类型
		QString receiver = jsonObj[RECV].toString();     //接收者（群号/员工ID）

		QString talkId = receiver;                       //聊天窗口ID
		if (type == TYPE_SINGLE)            //如果是单聊
		{
			if (receiver != LoginUserAccount) return;    //并且不是发给自己的消息，则返回
			talkId = sender;
		}

		QString what = jsonObj[WHAT].toString();         //消息类型
		QJsonValue data = jsonObj[what];                 //消息内容

		QJsonArray msgArray;
		if (what == WHAT_MSG)                   //普通消息
		{
			msgArray = data.toArray();
		}
		else if (what == WHAT_FILE)             //文件消息
		{
			QJsonObject fileObj = data.toObject();

			RecvFile* recvFile = new RecvFile(fileObj, this);
			QString msg = QString::fromLocal8Bit("收到来自")
				+ UseMySQL::instance()->getUserName(sender)
				+ QString::fromLocal8Bit("发来的文件，是否接收？");
			recvFile->setLabelMsg(msg);
			recvFile->show();
		}

		/*
			获取对应的聊天窗口，如果不存在，则忽略该消息
			对于群聊，聊天窗口不存在分为两种情况：
				1.没有对应群聊
				2.有对应群聊，但是没有打开
		*/
		QWidget* widget = WindowManager::instance()->findWindowMap(talkId.toInt());
		if (!widget) return;        //聊天窗口未打开

		this->updateCurrentWindow(widget);
		dynamic_cast<TalkWindow*>(widget)->appendObjectMsg(msgArray, sender);
	}
}