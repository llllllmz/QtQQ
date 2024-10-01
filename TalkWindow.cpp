#include "TalkWindow.h"
#include "WindowManager.h"
#include <qjsonarray.h>
#include <qtextobject.h>
#include <qjsonobject.h>
#include <qdebug.h>
#include <qtooltip.h>
#include "UseMySQL.h"
#include <qmessagebox.h>
#include "RootContactItem.h"
#include "ContactItem.h"
#include "CommonUtils.h"
#include "WindowManager.h"
#include "SendFile.h"
extern QString LoginUserAccount;       //定义于CCMainWindow.cpp

TalkWindow::TalkWindow(const int& id, QWidget* parent)
	: QWidget(parent), talkId(id)
{
	ui.setupUi(this);
	
	WindowManager::instance()->addWindowMap(talkId, this);
	//initTalkType();
	init();
}

TalkWindow::~TalkWindow()
{
	WindowManager::instance()->removeWindowMap(talkId);
}

void TalkWindow::init()
{
	initStyle();
	initConnect();
	initTalkWindow();
}
//
//void TalkWindow::initTalkType()
//{
//	if (QString::number(talkId).length() <= GROUP_ID_LENGTH)       //群聊
//		isGroupTalk = true;
//	else 
//		isGroupTalk = false;
//}

void TalkWindow::initStyle()
{
	ui.bodySplitter->setSizes(QList<int>{600, 138});
	ui.textEdit->setFocus();                  //设置焦点
}

void TalkWindow::initConnect()
{
	//TalkWindow构造时，parent()为TalkWindowShell
	connect(ui.sysmin, SIGNAL(clicked()), parent(), SLOT(onMin()));
	connect(ui.sysclose, SIGNAL(clicked()), parent(), SLOT(onClose()));
	connect(ui.closeBtn, SIGNAL(clicked()), parent(), SLOT(onClose()));
	
	connect(ui.faceBtn, SIGNAL(clicked()), parent(), SLOT(onEmotionBtnClicked()));
	connect(ui.sendBtn, &QToolButton::clicked, this, &TalkWindow::onSendBtnClicked);

	connect(ui.treeWidget, &QTreeWidget::itemDoubleClicked, this, &TalkWindow::onTreeItemDoubleClicked);
	connect(ui.fileopenBtn, &QPushButton::clicked, this, &TalkWindow::onFileOpenBtnClicked);
}

void TalkWindow::initTalkWindow()
{
	//if (QString::number(talkId).length() <= GROUP_ID_LENGTH)
	//	initGroupTalk();
	//else
		initSingleTalk();
}

void TalkWindow::initGroupTalk()
{
	//根项
	QTreeWidgetItem* rootItem = new QTreeWidgetItem;
	rootItem->setData(0, Qt::UserRole, 0);	

	//获取群成员信息
	QList<QPair<QString, QString>> infoList;
	if (!UseMySQL::instance()->getRegisterObjectInfo(QString::number(talkId),infoList))
	{
		QMessageBox::information(nullptr, "erro", "getRegisterObjectInfo Error.");
		return;
	}
	RootContactItem* rootWidget = new RootContactItem(false, ui.treeWidget);
	rootWidget->setText(QString::fromLocal8Bit("%1 成员-%2").arg(UseMySQL::instance()->getGroupName(talkId)).arg(infoList.count()));

	ui.treeWidget->addTopLevelItem(rootItem);
	ui.treeWidget->setItemWidget(rootItem, 0, rootWidget);

	rootItem->setExpanded(true);

	for (auto& info : infoList)
	{
		addMember(rootItem, info);
	}
}

void TalkWindow::initSingleTalk()
{
	QPixmap pixmap(":Resources/MainWindow/skin.png");
	ui.widget->setFixedSize(pixmap.size());
	QLabel*label = new QLabel(ui.widget);
	label->setPixmap(pixmap);
	label->setFixedSize(ui.widget->size());
}

void TalkWindow::setWindowName(const QString& name)
{
	ui.nameLabel->setStyleSheet("color: rgb(112, 114, 113); font: bold;");     //设置字体颜色
	ui.nameLabel->setAlignment(Qt::AlignCenter);         //设置字体垂直和水平居中
	ui.nameLabel->setText(name);
}

void TalkWindow::addEmotionImage(int name)
{
	ui.textEdit->setFocus();
	ui.textEdit->addEmotionHtml(name);
}

int TalkWindow::getTalkId()
{
	return talkId;
}

void TalkWindow::appendObjectMsg(const QJsonArray& jsonMsg, QString obj)
{
	//将消息添加到Html
	ui.msgWidget->appendMsg(jsonMsg, obj);
}

void TalkWindow::onTreeItemDoubleClicked(QTreeWidgetItem*item)
{
	bool isChild = item->data(0, Qt::UserRole).toBool();
	if (isChild)
	{
		QString talkId = item->data(0, Qt::UserRole + 1).toString();;
		if (talkId == LoginUserAccount) return;
		WindowManager::instance()->addNewTalkWindow(talkId.toInt());
	}
}

void TalkWindow::onFileOpenBtnClicked()
{
	SendFile* sendFile = new SendFile(this);
	sendFile->show();
}

/*
* 获取文本编辑器内容，转换为json格式
* @param document 文本编辑器内容
* @return json数组
*/
QJsonArray TalkWindow::parseDocumentToJson(QTextDocument* document)
{
	//遍历文档以查找和检索内容
	QTextCursor cursor(document);
	cursor.movePosition(QTextCursor::Start);     //将游标移动到文档的开头

	QJsonArray msgArray;    //msg Json数组，每个元素都是一个JsonArray（文本块）

	//检索文档内所有文本块
	while (!cursor.isNull())    //游标为空，则退出循环
	{
		QTextBlock block = cursor.block();      //当前文本块
		QTextBlock::Iterator it;                //文本块迭代器（用于检索文本块内片段）

		/*
		   在Qt中，文本片段通常是根据HTML标签来区分的
		   对于同一个片段，他们的QTextCharFormat（字符格式）是相同的，即显示出来的文本效果是相同的
		   对于同一个文本块内的两个图像，QTextDocument会将它们区分为两个不同的 QTextFragment（片段）
		*/
		QJsonArray blockArray;       //文本块Json数组（msg Json数组的元素）
		for (it = block.begin(); !(it.atEnd()); it++)
		{
			QTextFragment fragment = it.fragment();
			if (fragment.isValid())
			{
				if (fragment.charFormat().isImageFormat())   //当前片段的文本格式是图像格式
				{
					QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();       //获取图像格式信息
					QJsonValue value = imageFormat.name().replace(IMG_QRC, QString()).replace(IMG_PNG, QString());   //获取图片名称

					blockArray.append(QJsonObject{ {IMG, value} });        //将img对象插入文本块数组内
				}
				else     //该片段是文本
				{
					QString text = fragment.text();       //获取片段的纯文本
					//特殊字符转义，不自动解析为Html
					text.replace("&", "&amp");            //html内的转义字符基本都是&开头
					text.replace("<", "&lt");             //不把<>识别为html标签
					text.replace((QChar)8232, BR);        //如果有软换行符转出的字符，则替换为html里面的换行符<br>

					QJsonValue value = text;

					blockArray.append(QJsonObject{ {TXT, value} });         //将txt对象插入文本块数组内
				}
			}
		}

		msgArray.append(blockArray);        //将文本块数组插入msg数组内

		if (!cursor.movePosition(QTextCursor::NextBlock))  //移动到下一个文本块，移动失败（没有下一个文本块），则退出循环
		{
			break;
		}
	}

	//qDebug() << msgArray;

	return msgArray;
}

void TalkWindow::addMember(QTreeWidgetItem * rootItem, const QPair<QString, QString>& info)
{
	//子项
	QTreeWidgetItem* childItem = new QTreeWidgetItem;
	childItem->setData(0, Qt::UserRole, 1);
	childItem->setData(0, Qt::UserRole + 1, info.first);

	ContactItem* childWidget = new ContactItem(ui.treeWidget);

	//头像
	QPixmap src(info.second);
	QPixmap mask(":/Resources/MainWindow/head_mask.png");
	childWidget->setHeadPixmap(CommonUtils::getRoundedPixmap(src, mask, childWidget->getHeadLabelSize()));

	//用户名
	childWidget->setUserName(UseMySQL::instance()->getUserName(info.first));
	//签名
	childWidget->setSignName(UseMySQL::instance()->getUserSign(info.first));

	rootItem->addChild(childItem);
	ui.treeWidget->setItemWidget(childItem, 0, childWidget);

}

void TalkWindow::onSendBtnClicked()
{
	//如果全都是空白字符则不发送
	QString str = ui.textEdit->toPlainText();
	if (str.trimmed().isEmpty())
	{
		ui.textEdit->clear();
		QToolTip::showText(mapToGlobal(QPoint(600, 685)),
			QString::fromLocal8Bit("发送的信息不能为空！"), this, QRect(0, 0, 120, 100), 2000);
		return;
	}

	if (str.length() > MSG_LENGTH)      //单次消息长度限制
	{
		QToolTip::showText(mapToGlobal(QPoint(600, 685)),
			QString::fromLocal8Bit("发送的信息长度不能大于%1！").arg(MSG_LENGTH), this, QRect(0, 0, 120, 100), 2000);
		return;
	}

	QJsonArray msgArray = parseDocumentToJson(ui.textEdit->document());

	//将消息添加到Html
	ui.msgWidget->appendMsg(msgArray, MYSELF);
	ui.textEdit->clear();
}
