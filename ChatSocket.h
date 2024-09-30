#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QMutex>
#include "define.h"
#include "User.h"
class ChatSocket : public QObject
{
	Q_OBJECT
public:
	static ChatSocket *instance();

	ChatSocket(QObject *parent = nullptr);
	~ChatSocket();

	bool isConnected() const;
	void sendText(EventType eventType, const QByteArray &data);

	void connectToHost(const QString &hostName, quint16 port);
	void disconnectFromHost();

signals:
	void connected();
	void disconnected();
	void errorOccurred(QAbstractSocket::SocketError error);
	void readyRead();

	void login_response(bool,const QString&,User*);	//登录响应 todo 需要改成user类和bool类型

private slots:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError error);
	void onReadyRead();
private:
	static ChatSocket *m_instance;

	QTcpSocket *tcpSocket;
};
