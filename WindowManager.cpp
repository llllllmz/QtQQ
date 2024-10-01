#include "WindowManager.h"
#include "TalkWindowItem.h"
#include "TalkWindow.h"
#include "UseMySQL.h"

Q_GLOBAL_STATIC(WindowManager, theInstance)

WindowManager::WindowManager(QObject *parent)
	: QObject(parent), talkWindowShell(nullptr), createID(0)
{
}

WindowManager::~WindowManager()
{
}

WindowManager* WindowManager::instance()
{
	return theInstance();
}

void WindowManager::addNewTalkWindow(const int id)//todo 单聊还是私聊，加一个函数参数
{
	if (!talkWindowShell)       //talkWindowShell == nullptr
	{
		talkWindowShell = new TalkWindowShell;
		//destroyed在对象被销毁之前立即发出
		connect(talkWindowShell, &TalkWindowShell::destroyed, [this]() { talkWindowShell = nullptr; });
	}

	//判断窗口是否已经存在
	QWidget* widget = findWindowMap(id);
	if (!widget)         
	{
		//初始化创建ID并创建对应窗口部件
		createID = id;        
		TalkWindow* talkWindow = new TalkWindow(id, talkWindowShell);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkWindow);
		createID = 0;

		//获取聊天窗口相关信息
		QString itemMsg;
		QString windowName;
		QString headPath;
		//if (QString::number(id).length() <= GROUP_ID_LENGTH)     //群聊
		//{
		//	itemMsg = UseMySQL::instance()->getGroupName(id);
		//	windowName = itemMsg;               
		//	headPath = UseMySQL::instance()->getGroupHeadPixmap(id);     //头像
		//}
		//else              //单聊
		{
			QString account = QString::number(id);
			itemMsg = UseMySQL::instance()->getUserName(account);
			windowName = UseMySQL::instance()->getUserDepartment(account) + itemMsg;
			headPath = UseMySQL::instance()->getUserHeadPixmap(account);       //头像
		}

		//设置部件
		talkWindow->setWindowName(windowName);
		talkWindowItem->setMsgText(itemMsg);
		talkWindowItem->setHeadPixmap(headPath);

		//聊天窗口添加到窗口框架
		talkWindowShell->addTalkWindow(talkWindow, talkWindowItem);
	}
	else       //窗口已经存在
	{
		talkWindowShell->updateCurrentWindow(widget);
	}

	//显示聊天窗口
	talkWindowShell->onShow();
}

QWidget* WindowManager::findWindowMap(const int& id)
{
	/*
	* 如果存在，则返回窗口，否则返回nullptr
	*/
	return map.value(id, nullptr);
}

void WindowManager::addWindowMap(const int& id, QWidget* widget)
{
	/*
	* 如果不存在，则添加窗口
	*/
	if (!map.contains(id))
	{
		map.insert(id, widget);
	}
}

void WindowManager::removeWindowMap(const int& id)
{
	map.remove(id);
}

TalkWindowShell* WindowManager::getTalkWindowShell() const
{
	return talkWindowShell;
}

int WindowManager::getCreateID() const
{
	return createID;
}
