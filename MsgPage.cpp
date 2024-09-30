#include "MsgPage.h"
#include <qfile.h>
#include "define.h"
#include "qmessagebox.h"
#include "UseMySQL.h"
#include "WindowManager.h"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsondocument.h>

extern QString LoginUserAccount;       //定义于CCMainWindow.cpp

MsgHtmlObj::MsgHtmlObj(QObject* parent, QString headPath)
	: QObject(parent), leftHeadPath(headPath)
{
	initMsgHtml();
}

MsgHtmlObj::~MsgHtmlObj()
{
}

void MsgHtmlObj::initMsgHtml()
{
	//右边显示发送消息
	msgRight = getMsgHtml("userRight");
	msgRight.replace("%1", UseMySQL::instance()->getUserHeadPixmap(LoginUserAccount));

	//左边显示接收消息
	msgLeft = getMsgHtml("userLeft");
	msgLeft.replace("%1", leftHeadPath);
}

QString MsgHtmlObj::getMsgHtml(const QString& fileName)
{
	QFile file(HTML_PATH(fileName));
	QString fileContent;
	if(file.open(QFile::ReadOnly))
	{
		fileContent = file.readAll();
		file.close();
	}
	else
	{
		QMessageBox::information(nullptr, "error", QString("Read %1.html Error.").arg(fileName));
	}

	return fileContent;
}

MsgWebView::MsgWebView(QWidget* parent)
	: QWebEngineView(parent), rightObj(nullptr)
{
	this->setContextMenuPolicy(Qt::NoContextMenu);  //禁止右键弹出菜单

	MsgWebPage* page = new MsgWebPage(this);       	//初始化页面
	this->setPage(page);

	channel = new QWebChannel(this);               	//初始化通道
	this->page()->setWebChannel(channel);
	this->load(QUrl(QString("qrc") + HTML_PATH("msgPage")));        //加载页面

	registerChannelObject();       //注册通道对象

	/*
		SLOT() 宏的作用是将槽函数名称转换为特定的字符串格式
		当 connect() 函数被调用时，它不会验证槽函数的访问权限，而是直接根据函数签名进行匹配
	*/
	connect(this, SIGNAL(signalSendMsg(QJsonValue)), WindowManager::instance()->getTalkWindowShell(), SLOT(sendMsgToServer(QJsonValue)));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::registerChannelObject()
{
	//注册发送消息对象
	rightObj = new MsgHtmlObj(this);
	channel->registerObject(MSGOBJ_RIGHT, rightObj);

	//注册接收消息对象
	int talkID = WindowManager::instance()->getCreateID();
	QList<QPair<QString, QString>> infoList;
	if (!UseMySQL::instance()->getRegisterObjectInfo(QString::number(talkID), infoList))
	{
		QMessageBox::information(nullptr, "error", "getRegisterObjectInfo Error.");
		return;
	}

	for (auto& info : infoList)
	{
		QString account = info.first;
		QString headPath = info.second;

		MsgHtmlObj* leftObj = new MsgHtmlObj(this, headPath);
		channel->registerObject(MSGOBJ_LEFT(account), leftObj);
	}
}

void MsgWebView::appendMsg(const QJsonArray& jsonMsg, QString obj)
{
	QString msg = parseMsgFromJson(jsonMsg);

	QJsonObject msgObj;
	msgObj.insert(HTML_REPLACE, msg);

	//将QJsonObject转换为紧凑格式的JSON字符串
	const QString jsonString = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	if (obj == MYSELF)
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(jsonString));
		emit signalSendMsg(jsonMsg);     //发送信号将消息传递给TalkWindowShell
	}
	else
	{
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(obj).arg(jsonString));
	}
}

/*
* @brief  解析json消息
* @param  arr: json消息数组
* @return msg: 解析后的消息字符串
*/
QString MsgWebView::parseMsgFromJson(const QJsonArray& arr)
{
	QString msg;        //消息字符串
	for (int i = 0; i < arr.count(); i++)
	{
		QJsonArray blockArray = arr[i].toArray();           //通过下标获取文本块json数组
		for (int j = 0; j < blockArray.count(); j++)        //组装文本块
		{
			QJsonObject obj = blockArray[j].toObject();     //片段json对象
			QString key = obj.keys().at(0);                
			if (key == TXT)
			{
				msg += obj[key].toString();      //文本
			}
			else if (key == IMG)
			{
				QString value = QString(IMG_SRC).arg(IMG_QRC + obj[key].toString() + IMG_PNG);
				msg += value;
			}
		}
		msg += BR;       //换行
	}

	//删除末尾的<br>，在 HTML 中，浏览器通常会忽略位于文档末尾的 <br> 标签
	//int pos = msg.indexOf(BR, -4, Qt::CaseSensitive);     //从倒数第4个字符开始找<br>，大小写敏感
	//msg.truncate(pos);         //在pos处截断字符串

	return msg;
}
