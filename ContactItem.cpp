#include "ContactItem.h"

ContactItem::ContactItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ContactItem::~ContactItem()
{
}

//设置用户名
void ContactItem::setUserName(const QString & userName)
{
	ui.userName->setText(userName);
}

//设置签名
void ContactItem::setSignName(const QString& signName)
{
	ui.signName->setText(signName);
}

//设置头像
void ContactItem::setHeadPixmap(const QPixmap& pixmap)
{
	ui.label->setPixmap(pixmap);
}

//获取用户名
QString ContactItem::getUserName()
{
	return ui.userName->text();
}

//获取头像大小
QSize ContactItem::getHeadLabelSize()
{
	return ui.label->size();
}
