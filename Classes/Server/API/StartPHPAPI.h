#ifndef START_PHP_API_H__
#define START_PHP_API_H__
#include "HttpClientBase.h"
#include "base/database/DataUtils.h"

#include "Model/data/UserModel.h"
#include "Model/data/UserUnitModel.h"
#include "Model/data/UserSkillModel.h"

#include <string>

typedef std::function<void()> StartAPICallback;
class StartAPI :public Ref
{
public:
	static StartAPI *getInstance();
	static void destroyInstance();

	bool init();
	void setStartAPICallback(StartAPICallback callB);
	bool getServerCallbackFlg();

private:

	static StartAPI *s_StartAPI;
	bool _isResponsed = false;
	StartAPICallback _callBack;
};

#endif