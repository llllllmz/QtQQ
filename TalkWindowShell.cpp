#include "TalkWindowShell.h"
#include "CommonUtils.h"
#include "TalkWindow.h"
#include "TalkWindowItem.h"
#include "UseMySQL.h"
#include "CustomProxyStyle.h"
#include <qfile.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qdebug.h>
#include <qjsonobject.h>
#include <qjsondocument.h>
#include <qjsonarray.h>
#include "WindowManager.h"
#include "RecvFile.h"
extern QString LoginUserAccount;       //������CCMainWindow.cpp

TalkWindowShell::TalkWindowShell(QWidget *parent)
	: BasicWindow(parent), emotionWindow(nullptr)
{
	ui.setupUi(this);

	setAttribute(Qt::WA_DeleteOnClose);
	init();
}

TalkWindowShell::~TalkWindowShell()
{
	if (emotionWindow) emotionWindow->deleteLater();
}

void TalkWindowShell::init()
{
	initStyle();
	connect(ui.listWidget, &QListWidget::itemClicked, this, &TalkWindowShell::onListItemClicked);
	initEmotionWindow();
	initJsFile();
	initTcpSocket();
	initUdpSocket();
}

void TalkWindowShell::initStyle()
{
	loadStyleSheet("TalkWindow");
	ui.listWidget->setStyle(new CustomProxyStyle());
	ui.splitter->setSizes(QList<int>{154, width() - 154});
}

void TalkWindowShell::initEmotionWindow()
{
	emotionWindow = new EmotionWindow;
	emotionWindow->hide();
	connect(emotionWindow, &EmotionWindow::addEmotion, this, &TalkWindowShell::onAddEmotion);
}

void TalkWindowShell::initJsFile()
{
	//��ȡmsgPageTmp.txt
	QFile tmpFile(":/Resources/MainWindow/MsgHtml/msgPageTmp.txt");
	QString tmpFileContent;
	if (tmpFile.open(QFile::ReadOnly))
	{
		tmpFileContent = tmpFile.readAll();
		tmpFile.close();
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Read msgPageTmp.txt Error.");
		return;
	}

	if (!replaceJsTxt(tmpFileContent)) return;

	/*
	    qrc�ļ��ڱ���ʱ��Ƕ�뵽Ӧ�ó���Ķ������ļ��У����������ʱ�����ǲ��ɱ��
		qrc�ļ����ܱ��޸ģ�ֻ����ֻ����ʽ�򿪣��������������·��
		���·��������ڹ���Ŀ¼����Ŀ����->����->����Ŀ¼
		����msgPage.html�����õ��ļ�
	*/
	QFile js("./Resources/MainWindow/MsgHtml/msghandling.js");
	if (js.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream stream(&js);
		stream << tmpFileContent;
		js.close();
		return;
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Write msghandling.js Error.");
		return;
	}
}

/*
* @brief: �滻js.txt�ļ�����
* @param: fileContent: �ļ�����
* @return: bool: �Ƿ��滻�ɹ�
*/
bool TalkWindowShell::replaceJsTxt(QString& fileContent)
{
	//��Ҫ�滻���ı�
	QString replaceVar = "var external = null;";                      //��������
	QString replaceObj = "external = channel.objects.external;";      //�������ֵ
	QString replaceRecv;                                              //recv����
	QFile recvFile(":/Resources/MainWindow/MsgHtml/recvHtmlTmp.txt");
	if (recvFile.open(QFile::ReadOnly))
	{
		replaceRecv = recvFile.readAll();
		recvFile.close();
	}
	else
	{
		QMessageBox::information(nullptr, "error", "Read recvHtmlTmp.txt Error.");
		return false;
	}

	//��ȡ�û��б�
	QList<int> userList = UseMySQL::instance()->getUserList();

	//�滻�ı�����
	QString var, obj, recv;
	for (int id : userList)     //ÿ��ѭ����userList�л�ȡһ���û�id
	{
		QString tmpVar = replaceVar;
		tmpVar.replace("external", QString("external_%1").arg(id));
		var += (tmpVar + "\n");

		QString tmpObj = replaceObj;
		tmpObj.replace("external", QString("external_%1").arg(id));
		obj += (tmpObj + "\n");

		QString tmpRecv = replaceRecv;
		tmpRecv.replace("recvHtml", QString("recvHtml_%1").arg(id));
		tmpRecv.replace("external", QString("external_%1").arg(id));
		recv += (tmpRecv + "\n");
	}

	fileContent.replace(replaceVar, var);
	fileContent.replace(replaceObj, obj);
	fileContent.replace(replaceRecv, recv);

	return true;
}

void TalkWindowShell::initTcpSocket()
{
	tcp = new QTcpSocket(this);
	tcp->connectToHost("127.0.0.1", tcpPort);
}

void TalkWindowShell::initUdpSocket()
{
	udp = new QUdpSocket(this);

	//udp�󶨶˿ںţ��������׽��ֹ���ͬһ����ַ�Ͷ˿�
	if (!udp->bind(udpPort, QAbstractSocket::ReuseAddressHint))
	{
		QMessageBox::information(nullptr, "error", "Udp socket bind Error.");
		return;
	}

	connect(udp, &QUdpSocket::readyRead, this, &TalkWindowShell::readMsgFromServer);
}

void TalkWindowShell::addTalkWindow(TalkWindow* window, TalkWindowItem* windowItem)
{
	ui.rightStackedWidget->addWidget(window);    //���촰����ӵ���ջ����
	ui.rightStackedWidget->setCurrentWidget(window);

	QListWidgetItem* item = new QListWidgetItem(ui.listWidget);
	ui.listWidget->addItem(item);
	ui.listWidget->setItemWidget(item, windowItem);     //��������ӵ��б�
	item->setSelected(true);         //��Ӧ�б�������Ϊѡ��״̬

	map.insert(item, window);        //��ӵ�ӳ��

	connect(windowItem, &TalkWindowItem::signalCloseBtnClicked, this, [window, windowItem, item, this]()
	{
		map.remove(item);      //ɾ��������keyΪitem����

		//listWidget
		ui.listWidget->takeItem(ui.listWidget->row(item));      //�Ƴ������е���
		delete item;
		windowItem->setParent(nullptr);
		windowItem->deleteLater();

		//stackedWidget
		ui.rightStackedWidget->removeWidget(window);
		window->setParent(nullptr);
		window->deleteLater();

		if (ui.rightStackedWidget->count() == 0)
		{
			this->close();          //�ر�talkWindowShell
		}
	});
}

void TalkWindowShell::updateCurrentWindow(QWidget* window)
{
	QListWidgetItem* item = map.key(window, nullptr);
	if (item)
	{
		item->setSelected(true);
		ui.rightStackedWidget->setCurrentWidget(window);
	}
}

void TalkWindowShell::onEmotionBtnClicked()
{
	//���鰴ť���֮�󣬱��鴰����ʾ/����
	emotionWindow->setVisible(!emotionWindow->isVisible());

	//��ʾ���鴰��
	QPoint pos = this->mapToGlobal(QPoint(0, 0));       //����������Ͻ�����ת��Ϊȫ������
	QPoint movePos = pos + QPoint(170, 220);
	emotionWindow->move(movePos);
}

void TalkWindowShell::onAddEmotion(int name)
{
	emotionWindow->hide();     //���ر��鴰��
	//��ȡTalkWindow
	TalkWindow* window = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	if (window)
	{
		window->addEmotionImage(name);
	}
}

void TalkWindowShell::onListItemClicked(QListWidgetItem* item)
{
	/*
	* ������ڣ��򷵻ش��ڣ����򷵻�nullptr
	*/
	QWidget* widget = map.value(item, nullptr);
	if (widget)
	{
		ui.rightStackedWidget->setCurrentWidget(widget);
	}
}

void TalkWindowShell::sendMsgToServer(QJsonValue data)
{
	//��ȡ�����
	TalkWindow* window = dynamic_cast<TalkWindow*>(ui.rightStackedWidget->currentWidget());
	const QString talkID = QString::number(window->getTalkId());

	QJsonObject msgObj;     //��Ϣ����

	//���촰������
	//if (talkID.length() <= GROUP_ID_LENGTH)                      //Ⱥ��
	//	msgObj.insert(TYPE, TYPE_GROUP);
	//else                                                         //����
		msgObj.insert(TYPE, TYPE_SINGLE);

	//������/�����ߣ�Ⱥ��/Ա��ID��
	msgObj.insert(SEND, LoginUserAccount);
	msgObj.insert(RECV, talkID);

	//��Ϣ����
	switch (data.type())
	{
	case QJsonValue::Array:         //��ͨ��Ϣ
	{
		msgObj.insert(WHAT, WHAT_MSG);
		msgObj.insert(WHAT_MSG, data);
		break;
	}
	case QJsonValue::Object:        //�ļ���Ϣ
	{
		msgObj.insert(WHAT, WHAT_FILE);
		msgObj.insert(WHAT_FILE, data);
		break;
	}
	default:
	{
		QMessageBox::information(nullptr, "error", "Other Msg Type.");
		return;
	}
	}

	//������Ϣ��������
	QString msg = QJsonDocument(msgObj).toJson(QJsonDocument::Compact);         //��QJsonObjectת��Ϊ���ո�ʽ��JSON�ַ���

	qDebug() << msg;

	if (tcp->state() != QAbstractSocket::ConnectedState)      //δ����״̬
	{
		QMessageBox::information(nullptr, "error", "TCP socket connect Error.");
		return;
	}
	tcp->write(msg.toUtf8());
}

void TalkWindowShell::readMsgFromServer()
{
	//�˿�����δ���������
	while (udp->hasPendingDatagrams())    
	{
		QByteArray msg(udp->pendingDatagramSize(), 0);
		udp->readDatagram(msg.data(), msg.size());       //���ղ�����msg.size()�ֽڵ����ݱ���������洢��data��

		//����Ϣ�ַ�����Ϊjson����
		QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
		if (jsonDoc.isNull() || !jsonDoc.isObject()) return;       //json��ʽ����
		QJsonObject jsonObj = jsonDoc.object();          //�����ĵ��а�����QJsonObject

		/*      ����json����        */

		QString sender = jsonObj[SEND].toString();       //������
		if (sender == LoginUserAccount) return;          //�Լ����͵���Ϣ��������

		QString type = jsonObj[TYPE].toString();         //���촰������
		QString receiver = jsonObj[RECV].toString();     //�����ߣ�Ⱥ��/Ա��ID��

		QString talkId = receiver;                       //���촰��ID
		if (type == TYPE_SINGLE)            //����ǵ���
		{
			if (receiver != LoginUserAccount) return;    //���Ҳ��Ƿ����Լ�����Ϣ���򷵻�
			talkId = sender;
		}

		QString what = jsonObj[WHAT].toString();         //��Ϣ����
		QJsonValue data = jsonObj[what];                 //��Ϣ����

		QJsonArray msgArray;
		if (what == WHAT_MSG)                   //��ͨ��Ϣ
		{
			msgArray = data.toArray();
		}
		else if (what == WHAT_FILE)             //�ļ���Ϣ
		{
			QJsonObject fileObj = data.toObject();

			RecvFile* recvFile = new RecvFile(fileObj, this);
			QString msg = QString::fromLocal8Bit("�յ�����")
				+ UseMySQL::instance()->getUserName(sender)
				+ QString::fromLocal8Bit("�������ļ����Ƿ���գ�");
			recvFile->setLabelMsg(msg);
			recvFile->show();
		}

		/*
			��ȡ��Ӧ�����촰�ڣ���������ڣ�����Ը���Ϣ
			����Ⱥ�ģ����촰�ڲ����ڷ�Ϊ���������
				1.û�ж�ӦȺ��
				2.�ж�ӦȺ�ģ�����û�д�
		*/
		QWidget* widget = WindowManager::instance()->findWindowMap(talkId.toInt());
		if (!widget) return;        //���촰��δ��

		this->updateCurrentWindow(widget);
		dynamic_cast<TalkWindow*>(widget)->appendObjectMsg(msgArray, sender);
	}
}