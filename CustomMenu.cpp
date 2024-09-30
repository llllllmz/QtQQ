#include "CustomMenu.h"
#include "CommonUtils.h"

CustomMenu::CustomMenu(QWidget *parent)
	: QMenu(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);      //͸������
	CommonUtils::loadStyleSheet(this, "Menu");
}

CustomMenu::~CustomMenu()
{
}

/*
����: ��Ӳ˵���
����: name: �˵�������
	  icon: �˵���ͼ��
	  text: �˵����ı�
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

