#include "ContactItem.h"

ContactItem::ContactItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

ContactItem::~ContactItem()
{
}

//�����û���
void ContactItem::setUserName(const QString & userName)
{
	ui.userName->setText(userName);
}

//����ǩ��
void ContactItem::setSignName(const QString& signName)
{
	ui.signName->setText(signName);
}

//����ͷ��
void ContactItem::setHeadPixmap(const QPixmap& pixmap)
{
	ui.label->setPixmap(pixmap);
}

//��ȡ�û���
QString ContactItem::getUserName()
{
	return ui.userName->text();
}

//��ȡͷ���С
QSize ContactItem::getHeadLabelSize()
{
	return ui.label->size();
}
