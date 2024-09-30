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
#include "QQTcpSocket.h"//todo 删除
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

//初始化头像
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
    if (ConfigIni::getUserProfile(account, password))    //成功获取到账号密码
    {
        ui.editUserAccount->setText(account);
        ui.editPassword->setText(password);
    }
}

void UserLogin::on_tcp_login_response(bool success, const QString& reason, User*user)
{
	if (success) {
		QString account = ui.editUserAccount->text();      //账号
		QString password = ui.editPassword->text();        //密码

		//User user;
		//user
		//记住密码
		if (ui.checkBox->isChecked())
		{
			ConfigIni::saveUserProfile(account, password);
		}

		//关闭登录窗体
		this->close();
		CCMainWindow::user = user;

		//打开主窗体
		CCMainWindow* mainwindow = new CCMainWindow(account);
		mainwindow->show();
	}
	else
	{
		QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("登录失败！原因：") + reason);
	}
}

void UserLogin::on_loginBtn_clicked()
{
    QString account = ui.editUserAccount->text();      //账号
    QString password = ui.editPassword->text();        //密码

    if (account.isEmpty() || password.isEmpty())       //没有输入账号密码
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请输入账号或密码"));
        return;
    }

	if (!ChatSocket::instance()->isConnected())      //未连接状态
	{
		QMessageBox::information(nullptr, "error", "TCP socket connect Error.");
		return;
	}

	QJsonObject userJsonObj;
	userJsonObj.insert("username", account);
	userJsonObj.insert("password", password);
	QJsonDocument doc(userJsonObj);
	QByteArray jsonString = doc.toJson(QJsonDocument::Compact); // 或者使用 QJsonDocument::Indented

	ChatSocket::instance()->sendText(ET_LoginRequest, jsonString);
}
