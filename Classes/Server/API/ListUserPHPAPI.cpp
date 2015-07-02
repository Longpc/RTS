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
	HttpClientBase::getInstance()->setAPICallback([&](HttpClient *cl, HttpResponse* response) {
		std::vector<char>* data = response->getResponseData();
		std::string result(data->begin(), data->end());
		rapidjson::Document doc;
		doc.Parse<0>(result.c_str());
		if (doc.HasParseError())
		{
			log("error in parse json");
		}
		if (doc.IsObject() && doc.HasMember("data")) {
			if (strcmp(doc["event_name"].GetString(), "list_user") == 0) {
				log("List user Callback data: %s", result.c_str());
				for (int i = 0; i < doc["data"].Size(); i++)
				{
					RoomUser temp;
					temp.user_id = DataUtils::stringToFloat(doc["data"][i]["user_id"].GetString());
					temp.name = doc["data"][i]["name"].GetString();
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

vector<RoomUser> ListUserAPI::getListUserData()
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

std::string ListUserAPI::getUserNameByUserId(int user_id)
{
	auto listUser = getListUserData();
	for (auto &temp : listUser)
	{
		if (temp.user_id == user_id) {
			return temp.name.c_str();
		}
	}
	return "no name";
}


