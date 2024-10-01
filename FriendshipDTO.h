// FriendshipDTO.hpp
#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include "Friendship.h"
#include "User.h"

class FriendshipDTO : public Friendship
{
	Q_OBJECT
public:
	explicit FriendshipDTO(QObject *parent = nullptr) : Friendship(parent) {}
	~FriendshipDTO() {}

	User getUser() const { return user; }
	void setUser(const User& user) { this->user = user; }

	// JSON ���л�����
	QJsonObject to_json() const
	{
		QJsonObject j = Friendship::to_json();
		j["user"] = user.to_json();
		return j;
	}

	// JSON �����л�����
	bool from_json(const QJsonObject& j)
	{
		Friendship::from_json(j);
		user.from_json(j.value("user").toObject());
		return true;
	}

private:
	User user;
};