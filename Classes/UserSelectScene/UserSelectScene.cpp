#pragma execution_character_set("utf-8")
#include "UserSelectScene.h"

Scene * UserSelect::createScene()
{
	auto scene = Scene::create();
	auto layer = UserSelect::create();

	scene->addChild(layer);

	return scene;
}

UserSelect* UserSelect::create()
{
	auto l = new UserSelect();
	if (l && l->init()) {
		l->autorelease();
		return l;
	}
	CC_SAFE_DELETE(l);
	return nullptr;
}

bool UserSelect::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	getUserListFromServer();
	
	return true;
}

void UserSelect::onBackButtonClick(Ref *pSender)
{
	Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, ModeSelectScene::createScene()));
}

void UserSelect::createContent()
{
	_defaultLabel->setString("ユーザ選択");
	for (int i = 0; i < _userList.size(); i++)
	{
		stringstream ss;
		ss << "テスト " << i + 1;
		auto backGround = Button::create("image/navigator/title_240x100.png");
		backGround->setTitleFontName(JAPANESE_FONT_1_HEAVY);
		backGround->setTitleColor(Color3B::BLACK);
		backGround->setTitleFontSize(30);
		backGround->setTitleText(_userList[i]._name.c_str());
		backGround->addTouchEventListener(CC_CALLBACK_2(UserSelect::unitSelectButtonClick, this));
		backGround->setTag(i);
		int xValue = i % 2;
		int yvalue = i*0.5;
		float baseX = _visibleSize.width / 4;
		float baseY = 250;
		float space_x = _visibleSize.width / 2;
		float space_y = 120;

		backGround->setPosition(Vec2(baseX + space_x* xValue, _visibleSize.height - baseY - space_y* yvalue));

		addChild(backGround);
// 		auto nameLabel = Label::createWithSystemFont("テスト", JAPANESE_FONT_1_BOLD, 30);
// 		nameLabel->setPosition(backGround->getPosition());
// 		addChild(nameLabel);
	}
}

void UserSelect::unitSelectButtonClick(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		auto bt = (Button*)pSender;
		postUserInfoToServer(_userList[bt->getTag()]._id);
		UserModel::getInstance()->setUserInfo(_userList[bt->getTag()]);
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene(_userList[bt->getTag()]._id)));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void UserSelect::getUserListFromServer()
{
	auto httpRequest = new HttpRequest();
	auto addressValue = Configuration::getInstance()->getValue("APIServer");
	string address = addressValue.asString().c_str();
	httpRequest->setUrl(address.append("debug/list_user.php").c_str());
	httpRequest->setRequestType(HttpRequest::Type::POST);
	httpRequest->setResponseCallback(CC_CALLBACK_2(UserSelect::serverAPICallback,this));
	httpRequest->setTag("login");
	char data[100];
	sprintf(data, "app_key=%s&info=%s", APP_KEY, "2354232342KGJSD%'#$");
	const char* buffer = data;
	httpRequest->setRequestData(buffer, strlen(buffer));

	auto client = HttpClient::getInstance();
	client->enableCookies(nullptr);
	client->send(httpRequest);
	httpRequest->release();
}

void UserSelect::serverAPICallback(HttpClient* client, HttpResponse* response)
{
	if (response->getResponseCode() != 200) {
		log("connect failed");
		return;
	}
	std::vector<char>* data = response->getResponseData();
	std::string result(data->begin(), data->end());
	cocos2d::log("Result JSON: %s", result.c_str());
	rapidjson::Document doc;
	doc.Parse<0>(result.c_str());
	if (doc.HasParseError())
	{
		log("error in parse json");
	}
	if (doc.IsObject() && doc.HasMember("data")) {		
		for (int i = 0; i < doc["data"].Size(); i++)
		{
			UserInfo temp;
			temp._id = DataUtils::stringToFloat(doc["data"][i]["user_id"].GetString());
			temp._name = doc["data"][i]["name"].GetString();
			_userList.push_back(temp);
		}
	}
	createContent();
}

void UserSelect::postUserInfoToServer(int user_id)
{
	auto httpRequest = new HttpRequest();
	auto addressValue = Configuration::getInstance()->getValue("APIServer");
	string address = addressValue.asString().c_str();
	httpRequest->setUrl(address.append("debug/select_test_user.php").c_str());
	httpRequest->setRequestType(HttpRequest::Type::POST);
	httpRequest->setResponseCallback(CC_CALLBACK_2(UserSelect::loginCallback, this));
	httpRequest->setTag("login");
	char data[100];
	sprintf(data, "app_key=%s&user_id=%d", APP_KEY, user_id);
	const char* buffer = data;
	httpRequest->setRequestData(buffer, strlen(buffer));

	auto client = HttpClient::getInstance();
	client->enableCookies(nullptr);
	client->send(httpRequest);
	httpRequest->release();
}

void UserSelect::loginCallback(HttpClient* client, HttpResponse* response)
{
	if (response->getResponseCode() != 200) {
		log("connect failed");
		return;
	}
	std::vector<char>* data = response->getResponseData();
	std::string result(data->begin(), data->end());
	cocos2d::log("Result JSON: %s", result.c_str());
}
