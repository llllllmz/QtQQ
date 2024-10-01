#pragma once

#include "BasicWindow.h"
#include "ui_CCMainWindow.h"
#include "SkinWindow.h"
#include "User.h"
#include "UserGroup.h"
#include "FriendshipDTO.h"
#include <QTreeWidgetItem>
#include "QMap"
class CCMainWindow : public BasicWindow
{
	Q_OBJECT

public:
	CCMainWindow(const QString& account, QWidget *parent = nullptr);
	~CCMainWindow();

private:
	void init();
	void initButton();
	void initConnect();
	void initAppButton();
	void initLevelButton();
	void initContact();
	void initSysTray();

	void drawButtonIcon(QPushButton*& btn, const QString& icon);
	QPushButton* addOtherButton(const QString& icon, const QString& objName, QSize size = QSize(20, 20));
	void drawLevelButton(int level);

	void updateSearchStyle(bool hasFocus);

	void setUserName();
	void setUserHeadPixmap();

	void addUserGroup(QTreeWidgetItem* rootItem);

protected:
	bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
	void onButtonClicked();

	void onTreeItemClicked(QTreeWidgetItem* item);
	void onTreeItemDoubleClicked(QTreeWidgetItem* item);
	void onTreeItemExpanded(QTreeWidgetItem* item);
	void onTreeItemCollapsed(QTreeWidgetItem* item);

	void onQuit();

	void on_tcp_user_group_response(QMap<int, UserGroup*>);
	void on_tcp_friend_ship_response(QList<FriendshipDTO*> friendships);
private:
	Ui::CCMainWindowClass ui;

	QString account;			//账号 todo可以用User取代吗？

public:
	static User* user;

	QMap<int, UserGroup*> m_userGroups;
	QMap<int, QTreeWidgetItem*> m_treeWidgetItem;
};
