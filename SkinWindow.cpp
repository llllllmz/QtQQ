#include "SkinWindow.h"
#include "NotifyManager.h"

SkinWindow::SkinWindow(QWidget *parent)
	: BasicWindow(parent)
{
	ui.setupUi(this);

	//setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_DeleteOnClose);      //关闭时释放窗口
	loadStyleSheet("SkinWindow");
	initControl();
}

SkinWindow::~SkinWindow()
{
}

void SkinWindow::initControl()
{
	//颜色
	QList<QColor> colorList =
	{
		QColor(22, 154, 128), QColor(40, 138, 221), QColor(49, 166, 107), QColor(218, 67, 68),
		QColor(177, 99, 158), QColor(107, 81, 92), QColor(89, 92, 160), QColor(21, 156, 199),
		QColor(79, 169, 172), QColor(155, 183, 154), QColor(128, 77, 77), QColor(240, 188, 189)
	};

	for (int row = 0; row < 3; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			ui.gridLayout->addWidget(initLabel(colorList[row * 4 + column]), row, column);
		}
	}

	//响应按钮
	connect(ui.sysmin, &QPushButton::clicked, this, &SkinWindow::onMin);
	connect(ui.sysclose, &QPushButton::clicked, this, &SkinWindow::onClose);
}

QClickLabel* SkinWindow::initLabel(const QColor& color)
{
	QClickLabel* label = new QClickLabel(this);
	label->setCursor(Qt::PointingHandCursor);      //鼠标手型
	label->setFixedSize(84, 84);
	label->setAutoFillBackground(true);            //自动填充背景
	label->setPalette(QPalette(color));

	connect(label, &QClickLabel::clicked, [color]()      //不能用引用捕捉
	{
		/*int red = color.red();
		int green = color.green();
		int blue = color.blue();*/

		NotifyManager::instance()->notifySkinChanged(color);
	});

	return label;
}