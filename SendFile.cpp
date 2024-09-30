#include "SendFile.h"
#include <qfiledialog.h>
#include <qfile.h>
#include <qmessagebox.h>
#include "qjsonobject.h"
#include <qfileinfo.h>
#include "WindowManager.h"
SendFile::SendFile(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	init();
}

SendFile::~SendFile()
{
}

void SendFile::init()
{
	initTitleBar(CLOSE_BUTTON, ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("SendFile");
	this->move(50, 400);

	connect(this, SIGNAL(signalSendFile(QJsonValue)), WindowManager::instance()->getTalkWindowShell(), SLOT(sendMsgToServer(QJsonValue)));
}

void SendFile::on_sendBtn_clicked()
{
	if (filePath.isEmpty()) return;

	QFile file(filePath);
	if (!file.open(QFile::ReadOnly))
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("���ļ� %1 ʧ��").arg(filePath));
		ui.lineEdit->clear();
		filePath = "";
		return;
	}
	QJsonObject fileObj;
	QString fileName = QFileInfo(filePath).fileName();	//�ļ�����
	fileObj.insert(FILE_NAME, fileName);

	QByteArray fileContent = file.readAll();
	fileObj.insert(FILE_LENGTH, QString::number(fileContent.length()));	//�ļ�����
	fileObj.insert(FILE_CONTENT, QString(fileContent));
	
	emit signalSendFile(fileObj);
	file.close();
	this->close();

}

void SendFile::on_openBtn_clicked()
{
	filePath = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ���ļ�"), "/", QString::fromLocal8Bit("�����ļ�(*.*)"));
	ui.lineEdit->setText(filePath);
}
