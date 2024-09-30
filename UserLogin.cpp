#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonObject>
#include <QDebug>
#include "UserLogin.h"
#include "CommonUtils.h"
#include <qmessagebox.h>
#include "ConfigIni.h"
#include "UseMySQL.h"
#include "CCMainWindow.h"
#include <QtEndian>
#include "QQTcpSocket.h"//todo ɾ��
#include "ChatSocket.h"
#include "User.h"
UserLogin::UserLogin(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);

    //setWindowFlags(windowFlags() | Qt::Tool);
	ChatSocket::instance()->connectToHost("192.168.58.204",2007);
	connect(ChatSocket::instance(), &ChatSocket::login_response, this, &UserLogin::on_tcp_login_response);

    init();
}

UserLogin::~UserLogin()
{
}

void UserLogin::init()
{
    initTitleBar(MIN_CLOSE_BUTTON, ":/Resources/MainWindow/qqlogoclassic.png");
    loadStyleSheet("UserLogin");
    initPicture();
    initUser();
    UseMySQL::instance();
}

//��ʼ��ͷ��
void UserLogin::initPicture()
{
    QLabel* head = new QLabel(this);
    head->setFixedSize(68, 68);
    head->move(width() / 2 - 34, height() / 2 - 90);

    QPixmap src(":/Resources/MainWindow/app/logo.ico");
    QPixmap mask(":/Resources/MainWindow/head_mask.png");
    head->setPixmap(CommonUtils::getRoundedPixmap(src, mask, head->size()));
}

void UserLogin::initUser()
{
    QString account, password;
    if (ConfigIni::getUserProfile(account, password))    //�ɹ���ȡ���˺�����
    {
        ui.editUserAccount->setText(account);
        ui.editPassword->setText(password);
    }
}

void UserLogin::on_tcp_login_response(bool success, const QString& reason, User*user)
{
	if (success) {
		QString account = ui.editUserAccount->text();      //�˺�
		QString password = ui.editPassword->text();        //����

		//User user;
		//user
		//��ס����
		if (ui.checkBox->isChecked())
		{
			ConfigIni::saveUserProfile(account, password);
		}

		//�رյ�¼����
		this->close();
		CCMainWindow::user = user;

		//��������
		CCMainWindow* mainwindow = new CCMainWindow(account);
		mainwindow->show();
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("��¼ʧ�ܣ�ԭ��") + reason);
	}
}

void UserLogin::on_loginBtn_clicked()
{
    QString account = ui.editUserAccount->text();      //�˺�
    QString password = ui.editPassword->text();        //����

    if (account.isEmpty() || password.isEmpty())       //û�������˺�����
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�������˺Ż�����"));
        return;
    }

	if (!ChatSocket::instance()->isConnected())      //δ����״̬
	{
		QMessageBox::information(nullptr, "error", "TCP socket connect Error.");
		return;
	}

	QJsonObject userJsonObj;
	userJsonObj.insert("username", account);
	userJsonObj.insert("password", password);
	QJsonDocument doc(userJsonObj);
	QByteArray jsonString = doc.toJson(QJsonDocument::Compact); // ����ʹ�� QJsonDocument::Indented

	ChatSocket::instance()->sendText(ET_LoginRequest, jsonString);
}
