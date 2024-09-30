#include "SysTrayIcon.h"
#include <qwidget.h>
#include "CustomMenu.h"

//QSystemTrayIconΪӦ�ó����ṩһ��ϵͳ����ͼ��
SysTrayIcon::SysTrayIcon(QWidget *parent)
	: QSystemTrayIcon(parent), parent(parent)
{
	init();
}

SysTrayIcon::~SysTrayIcon()
{
}

void SysTrayIcon::init()
{
	setToolTip("QtQQ");      //����ϵͳ����ͼ�����ʾ��Ϣ
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));      //����ϵͳ����ͼ��

	//activated(QSystemTrayIcon::ActivationReason reason) ���û����ϵͳ����ͼ��ʱ�������ź�
	connect(this, &QSystemTrayIcon::activated, this, &SysTrayIcon::onIconActivated);
}

void SysTrayIcon::popupMenu()
{
	//��ʼ���˵�
	CustomMenu* menu = new CustomMenu(parent);
	menu->addMenuAction("show", ":/Resources/MainWindow/app/logo.ico", QString::fromLocal8Bit("��ʾ"));
	menu->addMenuAction("quit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QString::fromLocal8Bit("�˳�"));

	//�����ź����
	connect(menu->getMenuAction("show"), SIGNAL(triggered()), parent, SLOT(onShow()));
	connect(menu->getMenuAction("quit"), SIGNAL(triggered()), parent, SLOT(onQuit()));

	//�����˵�
	menu->exec(QCursor::pos());       //��ȫ����Ļ���귵������Ļ����λ��
	menu->deleteLater();              //�ͷŲ˵�
}

void SysTrayIcon::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)        //�������
	{
		parent->show();
		parent->activateWindow();
	}
	else if (reason == QSystemTrayIcon::Context)   //�Ҽ�����
	{
		popupMenu();
	}
}
