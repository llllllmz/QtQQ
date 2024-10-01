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

User::User(const User& other)
	: QObject(other.parent()), m_id(other.m_id), m_username(other.m_username), m_password(other.m_password), m_signname(other.m_signname)
{}

User& User::operator=(const User& other)
{
	if (this != &other) {
		m_id = other.m_id;
		m_username = other.m_username;
		m_password = other.m_password;
		m_signname = other.m_signname;
	}
	return *this;
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
// JSON 序列化方法
QJsonObject User::to_json() const
{
	return QJsonObject{
		{"id", m_id},
		{"username", m_username},
		{"password", m_password},
		{"signname", m_signname}
	};
}

// JSON 反序列化方法
bool User::from_json(const QJsonObject& j)
{
	m_id = j.value("id").toInt();
	m_username = j.value("username").toString();
	m_password = j.value("password").toString();
	m_signname = j.value("signname").toString();
	return true;
}