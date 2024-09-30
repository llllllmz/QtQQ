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
	void enterEvent(QEvent* event) override;    //������
	void leaveEvent(QEvent* event) override;    //����뿪

private:
	Ui::TalkWindowItem ui;
};
