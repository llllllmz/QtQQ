#include "ConfigIni.h"
#include <qsettings.h>
#include <QFile>
#include <QApplication>
#include "define.h"

//������ɫ�������ļ�
void ConfigIni::saveColor(const QColor& color)
{
	const QString path = INI_PATH("color.ini");
	QSettings set(path, QSettings::IniFormat);         //�����ô洢��ini�ļ���

	set.setValue("COLOR/red", color.red());
	set.setValue("COLOR/green", color.green());
	set.setValue("COLOR/blue", color.blue());
}

//�������ļ���ȡ��ɫ
QColor ConfigIni::getColor()
{
	const QString path = INI_PATH("color.ini");

	if (!QFile::exists(path))      //����ļ������ڣ����ʼ��һ��
	{
		saveColor(QColor(22, 154, 218));
	}
	
	QSettings set(path, QSettings::IniFormat);         //��ini�ļ��ж�ȡ����
	int red = set.value("COLOR/red").toInt();
	int green = set.value("COLOR/green").toInt();
	int blue = set.value("COLOR/blue").toInt();

	return QColor(red, green, blue);
}

//�����û�����
void ConfigIni::saveUserProfile(const QString& account, const QString& password)
{
	const QString path = INI_PATH("user.ini");

	QSettings set(path, QSettings::IniFormat);
	set.setValue("USER/account", account);
	set.setValue("USER/password", password);
}

//��ȡ�û�����
bool ConfigIni::getUserProfile(QString& account, QString& password)
{
	const QString path = INI_PATH("user.ini");

	if (!QFile::exists(path))      //�����ļ�������
	{
		return false;
	}

	QSettings set(path, QSettings::IniFormat);
	account = set.value("USER/account").toString();
	password = set.value("USER/password").toString();

	return true;
}
