#pragma once

#include <QMenu>
#include <qmap.h>

class CustomMenu : public QMenu
{
	Q_OBJECT

public:
	CustomMenu(QWidget *parent = nullptr);
	~CustomMenu();

public:
	void addMenuAction(const QString& name, const QString& icon, const QString& text);
	QAction* getMenuAction(const QString& name);

private:
	/*
		菜单项集合
		key: 菜单项名称
		value: 菜单项
	*/
	QMap<QString, QAction*> map;
};
