#include "EmotionWindow.h"
#include "CommonUtils.h"
#include <qpainter.h>

EmotionWindow::EmotionWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	/*
		Qt::SubWindowû��������ͼ��
		Qt::SubWindow �ʺ�����Ƕ�������������ڲ����Ӵ��ڣ����ᱻ���ڹ�������Ϊ�������ڣ�
		Qt::Tool �ʺ����������ĸ������ߴ���
	*/
	setWindowFlags(Qt::FramelessWindowHint | Qt::SubWindow);        //�ޱ߿�/�Ӵ���
	setAttribute(Qt::WA_TranslucentBackground, true);    //͸��

	initControl();
}

EmotionWindow::~EmotionWindow()
{
}

void EmotionWindow::initControl()
{
	CommonUtils::loadStyleSheet(this, "EmotionWindow");

	//168������
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
	opt.initFrom(this);      //��ȡ�ؼ��ĵ�ǰ��ʽ

	QPainter p(this);
	/*
		QStyle::PE_Widget������һ�����ڲ���
		opt����������ʽѡ��
		p������ʹ�õĻ���
		this�����ƿؼ�
	*/
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);   //�ֶ�������ʽ����صĻ��Ʒ�����ȷ����ʽ��������Ч
}
