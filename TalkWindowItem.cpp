#include "TalkWindowItem.h"
#include "CommonUtils.h"

TalkWindowItem::TalkWindowItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	init();
}

TalkWindowItem::~TalkWindowItem()
{
}

void TalkWindowItem::init()
{
	ui.tclosebtn->setVisible(false);      //���عرհ�ť
	connect(ui.tclosebtn, &QPushButton::clicked, this, &TalkWindowItem::signalCloseBtnClicked);        //�ź������ź�
}

void TalkWindowItem::setHeadPixmap(const QString& headPath)
{
	QPixmap head(headPath);
	QPixmap mask(":/Resources/MainWindow/head_mask.png");

	ui.headLabel->setPixmap(CommonUtils::getRoundedPixmap(head, mask, ui.headLabel->size()));
}

void TalkWindowItem::setMsgText(const QString& msg)
{
	ui.msgLabel->setText(msg);
}

void TalkWindowItem::enterEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(true);

	return QWidget::enterEvent(event);
}

void TalkWindowItem::leaveEvent(QEvent* event)
{
	ui.tclosebtn->setVisible(false);
	return QWidget::leaveEvent(event);
}