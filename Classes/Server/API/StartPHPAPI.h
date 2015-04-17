#ifndef START_PHP_API_H__
#define START_PHP_API_H__
#include "HttpClientBase.h"
#include "base/database/DataUtils.h"

#include "Model/data/UserModel.h"
#include "Model/data/UserUnit.h"
#include "Model/data/UserSkill.h"

#include <string>

typedef std::function<void()> StartAPICallback;
class StartAPI :public Ref
{
public:
	static StartAPI *getInstance();
	static void destroyInstance();

	bool init();
	void setStartAPICallback(StartAPICallback callB);

private:

	static StartAPI *s_StartAPI;
	bool _isResponsed = false;
	StartAPICallback _callBack;
};

#endif