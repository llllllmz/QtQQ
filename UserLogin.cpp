#include "UserLogin.h"
#include "CommonUtils.h"
#include <qmessagebox.h>
#include "ConfigIni.h"
#include "UseMySQL.h"
#include "CCMainWindow.h"

UserLogin::UserLogin(QWidget *parent)
    : BasicWindow(parent)
{
    ui.setupUi(this);

    //setWindowFlags(windowFlags() | Qt::Tool);

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

void UserLogin::on_loginBtn_clicked()
{
    QString account = ui.editUserAccount->text();      //账号
    QString password = ui.editPassword->text();        //密码

    if (account.isEmpty() || password.isEmpty())       //没有输入账号密码
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请输入账号或密码"));
        return;
    }

    //验证账号密码
    if (!UseMySQL::instance()->verifyLogin(account, password))
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("账号或密码错误"));
        return;
    }

    //记住密码
    if (ui.checkBox->isChecked())        
    {
        ConfigIni::saveUserProfile(account, password);
    }

    //关闭登录窗体
    this->close();

    //打开主窗体
    CCMainWindow* mainwindow = new CCMainWindow(account);
    mainwindow->show();
}
