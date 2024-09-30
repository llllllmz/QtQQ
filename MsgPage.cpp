#include "MsgPage.h"
#include <qfile.h>
#include "define.h"
#include "qmessagebox.h"
#include "UseMySQL.h"
#include "WindowManager.h"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsondocument.h>

extern QString LoginUserAccount;       //������CCMainWindow.cpp

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
	//�ұ���ʾ������Ϣ
	msgRight = getMsgHtml("userRight");
	msgRight.replace("%1", UseMySQL::instance()->getUserHeadPixmap(LoginUserAccount));

	//�����ʾ������Ϣ
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
	this->setContextMenuPolicy(Qt::NoContextMenu);  //��ֹ�Ҽ������˵�

	MsgWebPage* page = new MsgWebPage(this);       	//��ʼ��ҳ��
	this->setPage(page);

	channel = new QWebChannel(this);               	//��ʼ��ͨ��
	this->page()->setWebChannel(channel);
	this->load(QUrl(QString("qrc") + HTML_PATH("msgPage")));        //����ҳ��

	registerChannelObject();       //ע��ͨ������

	/*
		SLOT() ��������ǽ��ۺ�������ת��Ϊ�ض����ַ�����ʽ
		�� connect() ����������ʱ����������֤�ۺ����ķ���Ȩ�ޣ�����ֱ�Ӹ��ݺ���ǩ������ƥ��
	*/
	connect(this, SIGNAL(signalSendMsg(QJsonValue)), WindowManager::instance()->getTalkWindowShell(), SLOT(sendMsgToServer(QJsonValue)));
}

MsgWebView::~MsgWebView()
{
}

void MsgWebView::registerChannelObject()
{
	//ע�ᷢ����Ϣ����
	rightObj = new MsgHtmlObj(this);
	channel->registerObject(MSGOBJ_RIGHT, rightObj);

	//ע�������Ϣ����
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

	//��QJsonObjectת��Ϊ���ո�ʽ��JSON�ַ���
	const QString jsonString = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);
	if (obj == MYSELF)
	{
		this->page()->runJavaScript(QString("appendHtml0(%1)").arg(jsonString));
		emit signalSendMsg(jsonMsg);     //�����źŽ���Ϣ���ݸ�TalkWindowShell
	}
	else
	{
		this->page()->runJavaScript(QString("recvHtml_%1(%2)").arg(obj).arg(jsonString));
	}
}

/*
* @brief  ����json��Ϣ
* @param  arr: json��Ϣ����
* @return msg: ���������Ϣ�ַ���
*/
QString MsgWebView::parseMsgFromJson(const QJsonArray& arr)
{
	QString msg;        //��Ϣ�ַ���
	for (int i = 0; i < arr.count(); i++)
	{
		QJsonArray blockArray = arr[i].toArray();           //ͨ���±��ȡ�ı���json����
		for (int j = 0; j < blockArray.count(); j++)        //��װ�ı���
		{
			QJsonObject obj = blockArray[j].toObject();     //Ƭ��json����
			QString key = obj.keys().at(0);                
			if (key == TXT)
			{
				msg += obj[key].toString();      //�ı�
			}
			else if (key == IMG)
			{
				QString value = QString(IMG_SRC).arg(IMG_QRC + obj[key].toString() + IMG_PNG);
				msg += value;
			}
		}
		msg += BR;       //����
	}

	//ɾ��ĩβ��<br>���� HTML �У������ͨ�������λ���ĵ�ĩβ�� <br> ��ǩ
	//int pos = msg.indexOf(BR, -4, Qt::CaseSensitive);     //�ӵ�����4���ַ���ʼ��<br>����Сд����
	//msg.truncate(pos);         //��pos���ض��ַ���

	return msg;
}
