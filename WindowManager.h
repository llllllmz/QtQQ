#pragma once

#include <QObject>
#include "TalkWindowShell.h"

class WindowManager  : public QObject
{
	Q_OBJECT

public:
	WindowManager(QObject *parent = nullptr);
	~WindowManager();

public:
	static WindowManager* instance();

	void addNewTalkWindow(const int id);
	QWidget* findWindowMap(const int& id);
	void addWindowMap(const int& id, QWidget* widget);
	void removeWindowMap(const int& id);

	TalkWindowShell* getTalkWindowShell() const;
	int getCreateID() const;

private:
	TalkWindowShell* talkWindowShell;

	/*
	* Key: id
	* Value: widget
	*/
	QMap<int, QWidget*> map;
	int createID;    //正在创建的窗口id
};
