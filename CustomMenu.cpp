#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);      //透明背景
	CommonUtils::loadStyleSheet(this, "Menu");
}

CustomMenu::~CustomMenu()
{
}

/*
功能: 添加菜单项
参数: name: 菜单项名称
	  icon: 菜单项图标
	  text: 菜单项文本
*/
void CustomMenu::addMenuAction(const QString& name, const QString& icon, const QString& text)
{
	QAction* action = this->addAction(QIcon(icon), text);
	map.insert(name, action);     
}

QAction* CustomMenu::getMenuAction(const QString& name)
{
	return map[name];
}

