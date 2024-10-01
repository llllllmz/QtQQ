#pragma once

#include <QTcpSocket>
#include <QHostAddress>
#include <QString>
#include <QByteArray>
#include <QDataStream>
#include <QMutex>
#include "define.h"
#include "User.h"
#include "UserGroup.h"
#include "FriendshipDTO.h"
#include <QMap>
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

	void login_response(bool,const QString&,User*);	//��¼��Ӧ todo ��Ҫ�ĳ�user���bool����
	void user_group_response(QMap<int, UserGroup*>);
	void friend_ship_response(QList<FriendshipDTO*>);
private slots:
	void onConnected();
	void onDisconnected();
	void onError(QAbstractSocket::SocketError error);
	void onReadyRead();
private:
	static ChatSocket *m_instance;

	QTcpSocket *tcpSocket;
};
