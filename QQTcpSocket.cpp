//#include "QQTcpSocket.h"
//#include <qdebug.h>
//Q_GLOBAL_STATIC(QQTcpSocket,theInstance)
//QQTcpSocket::QQTcpSocket(QObject *parent)
//	: QObject(parent)
//{
//	connect(&tcpSocket, &QTcpSocket::readyRead,this,&QQTcpSocket::on_tcp_ready_read);
//}
//
//void QQTcpSocket::on_tcp_ready_read()
//{
//	QByteArray data = tcpSocket.readAll();
//	qDebug() << data;
//}
//
//QQTcpSocket::~QQTcpSocket()
//{
//}
//
//void QQTcpSocket::connectToHost()
//{
//
//	tcpSocket.connectToHost("192.168.58.204", 2007);
//}
//
//QTcpSocket * QQTcpSocket::getTcpSocket()
//{
//	return &tcpSocket;
//}
//
//QQTcpSocket * QQTcpSocket::instance()
//{
//	return theInstance();
//}
