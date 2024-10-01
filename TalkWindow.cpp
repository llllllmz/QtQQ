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
extern QString LoginUserAccount;       //������CCMainWindow.cpp

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
//	if (QString::number(talkId).length() <= GROUP_ID_LENGTH)       //Ⱥ��
//		isGroupTalk = true;
//	else 
//		isGroupTalk = false;
//}

void TalkWindow::initStyle()
{
	ui.bodySplitter->setSizes(QList<int>{600, 138});
	ui.textEdit->setFocus();                  //���ý���
}

void TalkWindow::initConnect()
{
	//TalkWindow����ʱ��parent()ΪTalkWindowShell
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
	//����
	QTreeWidgetItem* rootItem = new QTreeWidgetItem;
	rootItem->setData(0, Qt::UserRole, 0);	

	//��ȡȺ��Ա��Ϣ
	QList<QPair<QString, QString>> infoList;
	if (!UseMySQL::instance()->getRegisterObjectInfo(QString::number(talkId),infoList))
	{
		QMessageBox::information(nullptr, "erro", "getRegisterObjectInfo Error.");
		return;
	}
	RootContactItem* rootWidget = new RootContactItem(false, ui.treeWidget);
	rootWidget->setText(QString::fromLocal8Bit("%1 ��Ա-%2").arg(UseMySQL::instance()->getGroupName(talkId)).arg(infoList.count()));

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
	ui.nameLabel->setStyleSheet("color: rgb(112, 114, 113); font: bold;");     //����������ɫ
	ui.nameLabel->setAlignment(Qt::AlignCenter);         //�������崹ֱ��ˮƽ����
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
	//����Ϣ��ӵ�Html
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
* ��ȡ�ı��༭�����ݣ�ת��Ϊjson��ʽ
* @param document �ı��༭������
* @return json����
*/
QJsonArray TalkWindow::parseDocumentToJson(QTextDocument* document)
{
	//�����ĵ��Բ��Һͼ�������
	QTextCursor cursor(document);
	cursor.movePosition(QTextCursor::Start);     //���α��ƶ����ĵ��Ŀ�ͷ

	QJsonArray msgArray;    //msg Json���飬ÿ��Ԫ�ض���һ��JsonArray���ı��飩

	//�����ĵ��������ı���
	while (!cursor.isNull())    //�α�Ϊ�գ����˳�ѭ��
	{
		QTextBlock block = cursor.block();      //��ǰ�ı���
		QTextBlock::Iterator it;                //�ı�������������ڼ����ı�����Ƭ�Σ�

		/*
		   ��Qt�У��ı�Ƭ��ͨ���Ǹ���HTML��ǩ�����ֵ�
		   ����ͬһ��Ƭ�Σ����ǵ�QTextCharFormat���ַ���ʽ������ͬ�ģ�����ʾ�������ı�Ч������ͬ��
		   ����ͬһ���ı����ڵ�����ͼ��QTextDocument�Ὣ��������Ϊ������ͬ�� QTextFragment��Ƭ�Σ�
		*/
		QJsonArray blockArray;       //�ı���Json���飨msg Json�����Ԫ�أ�
		for (it = block.begin(); !(it.atEnd()); it++)
		{
			QTextFragment fragment = it.fragment();
			if (fragment.isValid())
			{
				if (fragment.charFormat().isImageFormat())   //��ǰƬ�ε��ı���ʽ��ͼ���ʽ
				{
					QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();       //��ȡͼ���ʽ��Ϣ
					QJsonValue value = imageFormat.name().replace(IMG_QRC, QString()).replace(IMG_PNG, QString());   //��ȡͼƬ����

					blockArray.append(QJsonObject{ {IMG, value} });        //��img��������ı���������
				}
				else     //��Ƭ�����ı�
				{
					QString text = fragment.text();       //��ȡƬ�εĴ��ı�
					//�����ַ�ת�壬���Զ�����ΪHtml
					text.replace("&", "&amp");            //html�ڵ�ת���ַ���������&��ͷ
					text.replace("<", "&lt");             //����<>ʶ��Ϊhtml��ǩ
					text.replace((QChar)8232, BR);        //��������з�ת�����ַ������滻Ϊhtml����Ļ��з�<br>

					QJsonValue value = text;

					blockArray.append(QJsonObject{ {TXT, value} });         //��txt��������ı���������
				}
			}
		}

		msgArray.append(blockArray);        //���ı����������msg������

		if (!cursor.movePosition(QTextCursor::NextBlock))  //�ƶ�����һ���ı��飬�ƶ�ʧ�ܣ�û����һ���ı��飩�����˳�ѭ��
		{
			break;
		}
	}

	//qDebug() << msgArray;

	return msgArray;
}

void TalkWindow::addMember(QTreeWidgetItem * rootItem, const QPair<QString, QString>& info)
{
	//����
	QTreeWidgetItem* childItem = new QTreeWidgetItem;
	childItem->setData(0, Qt::UserRole, 1);
	childItem->setData(0, Qt::UserRole + 1, info.first);

	ContactItem* childWidget = new ContactItem(ui.treeWidget);

	//ͷ��
	QPixmap src(info.second);
	QPixmap mask(":/Resources/MainWindow/head_mask.png");
	childWidget->setHeadPixmap(CommonUtils::getRoundedPixmap(src, mask, childWidget->getHeadLabelSize()));

	//�û���
	childWidget->setUserName(UseMySQL::instance()->getUserName(info.first));
	//ǩ��
	childWidget->setSignName(UseMySQL::instance()->getUserSign(info.first));

	rootItem->addChild(childItem);
	ui.treeWidget->setItemWidget(childItem, 0, childWidget);

}

void TalkWindow::onSendBtnClicked()
{
	//���ȫ���ǿհ��ַ��򲻷���
	QString str = ui.textEdit->toPlainText();
	if (str.trimmed().isEmpty())
	{
		ui.textEdit->clear();
		QToolTip::showText(mapToGlobal(QPoint(600, 685)),
			QString::fromLocal8Bit("���͵���Ϣ����Ϊ�գ�"), this, QRect(0, 0, 120, 100), 2000);
		return;
	}

	if (str.length() > MSG_LENGTH)      //������Ϣ��������
	{
		QToolTip::showText(mapToGlobal(QPoint(600, 685)),
			QString::fromLocal8Bit("���͵���Ϣ���Ȳ��ܴ���%1��").arg(MSG_LENGTH), this, QRect(0, 0, 120, 100), 2000);
		return;
	}

	QJsonArray msgArray = parseDocumentToJson(ui.textEdit->document());

	//����Ϣ��ӵ�Html
	ui.msgWidget->appendMsg(msgArray, MYSELF);
	ui.textEdit->clear();
}
