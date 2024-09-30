#include "RootContactItem.h"
#include <QPainter>

RootContactItem::RootContactItem(bool hasArrow, QWidget* parent)
	: QLabel(parent), hasArrow(hasArrow), rotation(0), an(nullptr)
{
	init();
}

RootContactItem::~RootContactItem()
{
}

void RootContactItem::init()
{
	setFixedHeight(32);       //固定高度
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);     //水平/垂直策略

	an = new QPropertyAnimation(this, "rotation", this);      //初始化属性动画, 绑定"rotation"属性
	an->setDuration(30);      //动画持续时间30ms
	an->setEasingCurve(QEasingCurve::InQuad);           //动画缓动曲线    //QEasingCurve::InQuad 渐进式加速的视觉效果
}

void RootContactItem::setText(const QString& text)
{
	titleText = text;
	update();       //文本更新，触发重绘
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand)
	{
		an->setEndValue(90);        //设置属性动画的结束值
	}
	else
	{
		an->setEndValue(0);
	}

	an->start();       //启动动画
}

int RootContactItem::getRotation() const
{
	return rotation;
}

void RootContactItem::setRotation(int r)
{
	rotation = r;
	update();       //旋转角度更新，触发重绘
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);       //默认绘制行为

	//绘制文本
	QPainter p1(this);
	p1.setRenderHint(QPainter::TextAntialiasing, true);       //文本抗锯齿

	QFont font;
	font.setPointSize(10);       //字体大小
	p1.setFont(font);
	p1.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, titleText);      //在原点为(24,0)宽高为(width()-24,height())的区域内绘制文本

	//绘制箭头
	if (hasArrow)       //如果存在箭头
	{
		QPixmap arrow(":/Resources/MainWindow/arrow.png");
		QPixmap pixmap(arrow.size());        //底片
		pixmap.fill(Qt::transparent);        //填充透明

		QPainter p2(&pixmap);
		p2.setRenderHint(QPainter::SmoothPixmapTransform, true);      //提高图片质量，抗锯齿

		p2.translate(pixmap.width() / 2, pixmap.height() / 2);        //平移绘制原点到图像中心
		p2.rotate(rotation);                                          //在中心位置顺时针旋转坐标系 
		p2.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, arrow);        //现在原点在pixmap中心位置
		
		p1.drawPixmap(6, (this->height() - pixmap.height()) / 2, pixmap);
	}
}
