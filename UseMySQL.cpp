#include "UseMySQL.h"
#include <QMessageBox>
#include <QDebug>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qsqlquerymodel.h>
#include "define.h"

//在第一次使用的时候会进行初始化，线程安全
Q_GLOBAL_STATIC(UseMySQL, theInstance);

UseMySQL::UseMySQL(QObject* parent)
	: QObject(parent)
{
	connectMySQL();
}

UseMySQL::~UseMySQL()
{
}

UseMySQL* UseMySQL::instance()
{
	return theInstance();
}

/*
* 验证登录
* @param account 账号
* @param password 密码
*/
bool UseMySQL::verifyLogin(const QString& account, const QString& password)
{
	QSqlQuery sql(QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();        //获取检索的密码
		if (result == password)
		{
			return true;
		}
	}

	return false;
}

/*
* 获取用户名
* @param account 账号
* @return 用户名
*/
QString UseMySQL::getUserName(const QString& account)
{
	QSqlQuery sql(QString("SELECT employeeName FROM tab_employees WHERE employeeID = %1").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}

	return QString();
}

QString UseMySQL::getUserSign(const QString & account)
{
	QSqlQuery sql(QString("SELECT employee_sign FROM tab_employees WHERE employeeID = %1").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}
	return QString();
}

/*
* 获取用户头像
* @param account 账号
* @return 头像路径
*/
QString UseMySQL::getUserHeadPixmap(const QString& account)
{
	QSqlQuery sql(QString("SELECT picture FROM tab_employees WHERE employeeID = %1").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}

	return QString();
}

/*
* 获取用户群组
* @param account 账号
* @return 群组id列表
*/
QList<int> UseMySQL::getUserGroup(const QString& account)
{
	QList<int> result;
	QSqlQuery sql(base);

	//公司群
	if (sql.exec(QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QString::fromLocal8Bit("公司群"))))
	{
		if (sql.next())	result.append(sql.value(0).toInt());
	}

	//部门群
	if (sql.exec(QString("SELECT departmentID FROM tab_employees WHERE employeeID = '%1'").arg(account)))
	{
		if (sql.next())	result.append(sql.value(0).toInt());
	}

	return result;
}

/*
* 获取群组头像
* @param id 群组id
* @return 头像路径
*/
QString UseMySQL::getGroupHeadPixmap(const int id)
{
	QSqlQuery sql(QString("SELECT picture FROM tab_department WHERE departmentID = %1").arg(id), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}

	return QString();
}

/*
* 获取群组名称
* @param id 群组id
* @return 群组名称
*/
QString UseMySQL::getGroupName(const int id)
{
	QSqlQuery sql(QString("SELECT departmentName FROM tab_department WHERE departmentID = %1").arg(id), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}

	return QString();
}

/*
* 获取用户部门
* @param account 账号
* @return 部门名称
*/
QString UseMySQL::getUserDepartment(const QString& account)
{
	QSqlQuery sql(QString("SELECT departmentName FROM tab_department WHERE departmentID =    \
		(SELECT departmentID FROM tab_employees WHERE employeeID = %1);").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();
		return result;
	}

	return QString();
}

/*
* 获取所有用户id
* @return 用户id列表
*/
QList<int> UseMySQL::getUserList()
{
	QSqlQueryModel sql;       //查询模型
	sql.setQuery(QString("SELECT employeeID From tab_employees WHERE status = 1"), base);        //执行查询

	QList<int> result;
	int rowCount = sql.rowCount();      //获取行数
	if (rowCount > 0)
	{
		for (int i = 0; i < rowCount; i++)
		{
			QModelIndex index = sql.index(i, 0);
			result.append(sql.data(index).toInt());
		}
	}

	return result;
}

/*
* 获取注册对象信息
* @param id 窗体id
* @param infoList 信息列表 QList的每个元素都是一条信息，第一位是account，第二位是picture
* @return 查询是否成功
*/
 bool UseMySQL::getRegisterObjectInfo(const QString& id, QList<QPair<QString, QString>>& infoList)
{
	//获取公司群ID
	QSqlQuery sql(QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QString::fromLocal8Bit("公司群")), base);
	if (!sql.next()) return false;     

	QString	companyID = sql.value(0).toString();

	//判断聊天窗口类型
	QSqlQueryModel model;
	if (id == companyID) 
		model.setQuery("SELECT employeeID, picture FROM tab_employees WHERE status = 1", base);
	else
	{
		//if (id.length() <= GROUP_ID_LENGTH)    //其它群聊
		//	model.setQuery(QString("SELECT employeeID, picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(id), base);
		//else                     //单聊
			model.setQuery(QString("SELECT employeeID, picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(id), base);
	}

	int rowCount = model.rowCount();      //获取行数
	if (rowCount <= 0) return false;

	for (int i = 0; i < rowCount; i++)
	{
		QModelIndex accountIndex = model.index(i, 0);
		QModelIndex pictureIndex = model.index(i, 1);

		QString account = model.data(accountIndex).toString();       //账号
		QString picture = model.data(pictureIndex).toString();       //头像
		infoList.append(qMakePair<QString, QString>(account, picture));
	}

	return true;
}

bool UseMySQL::connectMySQL()
{
	qDebug() << QSqlDatabase::drivers();
	//连接数据库
	base = QSqlDatabase::addDatabase("QMYSQL", "main");
	base.setDatabaseName("qtqq");      //数据库名称
	base.setHostName("localHost");          //主机名称
	base.setUserName("root");               //用户名
	base.setPassword("root");          //密码
	base.setPort(3307);                     //端口号

	if (base.open())
	{
		//QMessageBox::information(nullptr, "Tip", "Successed to open database", QMessageBox::Yes);
		return true;
	}
	else
	{
		QMessageBox::information(nullptr, "Tip", "Failed to open database", QMessageBox::Cancel);
		qDebug() << base.lastError().text();
		return false;
	}
}
