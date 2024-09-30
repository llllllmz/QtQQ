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
	setFixedHeight(32);       //�̶��߶�
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);     //ˮƽ/��ֱ����

	an = new QPropertyAnimation(this, "rotation", this);      //��ʼ�����Զ���, ��"rotation"����
	an->setDuration(30);      //��������ʱ��30ms
	an->setEasingCurve(QEasingCurve::InQuad);           //������������    //QEasingCurve::InQuad ����ʽ���ٵ��Ӿ�Ч��
}

void RootContactItem::setText(const QString& text)
{
	titleText = text;
	update();       //�ı����£������ػ�
}

void RootContactItem::setExpanded(bool expand)
{
	if (expand)
	{
		an->setEndValue(90);        //�������Զ����Ľ���ֵ
	}
	else
	{
		an->setEndValue(0);
	}

	an->start();       //��������
}

int RootContactItem::getRotation() const
{
	return rotation;
}

void RootContactItem::setRotation(int r)
{
	rotation = r;
	update();       //��ת�Ƕȸ��£������ػ�
}

void RootContactItem::paintEvent(QPaintEvent* event)
{
	QLabel::paintEvent(event);       //Ĭ�ϻ�����Ϊ

	//�����ı�
	QPainter p1(this);
	p1.setRenderHint(QPainter::TextAntialiasing, true);       //�ı������

	QFont font;
	font.setPointSize(10);       //�����С
	p1.setFont(font);
	p1.drawText(24, 0, width() - 24, height(), Qt::AlignLeft | Qt::AlignVCenter, titleText);      //��ԭ��Ϊ(24,0)���Ϊ(width()-24,height())�������ڻ����ı�

	//���Ƽ�ͷ
	if (hasArrow)       //������ڼ�ͷ
	{
		QPixmap arrow(":/Resources/MainWindow/arrow.png");
		QPixmap pixmap(arrow.size());        //��Ƭ
		pixmap.fill(Qt::transparent);        //���͸��

		QPainter p2(&pixmap);
		p2.setRenderHint(QPainter::SmoothPixmapTransform, true);      //���ͼƬ�����������

		p2.translate(pixmap.width() / 2, pixmap.height() / 2);        //ƽ�ƻ���ԭ�㵽ͼ������
		p2.rotate(rotation);                                          //������λ��˳ʱ����ת����ϵ 
		p2.drawPixmap(0 - pixmap.width() / 2, 0 - pixmap.height() / 2, arrow);        //����ԭ����pixmap����λ��
		
		p1.drawPixmap(6, (this->height() - pixmap.height()) / 2, pixmap);
	}
}
