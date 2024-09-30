#include "CCMainWindow.h"
#include <qpainter.h>
#include <QHBoxLayout>
#include <qtimer.h>
#include "UseMySQL.h"
#include "CommonUtils.h"
#include "NotifyManager.h"
#include "RootContactItem.h"
#include <QTreeWidgetItem>
#include "ContactItem.h"
#include <qdebug.h>
#include "CustomProxyStyle.h"
#include "SysTrayIcon.h"
#include "WindowManager.h"

QString LoginUserAccount;     //登录用户账号

User* CCMainWindow::user = nullptr;

CCMainWindow::CCMainWindow(const QString& account, QWidget *parent)
	: BasicWindow(parent), account(account)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);      //不显示任务栏图标，窗口始终显示在最前面
	init();
	LoginUserAccount = account;
}

CCMainWindow::~CCMainWindow()
{
}

void CCMainWindow::init()
{
	loadStyleSheet("CCMainWindow");
	initButton();
	ui.searchLineEdit->installEventFilter(this);
	setUserName();
	setUserHeadPixmap();
	initConnect();
	initContact();
	initSysTray();
}

void CCMainWindow::initButton()
{
	//状态按钮
	drawButtonIcon(ui.statusBtn, ":/Resources/MainWindow/StatusSucceeded.png");
	//等级按钮
	initLevelButton();
	//appWidget以及buttonLayout按钮
	initAppButton();
}

void CCMainWindow::initConnect()
{
	/*
		层叠样式表优先级：
		1.内联样式（直接在部件上设置的样式）
		2.ID选择器（如 #searchWidget）
		3.类选择器（如 QWidget）
		4.属性选择器（如[titleskin]）
		使用组合选择器时，其优先级将比单个选择器更高（更具体）
	*/
	connect(NotifyManager::instance(), &NotifyManager::skinChanged, [this]() { updateSearchStyle(false); });
	connect(ui.sysmin, &QPushButton::clicked, this, &CCMainWindow::onMin);
	connect(ui.sysclose, &QPushButton::clicked, this, &CCMainWindow::onQuit);
}

void CCMainWindow::initLevelButton()
{
	drawLevelButton(0);               //初始化等级
	QTimer* timer = new QTimer(this);
	timer->setInterval(2000);         //间隔2s等级+1
	connect(timer, &QTimer::timeout, [this]()
	{
		static int level = 0;
		if (level == 99) level = 0;   //等级上限99级
		drawLevelButton(++level);
	});

	timer->start();                  //启动定时器
}

void CCMainWindow::initContact()
{
	//改变样式
	ui.treeWidget->setStyle(new CustomProxyStyle());

	//根项
	QTreeWidgetItem* rootItem = new QTreeWidgetItem;
	/*
		使用Qt::UserRole存储特定数据，根项数据设为0
		根据该值判断是根项或者子项
	*/
	rootItem->setData(0, Qt::UserRole, 0);

	RootContactItem* rootWidget = new RootContactItem(true, ui.treeWidget);
	rootWidget->setText(QString::fromLocal8Bit("奇牛科技"));

	ui.treeWidget->addTopLevelItem(rootItem);
	ui.treeWidget->setItemWidget(rootItem, 0, rootWidget);

	//子项
	addUserGroup(rootItem);

	//连接信号与槽
	connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &CCMainWindow::onTreeItemClicked);               //项被点击
	connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &CCMainWindow::onTreeItemDoubleClicked);   //项被双击
	connect(ui.treeWidget, &QTreeWidget::itemExpanded, this, &CCMainWindow::onTreeItemExpanded);             //项被展开
	connect(ui.treeWidget, &QTreeWidget::itemCollapsed, this, &CCMainWindow::onTreeItemCollapsed);           //项被折叠
}

void CCMainWindow::initSysTray()
{
	SysTrayIcon* icon = new SysTrayIcon(this);
	icon->show();     //显示系统托盘图标
}

/*
*  添加用户群聊
*  @param rootItem  根项
*/
void CCMainWindow::addUserGroup(QTreeWidgetItem* rootItem)
{
	//获取群号    公司群+部门群
	QList<int> group = UseMySQL::instance()->getUserGroup(account);

	for (int id : group)
	{
		QTreeWidgetItem* childItem = new QTreeWidgetItem;
		childItem->setData(0, Qt::UserRole, 1);          //子项UserRole角色数据为1
		childItem->setData(0, Qt::UserRole + 1, id);     //子项保存群号

		ContactItem* childWidget = new ContactItem(ui.treeWidget);
		
		//设置群组头像
		QString headPath = UseMySQL::instance()->getGroupHeadPixmap(id);
		QPixmap head(headPath);
		QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
		childWidget->setHeadPixmap(CommonUtils::getRoundedPixmap(head, head_mask, childWidget->getHeadLabelSize()));

		//设置群组名称
		QString groupName = UseMySQL::instance()->getGroupName(id);
		childWidget->setUserName(groupName);

		//将子项添加到根项
		rootItem->addChild(childItem);
		ui.treeWidget->setItemWidget(childItem, 0, childWidget);
	}
}


void CCMainWindow::initAppButton()
{
	//appWidget
	QHBoxLayout* appLayout = new QHBoxLayout(ui.appWidget);
	appLayout->setContentsMargins(0, 0, 0, 0);       //设置边距
	appLayout->setSpacing(2);                        //设置部件间间隔
	QStringList appList = { "app_7", "app_2", "app_3", "app_4", "app_5", "app_6", "app_skin" };
	for (auto app : appList)
	{
		appLayout->addWidget(addOtherButton(APP_ICON(app), app));
	}
	appLayout->addStretch();        //在末尾添加一个垫片

	//bottomLayout
	QStringList bottomList = { "app_10", "app_8", "app_11", "app_9" };
	for (auto app : bottomList)
	{
		ui.bottomLayout_up->addWidget(addOtherButton(APP_ICON(app), app));
	}
	ui.bottomLayout_up->addStretch();
}

/*
*  绘制按钮图标
*  @param btn       按钮
*  @param icon      图标路径
*/
void CCMainWindow::drawButtonIcon(QPushButton*& btn, const QString& icon)
{
	//通过QPixmap可以构造QIcon
	QPixmap pixmap(btn->size());       //创建画布
	pixmap.fill(Qt::transparent);

	QPixmap iconPixmap(icon);          //原片

	QPainter p(&pixmap);
	p.drawPixmap((pixmap.width() - iconPixmap.width()) / 2, (pixmap.height() - iconPixmap.height()) / 2, iconPixmap);

	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
}

/*
*  添加其他按钮
*  @param icon      图标路径
*  @param objName   部件对象名
*  @param size      按钮大小
*  @return          返回按钮
*/
QPushButton* CCMainWindow::addOtherButton(const QString& icon, const QString& objName, QSize size)
{
	QPushButton* btn = new QPushButton(this);     //新的按钮
	btn->setFixedSize(size);
	btn->setObjectName(objName);                  //设置部件对象名
	btn->setProperty("hasborder", true);          //添加动态属性并初始化为true

	drawButtonIcon(btn, icon);

	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onButtonClicked);

	return btn;
}

/*
*  绘制等级按钮
*  @param level     当前等级
*/
void CCMainWindow::drawLevelButton(int level)
{
	QPixmap pixmap(ui.levelBtn->size());       //创建画布
	pixmap.fill(Qt::transparent);

	QPainter p(&pixmap);
	p.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));    //在(0, 4)位置绘制Lv

	int tenNum = level / 10;      //十位
	int unitNum = level % 10;     //个位

	QPixmap levelValue(":/Resources/MainWindow/levelvalue.png");

	//绘制十位
	p.drawPixmap(10, 4, levelValue, tenNum * 6, 0, 6, 7);     //在(10, 4)位置绘制十位，从原图的(tenNum * 6, 0)位置开始，绘制宽6，高7
	//绘制个位
	p.drawPixmap(16, 4, levelValue, unitNum * 6, 0, 6, 7);    //在(16, 4)位置绘制个位，从原图的(unitNum * 6, 0)位置开始，绘制宽6，高7

	ui.levelBtn->setIcon(pixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::updateSearchStyle(bool hasFocus)
{
	/*
		setStyleSheet设置样式表时，只会应用最后一次调用设置的样式
	*/

	if (hasFocus)       //有焦点
	{
		ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{  \
												background-color: rgb(255, 255, 255);     \
												border-bottom: 1px solid rgba(%1, %2, %3, 100); } \
												QPushButton#searchBtn{ border-image: url(:/Resources/MainWindow/search/main_search_deldown.png); }  \
												QPushButton#searchBtn:hover{ border-image: url(:/Resources/MainWindow/search/main_search_delhighlight.png); } \
												QPushButton#searchBtn:pressed{ border-image: url(:/Resources/MainWindow/search/main_search_delhighdown.png); }"
		).arg(skinColor.red()).arg(skinColor.green()).arg(skinColor.blue()));
	}
	else               //没有焦点
	{
		ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{ \
												background-color: rgba(%1, %2, %3, 50);  \
												border-bottom: 1px solid rgba(%1, %2, %3, 30); }  \
												QPushButton#searchBtn{ border-image: url(:/Resources/MainWindow/search/search_icon.png); }"
		).arg(skinColor.red()).arg(skinColor.green()).arg(skinColor.blue()));
	}
}

void CCMainWindow::setUserName()
{
	QString name = UseMySQL::instance()->getUserName(account);
	//QString name = "zzzzzzzzzzzzzzzzzzzzzzzzzz";

	//Qt::ElideRight指示省略号出现在文本的末尾
	name = ui.nameLabel->fontMetrics().elidedText(name, Qt::ElideRight, ui.nameLabel->width());

	//font：简写形式 bold：字体加粗 17px：字体大小
	ui.nameLabel->setStyleSheet("color: rgb(255, 255, 255); font: bold 17px");
	ui.nameLabel->setText(name);
}

void CCMainWindow::setUserHeadPixmap()
{
	QString headPath = UseMySQL::instance()->getUserHeadPixmap(account);

	QPixmap head(headPath);        //原头像
	QPixmap mask(":/Resources/MainWindow/head_mask.png");    //底片
	ui.headLabel->setPixmap(CommonUtils::getRoundedPixmap(head, mask, ui.headLabel->size()));
}

bool CCMainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui.searchLineEdit)
	{
		if (event->type() == QEvent::FocusIn)            //获取焦点
		{
			updateSearchStyle(true);
			return false;
		}
		else if (event->type() == QEvent::FocusOut)      //失去焦点
		{
			updateSearchStyle(false);
			return false;
		}
	}

	return BasicWindow::eventFilter(watched, event);     //未处理的事件继续传递
}

void CCMainWindow::onButtonClicked()
{
	if (sender()->objectName() == "app_skin")
	{
		SkinWindow* skinWindow = new SkinWindow;
		skinWindow->show();
	}
}

void CCMainWindow::onTreeItemClicked(QTreeWidgetItem* item)
{
	/*
		根项UserRole角色数据为0
		点击的如果是根项，则更新根项状态
			展开状态：折叠子项
			折叠状态：展开子项
	*/
	bool isRoot = !(item->data(0, Qt::UserRole).toBool());
	if (isRoot)
	{
		item->setExpanded(!item->isExpanded());
	}
}

void CCMainWindow::onTreeItemDoubleClicked(QTreeWidgetItem* item)
{
	/*
		子项UserRole角色数据为1
		双击的如果是子项，则打开对应的聊天窗口
	*/
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild)
	{
		WindowManager::instance()->addNewTalkWindow(item->data(0, Qt::UserRole + 1).toInt());
	}
}

void CCMainWindow::onTreeItemExpanded(QTreeWidgetItem* item)
{
	/*
		更新根项箭头状态
		当设置根项展开时（项被点击槽内），会调用此函数
	*/
	bool isRoot = !(item->data(0, Qt::UserRole).toBool());
	if (isRoot)
	{
		RootContactItem* rootWidget = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (rootWidget)
		{
			rootWidget->setExpanded(true);     
		}
	}
}

void CCMainWindow::onTreeItemCollapsed(QTreeWidgetItem* item)
{
	/*
		更新根项箭头状态
		当设置根项折叠时（项被点击槽内），会调用此函数
	*/
	bool isRoot = !(item->data(0, Qt::UserRole).toBool());
	if (isRoot)
	{
		RootContactItem* rootWidget = dynamic_cast<RootContactItem*>(ui.treeWidget->itemWidget(item, 0));
		if (rootWidget)
		{
			rootWidget->setExpanded(false);     
		}
	}
}

void CCMainWindow::onQuit()
{
	TalkWindowShell* shell = WindowManager::instance()->getTalkWindowShell();
	if (shell)
	{
		shell->close();
	}
	QApplication::quit();
}
