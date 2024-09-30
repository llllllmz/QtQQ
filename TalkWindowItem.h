#pragma once

#include <QWidget>
#include "ui_TalkWindowItem.h"

class TalkWindowItem : public QWidget
{
	Q_OBJECT

public:
	TalkWindowItem(QWidget *parent);
	~TalkWindowItem();

private:
	void init();

signals:
	void signalCloseBtnClicked();

public:
	void setHeadPixmap(const QString& headPath);
	void setMsgText(const QString& msg);

protected:
	void enterEvent(QEvent* event) override;    //鼠标进入
	void leaveEvent(QEvent* event) override;    //鼠标离开

private:
	Ui::TalkWindowItem ui;
};
