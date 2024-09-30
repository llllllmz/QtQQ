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
	void initTitleBar(ButtonType buttonType = MIN_CLOSE_BUTTON, const QString& filePath = QString(), const QString& titleContent = QString());  //初始化标题栏
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
	TitleBar* titleBar;      //标题栏

	bool moveAble;           //是否可移动
	QPoint startMovePos;     //开始移动的点坐标

	QColor skinColor;        //皮肤颜色
	QString styleFileName;   //样式表文件名
};
