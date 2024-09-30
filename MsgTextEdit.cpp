#include "MsgTextEdit.h"
#include "define.h"

MsgTextEdit::MsgTextEdit(QWidget *parent)
	: QTextEdit(parent)
{}

MsgTextEdit::~MsgTextEdit()
{}

void MsgTextEdit::addEmotionHtml(int emotionName)
{
	const QString emotionPath = IMG_QRC + QString::number(emotionName) + IMG_PNG;
	insertHtml(QString(IMG_SRC).arg(emotionPath));      //在光标位置插入表情图片
}
