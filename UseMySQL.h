#pragma once

#include <QObject>
#include <QSqlDatabase>

class UseMySQL  : public QObject
{
	Q_OBJECT

public:
	UseMySQL(QObject *parent = nullptr);
	~UseMySQL();

	static UseMySQL* instance();    //µ¥Àý

public:
	bool verifyLogin(const QString& account, const QString& password);
	QString getUserName(const QString& account);
	QString getUserSign(const QString& account);
	QString getUserHeadPixmap(const QString& account);
	QList<int> getUserGroup(const QString& account);
	QString getGroupHeadPixmap(const int id);
	QString getGroupName(const int id);
	QString getUserDepartment(const QString& account);
	QList<int> getUserList();
	bool getRegisterObjectInfo(const QString& id, QList<QPair<QString, QString>>& infoList);

private:
	bool connectMySQL();

private:
	QSqlDatabase base;
};
