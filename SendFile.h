#pragma once

#include "BasicWindow.h"
#include "ui_SendFile.h"
#include <qjsonvalue.h>
class SendFile : public BasicWindow
{
	Q_OBJECT

public:
	SendFile(QWidget *parent = Q_NULLPTR);
	~SendFile();
private:
	void init();

signals:
	void signalSendFile(QJsonValue data);

private slots:
	void on_openBtn_clicked();
	void on_sendBtn_clicked();
private:
	Ui::SendFile ui;
	QString filePath;
};
