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

void UserLogin::on_loginBtn_clicked()
{
    QString account = ui.editUserAccount->text();      //�˺�
    QString password = ui.editPassword->text();        //����

    if (account.isEmpty() || password.isEmpty())       //û�������˺�����
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�������˺Ż�����"));
        return;
    }

    //��֤�˺�����
    if (!UseMySQL::instance()->verifyLogin(account, password))
    {
        QMessageBox::information(this, QString::fromLocal8Bit("��ʾ"), QString::fromLocal8Bit("�˺Ż��������"));
        return;
    }

    //��ס����
    if (ui.checkBox->isChecked())        
    {
        ConfigIni::saveUserProfile(account, password);
    }

    //�رյ�¼����
    this->close();

    //��������
    CCMainWindow* mainwindow = new CCMainWindow(account);
    mainwindow->show();
}
