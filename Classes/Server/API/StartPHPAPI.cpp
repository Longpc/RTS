#include "StartPHPAPI.h"

StartAPI* StartAPI::s_StartAPI = nullptr;
StartAPI * StartAPI::getInstance()
{
	if (!s_StartAPI) {
		s_StartAPI = new (std::nothrow) StartAPI();
		s_StartAPI->init();
	}
	return s_StartAPI;
}

void StartAPI::destroyInstance()
{
	CC_SAFE_DELETE(s_StartAPI);
}

bool StartAPI::init()
{

		char data[100];
		sprintf(data, "app_key=%s&user_id=%d", APP_KEY, UserModel::getInstance()->getUserInfo()._id);
		HttpClientBase::getInstance()->postAPIWithMethodNameAndParam("start.php", data, [&](string a) {
			log("CALL BACK data: %s",a.c_str());
			rapidjson::Document doc;
			doc.Parse<0>(a.c_str());
			if (doc.HasParseError())
			{
				log("error in parse json");

			}
			if (doc.IsObject() && doc.HasMember("data")) {
				if (strcmp(doc["event_name"].GetString(), "start") == 0) {
					UserUnit::getInstance()->createUserUnitDataFromJson(doc["data"]["unit_list"]);
					UserSkill::getInstance()->createUserSkillDataFromJson(doc["data"]["skill_list"]);
					_isResponsed = true;
					if (_callBack != nullptr)
					{
						_callBack();
					}

				}
			}

		});


	
	return true;
}

void StartAPI::setStartAPICallback(StartAPICallback callback)
{
	if (callback != nullptr) {
		_callBack = callback;
	}
}

bool StartAPI::getServerCallbackFlg()
{
	return _isResponsed;
}
