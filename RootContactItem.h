#pragma once

#include <QLabel>
#include <QPropertyAnimation>

class RootContactItem  : public QLabel
{
	Q_OBJECT

	Q_PROPERTY(int rotation READ getRotation WRITE setRotation)      //¼ýÍ·½Ç¶È

public:
	RootContactItem(bool hasArrow = true, QWidget *parent = nullptr);
	~RootContactItem();

	void init();

	void setText(const QString& text);
	void setExpanded(bool expand);

private:
	int getRotation() const;
	void setRotation(int r);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	bool hasArrow;
	int rotation;
	QString titleText;

	QPropertyAnimation* an;
};
