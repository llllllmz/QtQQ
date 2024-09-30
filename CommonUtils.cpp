#include "CommonUtils.h"
#include <qfile.h>
#include "define.h"
#include <QPainter>

CommonUtils::CommonUtils()
{
}

CommonUtils::~CommonUtils()
{
}

/*
	arg1：需要设置样式表的部件
	arg2：样式表文件名
	arg3：可选的定制样式
*/
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& fileName, const QString& otherStyle)
{
	if (!widget) return;
	widget->setStyleSheet(QString());        //清除部件当前样式

	const QString filePath = QRC_QSS(fileName);
	QFile file(filePath);
	
	if (file.open(QFile::ReadOnly))
	{
		QString qss = file.readAll();     //读取文件内样式
		qss += otherStyle;                //如果有其它定制样式

		widget->setStyleSheet(qss);
		file.close();
	}
}

/*
	arg1：原头像
	arg2：底片
	arg3：缩放大小
*/
QPixmap CommonUtils::getRoundedPixmap(QPixmap src, QPixmap mask, QSize maskSize)
{
	if (maskSize.isEmpty())      //如果宽度或高度任何一个小于或等于0，返回true
	{
		maskSize = mask.size();
	}
	else
	{
		//Qt::KeepAspectRatio 缩放时保留纵横比，获取尽可能大的矩形
		//Qt::SmoothTransformation 提高图像质量，使图像显示更加平滑，减少锯齿状边缘的出现
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//缩放原图片
	src = src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	/*
		QPixmap：需要高效显示
		QImage：需要分析处理
		QIcon：需要管理显示图标
	*/

	//QImage::Format_ARGB32_Premultiplied 表示图像的格式为 32 位的 ARGB
	//这种格式的图像通常用于需要透明或半透明效果的图像处理
	QImage image(maskSize, QImage::Format_ARGB32_Premultiplied);

	QPainter p(&image);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(image.rect(), Qt::transparent);
	p.setCompositionMode(QPainter::CompositionMode_SourceOver);
	p.drawPixmap(0, 0, mask);
	p.setCompositionMode(QPainter::CompositionMode_SourceIn);
	p.drawPixmap(0, 0, src);

	return QPixmap::fromImage(image);
}
