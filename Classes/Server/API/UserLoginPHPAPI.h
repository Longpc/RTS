#ifndef USER_LOGIN_API_H__
#define USER_LOGIN_API_H__
#include "HttpClientBase.h"
#include "base/database/DataUtils.h"
#include "Model/data/UserModel.h"

typedef std::function<void()> LogInCallback;
class UserLoginAPI :public Ref
{
public:
	static UserLoginAPI *getInstance();
	static void destroyInstance();

	bool init();
	bool setLoginCompletedCallback(LogInCallback callB);

private:

	static UserLoginAPI *s_UserLoginAPI;
	bool _isResponsed = false;
	LogInCallback _callBack;
};

#endif