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
	ui.tclosebtn->setVisible(false);      //隐藏关闭按钮
	connect(ui.tclosebtn, &QPushButton::clicked, this, &TalkWindowItem::signalCloseBtnClicked);        //信号连接信号
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