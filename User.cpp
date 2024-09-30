#include "User.h"

User::User(QObject *parent)
	: QObject(parent)
{
}

User::User(int id, const QString &username, const QString &password, const QString &signname, QObject *parent)
	: QObject(parent),
	m_id(id),
	m_username(username),
	m_password(password),
	m_signname(signname)
{
}

User::~User()
{
}

void User::setId(int id)
{
	if (m_id == id)
		return;

	m_id = id;
}

void User::setUsername(const QString &username)
{
	if (m_username == username)
		return;

	m_username = username;
}

void User::setPassword(const QString &password)
{
	if (m_password == password)
		return;

	m_password = password;
}

void User::setSignName(const QString &signname)
{
	if (m_signname == signname)
		return;

	m_signname = signname;
}

int User::getId() const
{
	return m_id;
}

QString User::getUsername() const
{
	return m_username;
}

QString User::getPassword() const
{
	return m_password;
}

QString User::getSignName() const
{
	return m_signname;
}