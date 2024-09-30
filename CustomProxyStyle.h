#pragma once
#include <qproxystyle.h>

/*
	QProxyStyle是Qt中用于代理样式绘制的类
	允许你自定义或修改特定的ui元素的绘制行为
*/
class CustomProxyStyle : public QProxyStyle
{
public:
	/*
		drawPrimitive负责绘制各种基础的ui元素
		element：代表绘制的基础元素（这里的基础元素是焦点矩形）
		option：提供绘制该元素所需的各种参数，包含尺寸、状态等信息
		painter：用于绘制的 QPainter 对象
		widget：当前正在绘制操作的部件
	*/
	virtual void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget = Q_NULLPTR) const override
	{
		if (element == PE_FrameFocusRect)    //绘制元素是焦点矩形
		{
			return;         //直接返回不进行任何绘制操作
		}
		else
		{
			QProxyStyle::drawPrimitive(element, option, painter, widget);       //默认样式绘制
		}
	}
};