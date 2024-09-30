#include "EmotionWindow.h"
#include "CommonUtils.h"
#include <qpainter.h>

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	/*
		Qt::SubWindow没有任务栏图标
		Qt::SubWindow 适合用于嵌入在其他窗口内部的子窗口（不会被窗口管理器视为独立窗口）
		Qt::Tool 适合用作独立的辅助工具窗口
	*/
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);        //无边框/子窗口
	setAttribute(Qt::WA_TranslucentBackground, true);    //透明

	initControl();
}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");

	//168个表情
	for (int row = 0; row < rowCount; row++)
	{
		for (int column = 0; column < columnCount; column++)
		{
			ui.gridLayout->addWidget(initLabel(row * columnCount + column), row, column);
		}
	}
}

EmotionLabel* EmotionWindow::initLabel(int name)
{
	EmotionLabel* label = new EmotionLabel(name, this);
	connect(label, &EmotionLabel::emotionClicked, this, &EmotionWindow::addEmotion);

	return label;
}

void EmotionWindow::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);

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
