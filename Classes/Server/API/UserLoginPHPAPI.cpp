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
	sprintf(data, "app_key=%s&user_id=%d", APP_KEY, UserModel::getInstance()->getUserInfo().user_id);
	HttpClientBase::getInstance()->postAPIWithMethodNameAndParam("debug/select_test_user.php", data, [&](HttpClient *cl, HttpResponse* response) {		
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
