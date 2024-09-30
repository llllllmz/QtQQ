#pragma once

#include <QWidget>
#include "ui_TitleBar.h"
#include "define.h"

class TitleBar : public QWidget
{
	Q_OBJECT

public:
	TitleBar(ButtonType type, QWidget *parent);
	~TitleBar();

	void setTitleIcon(const QString& filePath);
	void setTitleContent(const QString& titleContent);
	void setTitleWidth(int width);

	void saveRestoreInfo(const QPoint& point, const QSize& size);    
	void getRestoreInfo(QPoint& point, QSize& size);      

	bool windowIsMax();

protected:
	//��갴ѹ���ƶ����ͷ��¼����ƴ����ƶ�
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void mouseDoubleClickEvent(QMouseEvent* event) override;

	//��ͼ�¼�
	void paintEvent(QPaintEvent* event) override;

signals:
	void buttonMinClicked();       //��С����ť�����
	void buttonRestoreClicked();   //��ԭ��ť�����
	void buttonMaxClicked();       //��󻯰�ť�����
	void buttonCloseClicked();     //�رհ�ť�����

private slots:
	void onButtonRestoreClicked();       
	void onButtonMaxClicked();           

private:
	void init();
	void setTitleButton();
	void initConnect();

private:
	Ui::TitleBar ui;

	ButtonType btnType;

	bool moveAble;         //�����Ƿ�����ƶ�
	QPoint startMovePos;   //��ʼ�ƶ��ĵ�����

	QPoint restorePos;     //��ԭλ��
	QSize restoreSize;    //��ԭ��С
};
