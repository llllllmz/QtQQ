#pragma once

#include <QWidget>
#include "ui_EmotionWindow.h"
#include "EmotionLabel.h"

class EmotionWindow : public QWidget
{
	Q_OBJECT

public:
	EmotionWindow(QWidget *parent = nullptr);
	~EmotionWindow();

private:
	void initControl();
	EmotionLabel* initLabel(int name);

signals:
	void addEmotion(int name);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	Ui::EmotionWindow ui;
	const int rowCount = 12;       //行数
	const int columnCount = 14;    //列数
};
