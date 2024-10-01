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

void WindowManager::addNewTalkWindow(const int id)//todo ���Ļ���˽�ģ���һ����������
{
	if (!talkWindowShell)       //talkWindowShell == nullptr
	{
		talkWindowShell = new TalkWindowShell;
		//destroyed�ڶ�������֮ǰ��������
		connect(talkWindowShell, &TalkWindowShell::destroyed, [this]() { talkWindowShell = nullptr; });
	}

	//�жϴ����Ƿ��Ѿ�����
	QWidget* widget = findWindowMap(id);
	if (!widget)         
	{
		//��ʼ������ID��������Ӧ���ڲ���
		createID = id;        
		TalkWindow* talkWindow = new TalkWindow(id, talkWindowShell);
		TalkWindowItem* talkWindowItem = new TalkWindowItem(talkWindow);
		createID = 0;

		//��ȡ���촰�������Ϣ
		QString itemMsg;
		QString windowName;
		QString headPath;
		//if (QString::number(id).length() <= GROUP_ID_LENGTH)     //Ⱥ��
		//{
		//	itemMsg = UseMySQL::instance()->getGroupName(id);
		//	windowName = itemMsg;               
		//	headPath = UseMySQL::instance()->getGroupHeadPixmap(id);     //ͷ��
		//}
		//else              //����
		{
			QString account = QString::number(id);
			itemMsg = UseMySQL::instance()->getUserName(account);
			windowName = UseMySQL::instance()->getUserDepartment(account) + itemMsg;
			headPath = UseMySQL::instance()->getUserHeadPixmap(account);       //ͷ��
		}

		//���ò���
		talkWindow->setWindowName(windowName);
		talkWindowItem->setMsgText(itemMsg);
		talkWindowItem->setHeadPixmap(headPath);

		//���촰����ӵ����ڿ��
		talkWindowShell->addTalkWindow(talkWindow, talkWindowItem);
	}
	else       //�����Ѿ�����
	{
		talkWindowShell->updateCurrentWindow(widget);
	}

	//��ʾ���촰��
	talkWindowShell->onShow();
}

QWidget* WindowManager::findWindowMap(const int& id)
{
	/*
	* ������ڣ��򷵻ش��ڣ����򷵻�nullptr
	*/
	return map.value(id, nullptr);
}

void WindowManager::addWindowMap(const int& id, QWidget* widget)
{
	/*
	* ��������ڣ�����Ӵ���
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
