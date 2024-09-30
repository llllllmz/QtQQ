#pragma once

#include <QDialog>
#include "TitleBar.h"

class BasicWindow : public QDialog
{
	Q_OBJECT

protected:
	BasicWindow(QWidget *parent);
	virtual ~BasicWindow();

protected:
	void initTitleBar(ButtonType buttonType = MIN_CLOSE_BUTTON, const QString& filePath = QString(), const QString& titleContent = QString());  //��ʼ��������
	void loadStyleSheet(const QString& fileName);
	
protected:
	void paintEvent(QPaintEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
	void onMin();
	void onRestore();
	void onMax();
	void onClose();

	void onHide();
	void onShow();
	//void onQuit();

	void onSkinChanged(const QColor& color);

protected:
	TitleBar* titleBar;      //������

	bool moveAble;           //�Ƿ���ƶ�
	QPoint startMovePos;     //��ʼ�ƶ��ĵ�����

	QColor skinColor;        //Ƥ����ɫ
	QString styleFileName;   //��ʽ���ļ���
};
