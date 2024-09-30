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

	//�ı�Ƥ����ɫ
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
	titleBar->move(0, 0);         //�ƶ����������꣨0��0��

	connect(titleBar, &TitleBar::buttonMinClicked, this, &BasicWindow::onMin);
	connect(titleBar, &TitleBar::buttonMaxClicked, this, &BasicWindow::onMax);
	connect(titleBar, &TitleBar::buttonRestoreClicked, this, &BasicWindow::onRestore);
	connect(titleBar, &TitleBar::buttonCloseClicked, this, &BasicWindow::onClose);
}

//������ʽ��
void BasicWindow::loadStyleSheet(const QString& fileName)
{
	styleFileName = fileName;        //������ʽ���ļ���

	int r = skinColor.red();
	int g = skinColor.green();
	int b = skinColor.blue();

	//background-color ����ɫ
	QString qss = QString("QWidget[titleskin=true] \
								  { background-color: rgb(%1, %2, %3); }    \
						   QWidget[bottomskin=true] \
 								  { background-color: rgba(%1, %2, %3, 50); }"
	).arg(r).arg(g).arg(b);

	CommonUtils::loadStyleSheet(this, styleFileName, qss);
}

void BasicWindow::paintEvent(QPaintEvent* event)
{
	QDialog::paintEvent(event);  //����Ĭ�ϻ�����Ϊ��QDialog��ͼ�¼�����Ϊ�պ���

	//�����������ܶ�Ĭ�ϻ�����Ϊ����ЩĬ�ϻ�����Ϊ��ʱ������ʽ������ò�����ͻ��������ʽ���޷���ȷӦ�ã�����Ҫ��д��ͼ�¼�
	
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

void BasicWindow::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)    //��������¼��İ�ť��������
	{
		if (!titleBar)          //û�б����������޷����
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
		���ߴ�������ͨ���ڵ�����
		1.���ߴ���ͨ��Ϊ��ģ̬���ڣ����������û����������ڵĲ���    ��ͨ����ͨ��Ϊģ̬���ģ̬����
		2.���ߴ���û��������ͼ�꣬��ͨ����˵��۽�С
		3.���ߴ��ڲ��������һ�����ڣ������һ����ͨ���ڹرպ�Ӧ�ó����ֱ���˳�
	*/

	//qDebug() << (windowFlags()/* & Qt::Tool*/);
	/*
		Qt::Tool = 11       1011
		Qt::Window = 1      0001
		1011 & 0001 = 0001
	*/

	if (Qt::Tool == (windowFlags() & Qt::Tool))      //����ô����ǹ��ߴ��ڣ�������
	{
		hide();
	}
	else       //����ô��ڲ��ǹ��ߴ��ڣ�����С��
	{
		showMinimized();
	}
}

void BasicWindow::onRestore()
{
	QPoint pos;
	QSize size;
	titleBar->getRestoreInfo(pos, size);

	setGeometry(QRect(pos, size));       //���ļ���ͼ�Σ�λ�ü���С��
}

void BasicWindow::onMax()
{
	titleBar->saveRestoreInfo(this->pos(), this->size());

	//screens()����ϵͳ��������Ļ���б�
	QRect desktopRect = QApplication::screens()[0]->availableGeometry();        //��ȡ������õľ�������

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
	//��ʾ������Ϊ�����
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
