#pragma once

#include <QColor>

class ConfigIni
{
protected:
	ConfigIni() = default;
	~ConfigIni() = default;

public:
	//��ɫ����
	static void saveColor(const QColor& color);
	static QColor getColor();

	//�û�����
	static void saveUserProfile(const QString& account, const QString& password);
	static bool getUserProfile(QString& account, QString& password);
};

