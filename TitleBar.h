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
	//鼠标按压、移动、释放事件控制窗体移动
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	void mouseDoubleClickEvent(QMouseEvent* event) override;

	//绘图事件
	void paintEvent(QPaintEvent* event) override;

signals:
	void buttonMinClicked();       //最小化按钮被点击
	void buttonRestoreClicked();   //还原按钮被点击
	void buttonMaxClicked();       //最大化按钮被点击
	void buttonCloseClicked();     //关闭按钮被点击

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

	bool moveAble;         //窗体是否可以移动
	QPoint startMovePos;   //开始移动的点坐标

	QPoint restorePos;     //还原位置
	QSize restoreSize;    //还原大小
};
