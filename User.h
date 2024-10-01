// User.hpp
#pragma once
#include <QObject>
#include <QJsonObject>
class User : public QObject
{
	Q_OBJECT
public:
	User(QObject *parent = nullptr);
	User(int id, const QString &username, const QString &password, const QString &signname, QObject *parent = nullptr);
	~User();

	// �������캯��
	User(const User& other);

	// ��ֵ�����
	User& operator=(const User& other);

	// Setters
	void setId(int id);
	void setUsername(const QString &username);
	void setPassword(const QString &password);
	void setSignName(const QString &signname);

	// Getters
	int getId() const;
	QString getUsername() const;
	QString getPassword() const;
	QString getSignName() const;

	// JSON ���л�����
	QJsonObject to_json() const;

	// JSON �����л�����
	bool from_json(const QJsonObject& j);

private:
	int m_id;
	QString m_username;
	QString m_password;
	QString m_signname;
};
