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

	//��ʼ�����ݳ�Ա
	moveAble = ui.buttonMax->isVisible();       //��󻯰�ť�ɼ�������²ſ����ƶ�
	initConnect();
}

//���ñ�����ͼ��
void TitleBar::setTitleIcon(const QString & filePath)
{
	QPixmap pix(filePath);

	ui.iconLabel->setFixedSize(pix.size());
	ui.iconLabel->setPixmap(pix);
}

//���ñ���������
void TitleBar::setTitleContent(const QString & titleContent)
{
	ui.titleContent->setText(titleContent);
}

//���ñ��������
void TitleBar::setTitleWidth(int width)
{
	this->setFixedWidth(width);
}

//���洰�����ǰλ�úͳߴ�
void TitleBar::saveRestoreInfo(const QPoint& point, const QSize& size)
{
	restorePos = point;
	restoreSize = size;
}

//��ȡ�������ǰλ�úͳߴ�
void TitleBar::getRestoreInfo(QPoint& point, QSize& size)
{
	point = restorePos;
	size = restoreSize;
}

//���ش����Ƿ�Ϊ���״̬
bool TitleBar::windowIsMax()
{
	if (btnType == MIN_MAX_CLOSE_BUTTON)
	{
		if (!ui.buttonMax->isVisible())       //��󻯰�ť���ɼ�����Ϊ���״̬
		{
			return true;
		}
	}

	return false;         //Ĭ��Ϊ�����״̬
}

void TitleBar::mousePressEvent(QMouseEvent * event)
{
	//if (btnType == MIN_MAX_CLOSE_BUTTON)
	//{
	//	//�ڴ������ʱ��ֹ�϶�����
	//	if (ui.buttonMax->isVisible())
	//	{
	//		moveAble = true;
	//		startMovePos = event->globalPos();        //globalPos() �����¼�����ʱ��ȫ������
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

	//��Qt����д�����¼�֮��ͨ������Ҫ���ø�����¼�������, ����������ϣ�����������¼�������Ϊ�������
	//����������ȷ���¼��������������ԣ��������Qt���¼��ַ����ơ�
	//return QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent * event)
{
	if (moveAble)
	{
		QPoint movePoint = event->globalPos() - startMovePos;     //����ƶ���ȫ�ֵ�����
		QPoint windowPoint = parentWidget()->pos();               //����������
		QPoint windowMovePoint = windowPoint + movePoint;         //��������Ҫ�ƶ�������
		parentWidget()->move(windowMovePoint);
		
		startMovePos = event->globalPos();          //���¿�ʼ�ƶ�������

		event->accept();
	}

	//return QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent * event)
{
	moveAble = false;       //�ƶ�����

	event->accept();
	//return QWidget::mouseReleaseEvent(event);
}

//���˫���¼����ƴ�����󻯡���ԭ
void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	//ֻҪ������󻯻���С����ťʱ����Ч
	if (btnType == MIN_MAX_CLOSE_BUTTON)
	{
		if (ui.buttonMax->isVisible())        //˫��ʱ����󻯰�ť�ɼ������
		{
			onButtonMaxClicked();
		}
		else        //��ԭ��ť�ɼ���ԭ
		{
			onButtonRestoreClicked();
		}
	}

	return QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::paintEvent(QPaintEvent * event)
{
	//QWidget::paintEvent(event);     QWidget�Ļ�ͼ�¼��ǿպ���

	if (width() != parentWidget()->width())
	{
		setFixedWidth(parentWidget()->width());
	}
}

//���ñ�������ť
void TitleBar::setTitleButton()
{
	switch (btnType)
	{
	case MIN_CLOSE_BUTTON:
	{
		ui.buttonRestore->setVisible(false);     //setVisible�������Ƿ�ɼ�
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

//��Ӧ��ԭ��ť����ź�
void TitleBar::onButtonRestoreClicked()
{
	ui.buttonRestore->setVisible(false);
	ui.buttonMax->setVisible(true);

	emit buttonRestoreClicked();
}

//��Ӧ��󻯰�ť����ź�
void TitleBar::onButtonMaxClicked()
{
	ui.buttonMax->setVisible(false);
	ui.buttonRestore->setVisible(true);

	emit buttonMaxClicked();
}
