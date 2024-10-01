#pragma once
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>

class Friendship : public QObject
{
	Q_OBJECT
public:
	explicit Friendship(QObject *parent = nullptr) : QObject(parent) {}

	void setFriendshipId(int t_friendshipId) { this->friendshipId = t_friendshipId; }
	int getFriendshipId() const { return friendshipId; }
	void setUserId(int t_userId) { this->userId = t_userId; }
	int getUserId() const { return userId; }
	void setFriendId(int t_friendId) { this->friendId = t_friendId; }
	int getFriendId() const { return friendId; }
	void setUserGroupId(int t_userGroupId) { this->userGroupId = t_userGroupId; }
	int getUserGroupId() const { return userGroupId; }

	// JSON 序列化方法
	QJsonObject to_json() const
	{
		return QJsonObject{
			{"friendshipId", friendshipId},
			{"userId", userId},
			{"friendId", friendId},
			{"userGroupId", userGroupId}
		};
	}

	// JSON 反序列化方法
	bool from_json(const QJsonObject& j)
	{
		friendshipId = j.value("friendshipId").toInt();
		userId = j.value("userId").toInt();
		friendId = j.value("friendId").toInt();
		userGroupId = j.value("userGroupId").toInt();
		return true;
	}

private:
	int friendshipId;
	int userId;
	int friendId;
	int userGroupId;
};