#include "RecvFile.h"
#include "WindowManager.h"
#include "qfiledialog.h"
#include "qfile.h"
#include "qtextstream.h"
#include "qmessagebox.h"
RecvFile::RecvFile(const QJsonObject& data, QWidget *parent)
	: BasicWindow(parent),fileObj(data)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	init();
}

RecvFile::~RecvFile()
{
}

void RecvFile::setLabelMsg(const QString & msg)
{
	ui.label->setText(msg);
}

void RecvFile::init()
{
	initTitleBar(CLOSE_BUTTON, ":/Resources/MainWindow/qqlogoclassic.png");
	loadStyleSheet("ReceiveFile");
	this->move(200, 400);


}

void RecvFile::on_okBtn_clicked()
{
	if (!fileObj.isEmpty())
	{
		QString fileName = fileObj[FILE_NAME].toString();
		QString fileLength = fileObj[FILE_LENGTH].toString();
		QString fileContent = fileObj[FILE_CONTENT].toString();

		if (fileLength.toInt() == fileContent.toUtf8().length())//�ļ����Ⱥ��ļ����ݳ������
		{
			QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ���ļ�����·��"), "/");
			if (!dir.isEmpty())
			{
				QString filePath = dir + "/" + fileName;
				QFile file(filePath);
				if (file.open(QFile::WriteOnly))
				{
					QTextStream stream(&file);
					stream << fileContent;
					file.close();
					QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�����ļ� %1 �ɹ�").arg(fileName));
					this->close();
					return;
				}
			}
		}
	}
	QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�����ļ�ʧ�ܡ�"));
	this->close();
}

void RecvFile::on_cancelBtn_clicked()
{
	this->close();
}
