#pragma once

#include <QWidget>
#include "ui_TalkWindowShell.h"
#include "BasicWindow.h"
#include <qmap.h>
#include "EmotionWindow.h"
#include <qtcpsocket.h>
#include <qjsonvalue.h>
#include <qudpsocket.h>
#include "TalkWindowItem.h"
#include "TalkWindow.h"

/*
//������  ��ֹͷ�ļ��໥����
class TalkWindow;
class TalkWindowItem;
*/

class TalkWindowShell : public BasicWindow
{
	Q_OBJECT

public:
	TalkWindowShell(QWidget *parent = nullptr);
	~TalkWindowShell();

private:
	void init();
	void initStyle();
	void initEmotionWindow();
	void initJsFile();
	bool replaceJsTxt(QString& fileContent);
	void initTcpSocket();
	void initUdpSocket();

public:
	void addTalkWindow(TalkWindow* window, TalkWindowItem* windowItem);
	void updateCurrentWindow(QWidget* window);

private slots:
	void onListItemClicked(QListWidgetItem* item);
	void onEmotionBtnClicked();
	void onAddEmotion(int name);

//public slots:
	void sendMsgToServer(QJsonValue data);
	void readMsgFromServer();

private:
	Ui::TalkWindowShellClass ui;

	QMap<QListWidgetItem*, QWidget*> map;    //�б����봰�ڵ�ӳ��
	EmotionWindow* emotionWindow;

	QTcpSocket* tcp;              //tcp����
	const int tcpPort = 57777;    //tcp�˿�

	QUdpSocket* udp;              //udp����
	const int udpPort = 59999;    //udp�˿�
};
