#include "BasicWindow.h"
#include <qdesktopwidget.h>
#include <qscreen.h>
#include <qdebug.h>
#include <qpainter.h>
#include <QMouseEvent>
#include "ConfigIni.h"
#include "CommonUtils.h"
#include "NotifyManager.h"

BasicWindow::BasicWindow(QWidget *parent)
	: QDialog(parent), moveAble(false), titleBar(nullptr)
{
	setWindowFlags(Qt::FramelessWindowHint);

	skinColor = ConfigIni::getColor();

	//改变皮肤颜色
	connect(NotifyManager::instance(), &NotifyManager::skinChanged, this, &BasicWindow::onSkinChanged);
}

BasicWindow::~BasicWindow()
{
}

void BasicWindow::initTitleBar(ButtonType buttonType, const QString & filePath, const QString & titleContent)
{
	titleBar = new TitleBar(buttonType, this);
	titleBar->setTitleIcon(filePath);
	titleBar->setTitleContent(titleContent);
	titleBar->move(0, 0);         //移动到窗体坐标（0，0）

	connect(titleBar, &TitleBar::buttonMinClicked, this, &BasicWindow::onMin);
	connect(titleBar, &TitleBar::buttonMaxClicked, this, &BasicWindow::onMax);
	connect(titleBar, &TitleBar::buttonRestoreClicked, this, &BasicWindow::onRestore);
	connect(titleBar, &TitleBar::buttonCloseClicked, this, &BasicWindow::onClose);
}

//加载样式表
void BasicWindow::loadStyleSheet(const QString& fileName)
{
	styleFileName = fileName;        //保存样式表文件名

	int r = skinColor.red();
	int g = skinColor.green();
	int b = skinColor.blue();

	//background-color 背景色
	QString qss = QString("QWidget[titleskin=true] \
								  { background-color: rgb(%1, %2, %3); }    \
						   QWidget[bottomskin=true] \
 								  { background-color: rgba(%1, %2, %3, 50); }"
	).arg(r).arg(g).arg(b);

	CommonUtils::loadStyleSheet(this, styleFileName, qss);
}

void BasicWindow::paintEvent(QPaintEvent* event)
{
	QDialog::paintEvent(event);  //保留默认绘制行为，QDialog绘图事件函数为空函数

	//大多数类包含很多默认绘制行为，这些默认绘制行为有时会与样式表的设置产生冲突，导致样式表无法正确应用，所以要重写绘图事件
	
	QStyleOption opt;
	opt.initFrom(this);      //获取控件的当前样式

	QPainter p(this);
	/*
		QStyle::PE_Widget：绘制一个窗口部件
		opt：给定的样式选项
		p：绘制使用的画笔
		this：绘制控件
	*/
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);   //手动调用样式表相关的绘制方法，确保样式表设置生效
}

void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)    //如果触发事件的按钮是鼠标左键
	{
		if (!titleBar)          //没有标题栏，即无法最大化
		{
			moveAble = true;
			startMovePos = event->globalPos();
		}
		else
		{
			if (!titleBar->windowIsMax())
			{
				moveAble = true;
				startMovePos = event->globalPos();
			}
		}
	}

	return QDialog::mousePressEvent(event);
}

void BasicWindow::mouseMoveEvent(QMouseEvent* event)
{
	if (moveAble)
	{
		QPoint movePoint = event->globalPos() - startMovePos;
		QPoint windowPoint = this->pos();
		this->move(windowPoint + movePoint);

		startMovePos = event->globalPos();
	}

	return QDialog::mouseMoveEvent(event);
}

void BasicWindow::mouseReleaseEvent(QMouseEvent* event)
{
	moveAble = false;

	return QDialog::mouseReleaseEvent(event);
}

void BasicWindow::onMin()
{
	/*
		工具窗口与普通窗口的区别：
		1.工具窗口通常为非模态窗口，不会阻塞用户对其他窗口的操作    普通窗口通常为模态或非模态窗口
		2.工具窗口没有任务栏图标，且通常来说外观较小
		3.工具窗口不算做最后一个窗口，即最后一个普通窗口关闭后应用程序会直接退出
	*/

	//qDebug() << (windowFlags()/* & Qt::Tool*/);
	/*
		Qt::Tool = 11       1011
		Qt::Window = 1      0001
		1011 & 0001 = 0001
	*/

	if (Qt::Tool == (windowFlags() & Qt::Tool))      //如果该窗口是工具窗口，则隐藏
	{
		hide();
	}
	else       //如果该窗口不是工具窗口，则最小化
	{
		showMinimized();
	}
}

void BasicWindow::onRestore()
{
	QPoint pos;
	QSize size;
	titleBar->getRestoreInfo(pos, size);

	setGeometry(QRect(pos, size));       //更改几何图形（位置及大小）
}

void BasicWindow::onMax()
{
	titleBar->saveRestoreInfo(this->pos(), this->size());

	//screens()返回系统中所有屏幕的列表
	QRect desktopRect = QApplication::screens()[0]->availableGeometry();        //获取桌面可用的矩形区域

	setGeometry(desktopRect);
}

void BasicWindow::onClose()
{
	QObject* parent = sender()->parent();
	if (parent)
	{
		if (parent->objectName() == "UserLoginClass")
		{
			QApplication::quit();
			return;
		}
	}

	close();
}

void BasicWindow::onHide()
{
	hide();
}

void BasicWindow::onShow()
{
	//显示并设置为活动窗口
	show();
	activateWindow();        
}

//void BasicWindow::onQuit()
//{
//	QApplication::quit();
//}

void BasicWindow::onSkinChanged(const QColor& color)
{
	skinColor = color;
	loadStyleSheet(styleFileName);
}
