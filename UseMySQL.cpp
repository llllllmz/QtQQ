#include "UseMySQL.h"
#include <QMessageBox>
#include <QDebug>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qsqlquerymodel.h>
#include "define.h"

//�ڵ�һ��ʹ�õ�ʱ�����г�ʼ�����̰߳�ȫ
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
* ��֤��¼
* @param account �˺�
* @param password ����
*/
bool UseMySQL::verifyLogin(const QString& account, const QString& password)
{
	QSqlQuery sql(QString("SELECT code FROM tab_accounts WHERE employeeID = %1").arg(account), base);
	if (sql.next())
	{
		QString result = sql.value(0).toString();        //��ȡ����������
		if (result == password)
		{
			return true;
		}
	}

	return false;
}

/*
* ��ȡ�û���
* @param account �˺�
* @return �û���
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
* ��ȡ�û�ͷ��
* @param account �˺�
* @return ͷ��·��
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
* ��ȡ�û�Ⱥ��
* @param account �˺�
* @return Ⱥ��id�б�
*/
QList<int> UseMySQL::getUserGroup(const QString& account)
{
	QList<int> result;
	QSqlQuery sql(base);

	//��˾Ⱥ
	if (sql.exec(QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ"))))
	{
		if (sql.next())	result.append(sql.value(0).toInt());
	}

	//����Ⱥ
	if (sql.exec(QString("SELECT departmentID FROM tab_employees WHERE employeeID = '%1'").arg(account)))
	{
		if (sql.next())	result.append(sql.value(0).toInt());
	}

	return result;
}

/*
* ��ȡȺ��ͷ��
* @param id Ⱥ��id
* @return ͷ��·��
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
* ��ȡȺ������
* @param id Ⱥ��id
* @return Ⱥ������
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
* ��ȡ�û�����
* @param account �˺�
* @return ��������
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
* ��ȡ�����û�id
* @return �û�id�б�
*/
QList<int> UseMySQL::getUserList()
{
	QSqlQueryModel sql;       //��ѯģ��
	sql.setQuery(QString("SELECT employeeID From tab_employees WHERE status = 1"), base);        //ִ�в�ѯ

	QList<int> result;
	int rowCount = sql.rowCount();      //��ȡ����
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
* ��ȡע�������Ϣ
* @param id ����id
* @param infoList ��Ϣ�б� QList��ÿ��Ԫ�ض���һ����Ϣ����һλ��account���ڶ�λ��picture
* @return ��ѯ�Ƿ�ɹ�
*/
 bool UseMySQL::getRegisterObjectInfo(const QString& id, QList<QPair<QString, QString>>& infoList)
{
	//��ȡ��˾ȺID
	QSqlQuery sql(QString("SELECT departmentID FROM tab_department WHERE departmentName = '%1'").arg(QString::fromLocal8Bit("��˾Ⱥ")), base);
	if (!sql.next()) return false;     

	QString	companyID = sql.value(0).toString();

	//�ж����촰������
	QSqlQueryModel model;
	if (id == companyID) 
		model.setQuery("SELECT employeeID, picture FROM tab_employees WHERE status = 1", base);
	else
	{
		//if (id.length() <= GROUP_ID_LENGTH)    //����Ⱥ��
		//	model.setQuery(QString("SELECT employeeID, picture FROM tab_employees WHERE status = 1 AND departmentID = %1").arg(id), base);
		//else                     //����
			model.setQuery(QString("SELECT employeeID, picture FROM tab_employees WHERE status = 1 AND employeeID = %1").arg(id), base);
	}

	int rowCount = model.rowCount();      //��ȡ����
	if (rowCount <= 0) return false;

	for (int i = 0; i < rowCount; i++)
	{
		QModelIndex accountIndex = model.index(i, 0);
		QModelIndex pictureIndex = model.index(i, 1);

		QString account = model.data(accountIndex).toString();       //�˺�
		QString picture = model.data(pictureIndex).toString();       //ͷ��
		infoList.append(qMakePair<QString, QString>(account, picture));
	}

	return true;
}

bool UseMySQL::connectMySQL()
{
	qDebug() << QSqlDatabase::drivers();
	//�������ݿ�
	base = QSqlDatabase::addDatabase("QMYSQL", "main");
	base.setDatabaseName("qtqq");      //���ݿ�����
	base.setHostName("localHost");          //��������
	base.setUserName("root");               //�û���
	base.setPassword("root");          //����
	base.setPort(3307);                     //�˿ں�

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
