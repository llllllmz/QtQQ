#pragma once

#include <QWidget>
#include "ui_TalkWindow.h"

class TalkWindow : public QWidget
{
	Q_OBJECT

public:
	TalkWindow(const int& id, QWidget *parent = nullptr);
	~TalkWindow();

private:
	void init();
	//void initTalkType();
	void initStyle();
	void initConnect();
	void initTalkWindow();
	void initGroupTalk();
	void initSingleTalk();

	QJsonArray parseDocumentToJson(QTextDocument* document);
	void addMember(QTreeWidgetItem* rootItem,const QPair<QString,QString>& info);

public:
	void setWindowName(const QString& name);
	void addEmotionImage(int name);
	int getTalkId();
	void appendObjectMsg(const QJsonArray& jsonMsg, QString obj);

private slots:
	void onSendBtnClicked();
	void onTreeItemDoubleClicked(QTreeWidgetItem*item);
	void onFileOpenBtnClicked();

private:
	Ui::TalkWindow ui;
	const int talkId;
	//bool isGroupTalk;      //�Ƿ�ΪȺ��
};
