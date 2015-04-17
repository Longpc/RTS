#include "UserLoginPHPAPI.h"

UserLoginAPI* UserLoginAPI::s_UserLoginAPI = nullptr;

UserLoginAPI * UserLoginAPI::getInstance()
{
	if (!s_UserLoginAPI) {
		s_UserLoginAPI = new (std::nothrow) UserLoginAPI();
		s_UserLoginAPI->init();
	}
	return s_UserLoginAPI;
}

void UserLoginAPI::destroyInstance()
{
	CC_SAFE_DELETE(s_UserLoginAPI);
}

bool UserLoginAPI::init()
{
	char data[100];
	sprintf(data, "app_key=%s&user_id=%d", APP_KEY, UserModel::getInstance()->getUserInfo()._id);
	HttpClientBase::getInstance()->postAPIAddressAndParam("debug/select_test_user.php", data, [&](string a) {
		log("LOGIN--->Callback data: %s", a.c_str());
		// 		rapidjson::Document doc;
		// 		doc.Parse<0>(a.c_str());
		// 		if (doc.HasParseError())
		// 		{
		// 			log("error in parse json");
		// 		}
		// 		if (doc.IsObject() && doc.HasMember("data")) {
		// 			for (int i = 0; i < doc["data"].Size(); i++)
		// 			{
		// 				UserInfo temp;
		// 				temp._id = DataUtils::stringToFloat(doc["data"][i]["user_id"].GetString());
		// 				temp._name = doc["data"][i]["name"].GetString();
		// 				_listUser.push_back(temp);
		// 			}
		// 		}
		if (_callBack)
		{
			_callBack();
		}
		_isResponsed = true;
		UserLoginAPI::destroyInstance();
	});

	return true;
}

bool UserLoginAPI::setLoginCompletedCallback(LogInCallback callB)
{
	if (callB) {
		_callBack = callB;
	}
	return _isResponsed;
}
