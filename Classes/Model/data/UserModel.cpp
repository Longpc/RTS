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

UserInfo UserModel::getUserInfo()
{
	return _userInfo;
}

void UserModel::setUserInfo(UserInfo ui)
{
	_userInfo = ui;
}

