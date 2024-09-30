#include "SysTrayIcon.h"
#include <qwidget.h>
#include "CustomMenu.h"

//QSystemTrayIcon为应用程序提供一个系统托盘图标
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
	setToolTip("QtQQ");      //设置系统托盘图标的提示信息
	setIcon(QIcon(":/Resources/MainWindow/app/logo.ico"));      //设置系统托盘图标

	//activated(QSystemTrayIcon::ActivationReason reason) 当用户点击系统托盘图标时发出此信号
	connect(this, &QSystemTrayIcon::activated, this, &SysTrayIcon::onIconActivated);
}

void SysTrayIcon::popupMenu()
{
	//初始化菜单
	CustomMenu* menu = new CustomMenu(parent);
	menu->addMenuAction("show", ":/Resources/MainWindow/app/logo.ico", QString::fromLocal8Bit("显示"));
	menu->addMenuAction("quit", ":/Resources/MainWindow/app/page_close_btn_hover.png", QString::fromLocal8Bit("退出"));

	//连接信号与槽
	connect(menu->getMenuAction("show"), SIGNAL(triggered()), parent, SLOT(onShow()));
	connect(menu->getMenuAction("quit"), SIGNAL(triggered()), parent, SLOT(onQuit()));

	//弹出菜单
	menu->exec(QCursor::pos());       //以全局屏幕坐标返回主屏幕光标的位置
	menu->deleteLater();              //释放菜单
}

void SysTrayIcon::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)        //左键单击
	{
		parent->show();
		parent->activateWindow();
	}
	else if (reason == QSystemTrayIcon::Context)   //右键单击
	{
		popupMenu();
	}
}
