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

QString LoginUserAccount;     //��¼�û��˺�

User* CCMainWindow::user = nullptr;

CCMainWindow::CCMainWindow(const QString& account, QWidget *parent)
	: BasicWindow(parent), account(account)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() | Qt::Tool | Qt::WindowStaysOnTopHint);      //����ʾ������ͼ�꣬����ʼ����ʾ����ǰ��
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
	//״̬��ť
	drawButtonIcon(ui.statusBtn, ":/Resources/MainWindow/StatusSucceeded.png");
	//�ȼ���ť
	initLevelButton();
	//appWidget�Լ�buttonLayout��ť
	initAppButton();
}

void CCMainWindow::initConnect()
{
	/*
		�����ʽ�����ȼ���
		1.������ʽ��ֱ���ڲ��������õ���ʽ��
		2.IDѡ�������� #searchWidget��
		3.��ѡ�������� QWidget��
		4.����ѡ��������[titleskin]��
		ʹ�����ѡ����ʱ�������ȼ����ȵ���ѡ�������ߣ������壩
	*/
	connect(NotifyManager::instance(), &NotifyManager::skinChanged, [this]() { updateSearchStyle(false); });
	connect(ui.sysmin, &QPushButton::clicked, this, &CCMainWindow::onMin);
	connect(ui.sysclose, &QPushButton::clicked, this, &CCMainWindow::onQuit);
}

void CCMainWindow::initLevelButton()
{
	drawLevelButton(0);               //��ʼ���ȼ�
	QTimer* timer = new QTimer(this);
	timer->setInterval(2000);         //���2s�ȼ�+1
	connect(timer, &QTimer::timeout, [this]()
	{
		static int level = 0;
		if (level == 99) level = 0;   //�ȼ�����99��
		drawLevelButton(++level);
	});

	timer->start();                  //������ʱ��
}

void CCMainWindow::initContact()
{
	//�ı���ʽ
	ui.treeWidget->setStyle(new CustomProxyStyle());

	//����
	QTreeWidgetItem* rootItem = new QTreeWidgetItem;
	/*
		ʹ��Qt::UserRole�洢�ض����ݣ�����������Ϊ0
		���ݸ�ֵ�ж��Ǹ����������
	*/
	rootItem->setData(0, Qt::UserRole, 0);

	RootContactItem* rootWidget = new RootContactItem(true, ui.treeWidget);
	rootWidget->setText(QString::fromLocal8Bit("��ţ�Ƽ�"));

	ui.treeWidget->addTopLevelItem(rootItem);
	ui.treeWidget->setItemWidget(rootItem, 0, rootWidget);

	//����
	addUserGroup(rootItem);

	//�����ź����
	connect(ui.treeWidget, &QTreeWidget::itemClicked, this, &CCMainWindow::onTreeItemClicked);               //����
	connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &CCMainWindow::onTreeItemDoubleClicked);   //�˫��
	connect(ui.treeWidget, &QTreeWidget::itemExpanded, this, &CCMainWindow::onTreeItemExpanded);             //�չ��
	connect(ui.treeWidget, &QTreeWidget::itemCollapsed, this, &CCMainWindow::onTreeItemCollapsed);           //��۵�
}

void CCMainWindow::initSysTray()
{
	SysTrayIcon* icon = new SysTrayIcon(this);
	icon->show();     //��ʾϵͳ����ͼ��
}

/*
*  ����û�Ⱥ��
*  @param rootItem  ����
*/
void CCMainWindow::addUserGroup(QTreeWidgetItem* rootItem)
{
	//��ȡȺ��    ��˾Ⱥ+����Ⱥ
	QList<int> group = UseMySQL::instance()->getUserGroup(account);

	for (int id : group)
	{
		QTreeWidgetItem* childItem = new QTreeWidgetItem;
		childItem->setData(0, Qt::UserRole, 1);          //����UserRole��ɫ����Ϊ1
		childItem->setData(0, Qt::UserRole + 1, id);     //�����Ⱥ��

		ContactItem* childWidget = new ContactItem(ui.treeWidget);
		
		//����Ⱥ��ͷ��
		QString headPath = UseMySQL::instance()->getGroupHeadPixmap(id);
		QPixmap head(headPath);
		QPixmap head_mask(":/Resources/MainWindow/head_mask.png");
		childWidget->setHeadPixmap(CommonUtils::getRoundedPixmap(head, head_mask, childWidget->getHeadLabelSize()));

		//����Ⱥ������
		QString groupName = UseMySQL::instance()->getGroupName(id);
		childWidget->setUserName(groupName);

		//��������ӵ�����
		rootItem->addChild(childItem);
		ui.treeWidget->setItemWidget(childItem, 0, childWidget);
	}
}


void CCMainWindow::initAppButton()
{
	//appWidget
	QHBoxLayout* appLayout = new QHBoxLayout(ui.appWidget);
	appLayout->setContentsMargins(0, 0, 0, 0);       //���ñ߾�
	appLayout->setSpacing(2);                        //���ò�������
	QStringList appList = { "app_7", "app_2", "app_3", "app_4", "app_5", "app_6", "app_skin" };
	for (auto app : appList)
	{
		appLayout->addWidget(addOtherButton(APP_ICON(app), app));
	}
	appLayout->addStretch();        //��ĩβ���һ����Ƭ

	//bottomLayout
	QStringList bottomList = { "app_10", "app_8", "app_11", "app_9" };
	for (auto app : bottomList)
	{
		ui.bottomLayout_up->addWidget(addOtherButton(APP_ICON(app), app));
	}
	ui.bottomLayout_up->addStretch();
}

/*
*  ���ư�ťͼ��
*  @param btn       ��ť
*  @param icon      ͼ��·��
*/
void CCMainWindow::drawButtonIcon(QPushButton*& btn, const QString& icon)
{
	//ͨ��QPixmap���Թ���QIcon
	QPixmap pixmap(btn->size());       //��������
	pixmap.fill(Qt::transparent);

	QPixmap iconPixmap(icon);          //ԭƬ

	QPainter p(&pixmap);
	p.drawPixmap((pixmap.width() - iconPixmap.width()) / 2, (pixmap.height() - iconPixmap.height()) / 2, iconPixmap);

	btn->setIcon(pixmap);
	btn->setIconSize(btn->size());
}

/*
*  ���������ť
*  @param icon      ͼ��·��
*  @param objName   ����������
*  @param size      ��ť��С
*  @return          ���ذ�ť
*/
QPushButton* CCMainWindow::addOtherButton(const QString& icon, const QString& objName, QSize size)
{
	QPushButton* btn = new QPushButton(this);     //�µİ�ť
	btn->setFixedSize(size);
	btn->setObjectName(objName);                  //���ò���������
	btn->setProperty("hasborder", true);          //��Ӷ�̬���Բ���ʼ��Ϊtrue

	drawButtonIcon(btn, icon);

	connect(btn, &QPushButton::clicked, this, &CCMainWindow::onButtonClicked);

	return btn;
}

/*
*  ���Ƶȼ���ť
*  @param level     ��ǰ�ȼ�
*/
void CCMainWindow::drawLevelButton(int level)
{
	QPixmap pixmap(ui.levelBtn->size());       //��������
	pixmap.fill(Qt::transparent);

	QPainter p(&pixmap);
	p.drawPixmap(0, 4, QPixmap(":/Resources/MainWindow/lv.png"));    //��(0, 4)λ�û���Lv

	int tenNum = level / 10;      //ʮλ
	int unitNum = level % 10;     //��λ

	QPixmap levelValue(":/Resources/MainWindow/levelvalue.png");

	//����ʮλ
	p.drawPixmap(10, 4, levelValue, tenNum * 6, 0, 6, 7);     //��(10, 4)λ�û���ʮλ����ԭͼ��(tenNum * 6, 0)λ�ÿ�ʼ�����ƿ�6����7
	//���Ƹ�λ
	p.drawPixmap(16, 4, levelValue, unitNum * 6, 0, 6, 7);    //��(16, 4)λ�û��Ƹ�λ����ԭͼ��(unitNum * 6, 0)λ�ÿ�ʼ�����ƿ�6����7

	ui.levelBtn->setIcon(pixmap);
	ui.levelBtn->setIconSize(ui.levelBtn->size());
}

void CCMainWindow::updateSearchStyle(bool hasFocus)
{
	/*
		setStyleSheet������ʽ��ʱ��ֻ��Ӧ�����һ�ε������õ���ʽ
	*/

	if (hasFocus)       //�н���
	{
		ui.searchWidget->setStyleSheet(QString("QWidget#searchWidget{  \
												background-color: rgb(255, 255, 255);     \
												border-bottom: 1px solid rgba(%1, %2, %3, 100); } \
												QPushButton#searchBtn{ border-image: url(:/Resources/MainWindow/search/main_search_deldown.png); }  \
												QPushButton#searchBtn:hover{ border-image: url(:/Resources/MainWindow/search/main_search_delhighlight.png); } \
												QPushButton#searchBtn:pressed{ border-image: url(:/Resources/MainWindow/search/main_search_delhighdown.png); }"
		).arg(skinColor.red()).arg(skinColor.green()).arg(skinColor.blue()));
	}
	else               //û�н���
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

	//Qt::ElideRightָʾʡ�Ժų������ı���ĩβ
	name = ui.nameLabel->fontMetrics().elidedText(name, Qt::ElideRight, ui.nameLabel->width());

	//font����д��ʽ bold������Ӵ� 17px�������С
	ui.nameLabel->setStyleSheet("color: rgb(255, 255, 255); font: bold 17px");
	ui.nameLabel->setText(name);
}

void CCMainWindow::setUserHeadPixmap()
{
	QString headPath = UseMySQL::instance()->getUserHeadPixmap(account);

	QPixmap head(headPath);        //ԭͷ��
	QPixmap mask(":/Resources/MainWindow/head_mask.png");    //��Ƭ
	ui.headLabel->setPixmap(CommonUtils::getRoundedPixmap(head, mask, ui.headLabel->size()));
}

bool CCMainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == ui.searchLineEdit)
	{
		if (event->type() == QEvent::FocusIn)            //��ȡ����
		{
			updateSearchStyle(true);
			return false;
		}
		else if (event->type() == QEvent::FocusOut)      //ʧȥ����
		{
			updateSearchStyle(false);
			return false;
		}
	}

	return BasicWindow::eventFilter(watched, event);     //δ������¼���������
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
		����UserRole��ɫ����Ϊ0
		���������Ǹ������¸���״̬
			չ��״̬���۵�����
			�۵�״̬��չ������
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
		����UserRole��ɫ����Ϊ1
		˫��������������򿪶�Ӧ�����촰��
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
		���¸����ͷ״̬
		�����ø���չ��ʱ���������ڣ�������ô˺���
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
		���¸����ͷ״̬
		�����ø����۵�ʱ���������ڣ�������ô˺���
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
