#include "UserModel.h"

UserModel* UserModel::s_UserModel = nullptr;

UserModel * UserModel::getInstance()
{
	if (!s_UserModel)
	{
		s_UserModel = new (std::nothrow) UserModel();
	}
	return s_UserModel;
}

void UserModel::setTeamId(int teamid)
{
	auto a = getUserInfo();
	a.team_id = teamid;
	setUserInfo(a);
}

void UserModel::setRoomId(int roomId)
{
	auto a = getUserInfo();
	a.room_id = roomId;
	setUserInfo(a);
}


