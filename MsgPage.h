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
	    QString msgRHtmlTmpl动态属性与msgRight数据成员相关联
		访问或修改 msgRHtmlTmpl 动态属性的值时，实际上是在访问或修改 msgRight 数据成员的值
	*/
	Q_PROPERTY(QString msgRHtmlTmpl MEMBER msgRight NOTIFY msgHtmlChanged)      //发送消息
	Q_PROPERTY(QString msgLHtmlTmpl MEMBER msgLeft NOTIFY msgHtmlChanged)       //接收消息

public:
	MsgHtmlObj(QObject *parent, QString headPath = QString());
	~MsgHtmlObj();

signals:
	void msgHtmlChanged(const QString& msgHtml);        //抑制警告

private:
	void initMsgHtml();       //初始化消息html模板
	QString getMsgHtml(const QString& fileName);       //获取消息html模板

private:
	QString msgRight;
	QString msgLeft;
	QString leftHeadPath;     //左边用户头像（即接收消息的头像）
};

/*
	QWebEnginePage主要负责管理和控制网页的内容、行为以及与外部的交互
	它是网页内容的逻辑部分，处理诸如导航、加载、JavaScript交互、网络请求等操作
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
		用于控制网页导航请求的接受或拒绝
		url: 当前导航请求的目标 URL
		type: 导航的类型，如点击链接、输入地址等
		isMainFrame: 指示导航是否在主框架（Main Frame）中发生
	*/ 
	bool acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame) override
	{
	/*
		在 Qt 应用程序的 C++ 代码中，使用 :/ 前缀来访问资源文件
		在 HTML 或 Web 内容中，使用 qrc:/ 前缀来引用 Qt 资源文件，为资源文件路径提供明确指示
		确保在不同的上下文中正确引用资源
	*/
		if (url.scheme() == "qrc")      //仅允许导航到 qrc 资源路径中的 .html 文件    "qrc:/*.html"
		{
			return true;      //允许导航
		}
		return false;
	}
};

/*
	QWebEngineView负责将网页内容显示给用户
	QWebEngineView内部包含一个 QWebEnginePage，默认情况下，它自动管理页面的加载和呈现
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
	void registerChannelObject();   //注册通道对象
	QString parseMsgFromJson(const QJsonArray& arr);

private:
	MsgHtmlObj* rightObj;       //发送消息的对象
	QWebChannel* channel;       //与网页进行通信的通道
};