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

		if (fileLength.toInt() == fileContent.toUtf8().length())//文件长度和文件内容长度相等
		{
			QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择文件保存路径"), "/");
			if (!dir.isEmpty())
			{
				QString filePath = dir + "/" + fileName;
				QFile file(filePath);
				if (file.open(QFile::WriteOnly))
				{
					QTextStream stream(&file);
					stream << fileContent;
					file.close();
					QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("接收文件 %1 成功").arg(fileName));
					this->close();
					return;
				}
			}
		}
	}
	QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("接收文件失败。"));
	this->close();
}

void RecvFile::on_cancelBtn_clicked()
{
	this->close();
}
