#pragma once

#include <QDialog>
#include "ui_UserLogin.h"
#include "BasicWindow.h"

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

private:
    Ui::UserLoginClass ui;
};
