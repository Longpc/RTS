#include "ListUserPHPAPI.h"

ListUserAPI* ListUserAPI::s_ListUserAPI = nullptr;

ListUserAPI * ListUserAPI::getInstance()
{
	if (!s_ListUserAPI) {
		s_ListUserAPI = new (std::nothrow) ListUserAPI();
		s_ListUserAPI->init();
	}
	return s_ListUserAPI;
}
bool ListUserAPI::init()
{
	_listUser = {};
	char data[100];
	sprintf(data, "app_key=%s&info=%s", APP_KEY, "2354232342KGJSD%'#$");
	HttpClientBase::getInstance()->postAPIWithMethodNameAndParam("debug/list_user.php", data);
	HttpClientBase::getInstance()->setAPICallback([&](string a) {
		rapidjson::Document doc;
		doc.Parse<0>(a.c_str());
		if (doc.HasParseError())
		{
			log("error in parse json");
		}
		if (doc.IsObject() && doc.HasMember("data")) {
			if (strcmp(doc["event_name"].GetString(), "list_user") == 0) {
				log("List user Callback data: %s", a.c_str());
				for (int i = 0; i < doc["data"].Size(); i++)
				{
					UserInfo temp;
					temp._id = DataUtils::stringToFloat(doc["data"][i]["user_id"].GetString());
					temp._name = doc["data"][i]["name"].GetString();
					_listUser.push_back(temp);
					_isResponsed = true;
					if (_callBack)
					{
						_callBack(_listUser);
					}		
				}
			}
		}
		
	});
	return true;
}

void ListUserAPI::destroyInstance()
{
	CC_SAFE_RELEASE_NULL(s_ListUserAPI);
}

vector<UserInfo> ListUserAPI::getListUserData()
{
	if (_listUser.size() > 0) {
		return _listUser;
	}
	return {};
}

bool ListUserAPI::setLoadDataCompledCallback(ListUserCallback callB)
{
	if (callB != nullptr) {
		_callBack = callB;
	}
	return _isResponsed;
}


