#include "UserGroup.h"

// 构造函数和析构函数
UserGroup::UserGroup(QObject *parent)
	:QObject(parent)
{
}

UserGroup::~UserGroup() {}

void UserGroup::setUserGroupId(int t_userGroupId)
{
	userGroupId = t_userGroupId;
}

int UserGroup::getUserGroupId() const
{
	return userGroupId;
}

void UserGroup::setGroupName(const QString& t_groupName)
{
	groupName = t_groupName;
}

QString UserGroup::getGroupName() const
{
	return groupName;
}

void UserGroup::setUserId(int t_userId)
{
	userId = t_userId;
}

int UserGroup::getUserId() const
{
	return userId;
}

void UserGroup::setSortOrder(int t_sortOrder)
{
	sortOrder = t_sortOrder;
}

int UserGroup::getSortOrder() const
{
	return sortOrder;
}

void UserGroup::to_json(QJsonObject& j) const
{
	j["userGroupId"] = userGroupId;
	j["groupName"] = groupName;
	j["userId"] = userId;
	j["sortOrder"] = sortOrder;
}

bool UserGroup::from_json(const QJsonObject& j)
{
	userGroupId = j["userGroupId"].toInt();
	groupName = j["groupName"].toString();
	userId = j["userId"].toInt();
	sortOrder = j["sortOrder"].toInt();
	return true;
}