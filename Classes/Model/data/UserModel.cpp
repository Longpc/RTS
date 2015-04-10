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


