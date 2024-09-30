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
	arg1����Ҫ������ʽ��Ĳ���
	arg2����ʽ���ļ���
	arg3����ѡ�Ķ�����ʽ
*/
void CommonUtils::loadStyleSheet(QWidget* widget, const QString& fileName, const QString& otherStyle)
{
	if (!widget) return;
	widget->setStyleSheet(QString());        //���������ǰ��ʽ

	const QString filePath = QRC_QSS(fileName);
	QFile file(filePath);
	
	if (file.open(QFile::ReadOnly))
	{
		QString qss = file.readAll();     //��ȡ�ļ�����ʽ
		qss += otherStyle;                //���������������ʽ

		widget->setStyleSheet(qss);
		file.close();
	}
}

/*
	arg1��ԭͷ��
	arg2����Ƭ
	arg3�����Ŵ�С
*/
QPixmap CommonUtils::getRoundedPixmap(QPixmap src, QPixmap mask, QSize maskSize)
{
	if (maskSize.isEmpty())      //�����Ȼ�߶��κ�һ��С�ڻ����0������true
	{
		maskSize = mask.size();
	}
	else
	{
		//Qt::KeepAspectRatio ����ʱ�����ݺ�ȣ���ȡ�����ܴ�ľ���
		//Qt::SmoothTransformation ���ͼ��������ʹͼ����ʾ����ƽ�������پ��״��Ե�ĳ���
		mask = mask.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	}

	//����ԭͼƬ
	src = src.scaled(maskSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	/*
		QPixmap����Ҫ��Ч��ʾ
		QImage����Ҫ��������
		QIcon����Ҫ������ʾͼ��
	*/

	//QImage::Format_ARGB32_Premultiplied ��ʾͼ��ĸ�ʽΪ 32 λ�� ARGB
	//���ָ�ʽ��ͼ��ͨ��������Ҫ͸�����͸��Ч����ͼ����
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
