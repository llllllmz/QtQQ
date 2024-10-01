#pragma once
#include <QString>
#include <qobject.h>
#include <QJsonObject>
#include <QJsonDocument>

class UserGroup : public QObject
{
	Q_OBJECT
public:
	UserGroup(QObject *parent = nullptr);
	~UserGroup();

	void setUserGroupId(int t_userGroupId);
	int getUserGroupId() const;

	void setGroupName(const QString& t_groupName);
	QString getGroupName() const;

	void setUserId(int t_userId);
	int getUserId() const;

	void setSortOrder(int t_sortOrder);
	int getSortOrder() const;

	// ת���� JSON
	void to_json(QJsonObject& j) const;

	// �� JSON �н���
	bool from_json(const QJsonObject& j);

private:
	int userGroupId;
	QString groupName;
	int userId;
	int sortOrder;
};