#include "ConfigIni.h"
#include <qsettings.h>
#include <QFile>
#include <QApplication>
#include "define.h"

//保存颜色到配置文件
void ConfigIni::saveColor(const QColor& color)
{
	const QString path = INI_PATH("color.ini");
	QSettings set(path, QSettings::IniFormat);         //将设置存储在ini文件中

	set.setValue("COLOR/red", color.red());
	set.setValue("COLOR/green", color.green());
	set.setValue("COLOR/blue", color.blue());
}

//从配置文件读取颜色
QColor ConfigIni::getColor()
{
	const QString path = INI_PATH("color.ini");

	if (!QFile::exists(path))      //如果文件不存在，则初始化一个
	{
		saveColor(QColor(22, 154, 218));
	}
	
	QSettings set(path, QSettings::IniFormat);         //从ini文件中读取设置
	int red = set.value("COLOR/red").toInt();
	int green = set.value("COLOR/green").toInt();
	int blue = set.value("COLOR/blue").toInt();

	return QColor(red, green, blue);
}

//保存用户配置
void ConfigIni::saveUserProfile(const QString& account, const QString& password)
{
	const QString path = INI_PATH("user.ini");

	QSettings set(path, QSettings::IniFormat);
	set.setValue("USER/account", account);
	set.setValue("USER/password", password);
}

//读取用户配置
bool ConfigIni::getUserProfile(QString& account, QString& password)
{
	const QString path = INI_PATH("user.ini");

	if (!QFile::exists(path))      //配置文件不存在
	{
		return false;
	}

	QSettings set(path, QSettings::IniFormat);
	account = set.value("USER/account").toString();
	password = set.value("USER/password").toString();

	return true;
}
