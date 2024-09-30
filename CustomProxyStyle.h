#pragma once
#include <qproxystyle.h>

/*
	QProxyStyle��Qt�����ڴ�����ʽ���Ƶ���
	�������Զ�����޸��ض���uiԪ�صĻ�����Ϊ
*/
class CustomProxyStyle : public QProxyStyle
{
public:
	/*
		drawPrimitive������Ƹ��ֻ�����uiԪ��
		element��������ƵĻ���Ԫ�أ�����Ļ���Ԫ���ǽ�����Σ�
		option���ṩ���Ƹ�Ԫ������ĸ��ֲ����������ߴ硢״̬����Ϣ
		painter�����ڻ��Ƶ� QPainter ����
		widget����ǰ���ڻ��Ʋ����Ĳ���
	*/
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = Q_NULLPTR) const override
	{
		if (element == PE_FrameFocusRect)    //����Ԫ���ǽ������
		{
			return;         //ֱ�ӷ��ز������κλ��Ʋ���
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);       //Ĭ����ʽ����
		}
	}
};