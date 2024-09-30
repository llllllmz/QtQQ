#pragma once
#include <QObject>
class User : public QObject
{
	Q_OBJECT
public:
	explicit User(QObject *parent = nullptr);
	User(int id, const QString &username, const QString &password, const QString &signname, QObject *parent = nullptr);
	~User();
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

private:
	int m_id;
	QString m_username;
	QString m_password;
	QString m_signname;
};
