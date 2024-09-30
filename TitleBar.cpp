#include "TitleBar.h"
#include "CommonUtils.h"
#include <qpixmap.h>
#include <QMouseEvent>

TitleBar::TitleBar(ButtonType type, QWidget *parent)
	: btnType(type), QWidget(parent)
{
	ui.setupUi(this);

	init();
}

TitleBar::~TitleBar()
{
}

void TitleBar::init()
{
	setTitleButton();
	CommonUtils::loadStyleSheet(this, "Title");

	//初始化数据成员
	moveAble = ui.buttonMax->isVisible();       //最大化按钮可见的情况下才可以移动
	initConnect();
}

//设置标题栏图标
void TitleBar::setTitleIcon(const QString & filePath)
{
	QPixmap pix(filePath);

	ui.iconLabel->setFixedSize(pix.size());
	ui.iconLabel->setPixmap(pix);
}

//设置标题栏内容
void TitleBar::setTitleContent(const QString & titleContent)
{
	ui.titleContent->setText(titleContent);
}

//设置标题栏宽度
void TitleBar::setTitleWidth(int width)
{
	this->setFixedWidth(width);
}

//保存窗体最大化前位置和尺寸
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	restorePos = point;
	restoreSize = size;
}

//获取窗体最大化前位置和尺寸
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = restorePos;
	size = restoreSize;
}

//返回窗体是否为最大化状态
bool TitleBar::windowIsMax()
{
	if (btnType == MIN_MAX_CLOSE_BUTTON)
	{
		if (!ui.buttonMax->isVisible())       //最大化按钮不可见，则为最大化状态
		{
			return true;
		}
	}

	return false;         //默认为非最大化状态
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	//if (btnType == MIN_MAX_CLOSE_BUTTON)
	//{
	//	//在窗口最大化时禁止拖动窗口
	//	if (ui.buttonMax->isVisible())
	//	{
	//		moveAble = true;
	//		startMovePos = event->globalPos();        //globalPos() 返回事件发生时的全局坐标
	//	}
	//}
	//else
	//{
	//	moveAble = true;
	//	startMovePos = event->globalPos();
	//}

	if (!windowIsMax())
	{
		moveAble = true;
		startMovePos = event->globalPos();

		event->accept();
	}

	//在Qt中重写父类事件之后，通常是需要调用父类的事件函数的, 尤其是在你希望保留父类事件处理行为的情况下
	//这样做可以确保事件处理链的完整性，不会打破Qt的事件分发机制。
	//return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent * event)
{
	if (moveAble)
	{
		QPoint movePoint = event->globalPos() - startMovePos;     //鼠标移动的全局点坐标
		QPoint windowPoint = parentWidget()->pos();               //父窗体坐标
		QPoint windowMovePoint = windowPoint + movePoint;         //父窗体需要移动的坐标
		parentWidget()->move(windowMovePoint);
		
		startMovePos = event->globalPos();          //更新开始移动点坐标

		event->accept();
	}

	//return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent * event)
{
	moveAble = false;       //移动结束

	event->accept();
	//return QWidget::mouseReleaseEvent(event);
}

//鼠标双击事件控制窗体最大化、还原
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	//只要存在最大化或最小化按钮时才有效
	if (btnType == MIN_MAX_CLOSE_BUTTON)
	{
		if (ui.buttonMax->isVisible())        //双击时，最大化按钮可见则最大化
		{
			onButtonMaxClicked();
		}
		else        //还原按钮可见则还原
		{
			onButtonRestoreClicked();
		}
	}

	return QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::paintEvent(QPaintEvent * event)
{
	//QWidget::paintEvent(event);     QWidget的绘图事件是空函数

	if (width() != parentWidget()->width())
	{
		setFixedWidth(parentWidget()->width());
	}
}

//设置标题栏按钮
void TitleBar::setTitleButton()
{
	switch (btnType)
	{
	case MIN_CLOSE_BUTTON:
	{
		ui.buttonRestore->setVisible(false);     //setVisible，部件是否可见
		ui.buttonMax->setVisible(false);         
		break;
	}
	case MIN_MAX_CLOSE_BUTTON:
	{
		ui.buttonRestore->setVisible(false);
		break;
	}
	case CLOSE_BUTTON:
	{
		ui.buttonMin->setVisible(false);
		ui.buttonRestore->setVisible(false);
		ui.buttonMax->setVisible(false);
		break;
	}
	default:
		break;
	}
}

void TitleBar::initConnect()
{
	connect(ui.buttonMin, &QPushButton::clicked, this, &TitleBar::buttonMinClicked);
	connect(ui.buttonRestore, &QPushButton::clicked, this, &TitleBar::onButtonRestoreClicked);
	connect(ui.buttonMax, &QPushButton::clicked, this, &TitleBar::onButtonMaxClicked);
	connect(ui.buttonClose, &QPushButton::clicked, this, &TitleBar::buttonCloseClicked);
}

//响应还原按钮点击信号
void TitleBar::onButtonRestoreClicked()
{
	ui.buttonRestore->setVisible(false);
	ui.buttonMax->setVisible(true);

	emit buttonRestoreClicked();
}

//响应最大化按钮点击信号
void TitleBar::onButtonMaxClicked()
{
	ui.buttonMax->setVisible(false);
	ui.buttonRestore->setVisible(true);

	emit buttonMaxClicked();
}
