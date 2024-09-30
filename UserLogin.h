#pragma once

#include <QDialog>
#include "ui_UserLogin.h"
#include "BasicWindow.h"
#include <QTcpSocket>
#include <QHostAddress>
#include "User.h"
class UserLogin : public BasicWindow
{
    Q_OBJECT

public:
    UserLogin(QWidget *parent = Q_NULLPTR);
    ~UserLogin();

private:
    void init();
    void initPicture();
    void initUser();

private slots:
    void on_loginBtn_clicked();
	void on_tcp_login_response(bool, const QString&, User*);

private:
    Ui::UserLoginClass ui;
};
