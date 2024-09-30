#pragma once

#include "BasicWindow.h"
#include "ui_RecvFile.h"
#include <QJsonObject>
class RecvFile : public BasicWindow
{
	Q_OBJECT

public:
	RecvFile(const QJsonObject& data,QWidget *parent = Q_NULLPTR);
	~RecvFile();

	void setLabelMsg(const QString& msg);
private:
	void init();

private slots:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();
private:
	Ui::ReceiveFile ui;
	QJsonObject fileObj;
};
