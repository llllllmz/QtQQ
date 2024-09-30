#pragma once

#include <QObject>
#include <qwebenginepage.h>
#include <QUrl>
#include <QWebEngineView>
#include <qwebchannel.h>

class MsgHtmlObj  : public QObject
{
	Q_OBJECT

	/*
	    QString msgRHtmlTmpl��̬������msgRight���ݳ�Ա�����
		���ʻ��޸� msgRHtmlTmpl ��̬���Ե�ֵʱ��ʵ�������ڷ��ʻ��޸� msgRight ���ݳ�Ա��ֵ
	*/
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER msgRight NOTIFY msgHtmlChanged)      //������Ϣ
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER msgLeft NOTIFY msgHtmlChanged)       //������Ϣ

public:
	MsgHtmlObj(QObject *parent, QString headPath = QString());
	~MsgHtmlObj();

signals:
	void msgHtmlChanged(const QString& msgHtml);        //���ƾ���

private:
	void initMsgHtml();       //��ʼ����Ϣhtmlģ��
	QString getMsgHtml(const QString& fileName);       //��ȡ��Ϣhtmlģ��

private:
	QString msgRight;
	QString msgLeft;
	QString leftHeadPath;     //����û�ͷ�񣨼�������Ϣ��ͷ��
};

/*
	QWebEnginePage��Ҫ�������Ϳ�����ҳ�����ݡ���Ϊ�Լ����ⲿ�Ľ���
	������ҳ���ݵ��߼����֣��������絼�������ء�JavaScript��������������Ȳ���
*/
class MsgWebPage : public QWebEnginePage
{
	Q_OBJECT

public:
	MsgWebPage(QObject* parent = nullptr) : QWebEnginePage(parent)
	{
	}

protected:
	/*
		���ڿ�����ҳ��������Ľ��ܻ�ܾ�
		url: ��ǰ���������Ŀ�� URL
		type: ���������ͣ��������ӡ������ַ��
		isMainFrame: ָʾ�����Ƿ�������ܣ�Main Frame���з���
	*/ 
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override
	{
	/*
		�� Qt Ӧ�ó���� C++ �����У�ʹ�� :/ ǰ׺��������Դ�ļ�
		�� HTML �� Web �����У�ʹ�� qrc:/ ǰ׺������ Qt ��Դ�ļ���Ϊ��Դ�ļ�·���ṩ��ȷָʾ
		ȷ���ڲ�ͬ������������ȷ������Դ
	*/
		if (url.scheme() == "qrc")      //���������� qrc ��Դ·���е� .html �ļ�    "qrc:/*.html"
		{
			return true;      //������
		}
		return false;
	}
};

/*
	QWebEngineView������ҳ������ʾ���û�
	QWebEngineView�ڲ�����һ�� QWebEnginePage��Ĭ������£����Զ�����ҳ��ļ��غͳ���
*/

class MsgWebView : public QWebEngineView
{
	Q_OBJECT

public:
	MsgWebView(QWidget* parent = nullptr);
	~MsgWebView();

	void appendMsg(const QJsonArray& jsonMsg, QString obj);

signals:
	void signalSendMsg(QJsonValue data);   

private:
	void registerChannelObject();   //ע��ͨ������
	QString parseMsgFromJson(const QJsonArray& arr);

private:
	MsgHtmlObj* rightObj;       //������Ϣ�Ķ���
	QWebChannel* channel;       //����ҳ����ͨ�ŵ�ͨ��
};