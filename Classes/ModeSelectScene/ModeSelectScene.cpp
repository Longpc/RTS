#pragma execution_character_set("utf-8")
#include "ModeSelectScene.h"

const static int PLAYER_TEXT_X = 900;
const static int OTHER_TEXT_X = 50;
const static int TEXT_H = 60;

using namespace cocos2d::network;
Scene * ModeSelectScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ModeSelectScene::create();
	
	scene->addChild(layer);

	return scene;
}

bool ModeSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	auto config = Configuration::getInstance();
	if (config->getValue("ServerAddress").isNull())
	{
		config->loadConfigFile("configs/config-test-ok.plist");
	}
	if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		downloadDatabase();
	}

	_item1->setEnabled(false);
	_usernameBg->setVisible(false);

	if (_defaultLabel != nullptr) {
		_defaultLabel->setString("モードを選択して下さい");
	}

	Sprite *unit = Sprite::create("image/unit/navi.png");
	unit->setPosition(Vec2(_visibleSize.width* 1/4, _visibleSize.height/2 - 20));
	addChild(unit);

	Button *multiButton = Button::create();
	multiButton->loadTextureNormal("image/button/new/multi.png");
	multiButton->setTouchEnabled(true);
	multiButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::multiButtonCallback, this));
	multiButton->setPosition(Vec2(_visibleSize.width * 3 / 4, _visibleSize.height *2/ 3));
	addChild(multiButton);

	Button *soloButton = Button::create();
	soloButton->loadTextureNormal("image/button/new/solo.png");
	soloButton->setTouchEnabled(true);
	soloButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::soloButtonCallback, this));
	soloButton->setPosition(Vec2(_visibleSize.width * 3 / 4, _visibleSize.height *1/ 3));
	addChild(soloButton);

// 	auto nextButton = Button::create();
// 	nextButton->loadTextureNormal("CloseNormal.png");
// 	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
// 	nextButton->setTouchEnabled(true);
// 	nextButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::testDialog, this));
// 	addChild(nextButton, 10);

	editBox = TextField::create("please input text", "Meiryo", 40);
	editBox->setContentSize(Size(_visibleSize.width, 50));
	editBox->setPosition(Point(_visibleSize.width / 2, 100));
	editBox->addEventListener(CC_CALLBACK_2(ModeSelectScene::textFieldEvent, this));
	this->addChild(editBox,1000);

	// ここでsocket.io connection開始。clientを持っておく
// 	log("----> connect");
 	//_client = SocketIO::connect("ws://192.168.0.126:8080/", *this);
// 	//_client = SocketIO::connect("ws://localhost:8080/", *this);
// 	//CCLOG("----> on hello");
 	//_client->on("hello", CC_CALLBACK_2(ModeSelectScene::onReceiveEvent, this));

	auto sv = TestServer::getInstance();
	sv->startConnectWithHandler("hello", CC_CALLBACK_2(ModeSelectScene::onReceiveEvent, this));
	return true;
}



void ModeSelectScene::multiButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onDisableTouch) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkRoomMember() == true) {
			Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, UserSelect::createScene()));
		}
		else {
			//WARNING LAYER
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void ModeSelectScene::soloButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onDisableTouch) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1,SOLO_MODE)));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
bool ModeSelectScene::checkRoomMember()
{
	return _onAccess;
}

// void ModeSelectScene::testDialog(Ref *pSender, Widget::TouchEventType type)
// {
// 	switch (type)
// 	{
// 	case cocos2d::ui::Widget::TouchEventType::BEGAN:
// 		break;
// 	case cocos2d::ui::Widget::TouchEventType::MOVED:
// 		break;
// 	case cocos2d::ui::Widget::TouchEventType::ENDED:
// 	{
// 		if (_onAccess) {
// 			_onAccess = false;
// 		}
// 		else {
// 			_onAccess = true;
// 		}
// 		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, BatleScene::createScene(2)));
// 		break;
// 	}
// 	case cocos2d::ui::Widget::TouchEventType::CANCELED:
// 		break;
// 	default:
// 		break;
// 	}
// }

void ModeSelectScene::downloadDatabase()
{
	auto request = new HttpRequest();
	string svAddredd = Configuration::getInstance()->getValue("ServerAddress").asString();
	log("%s", svAddredd.c_str());
	request->setUrl(/*"http://tore.f-2.jp/mokha/server/download/innolab/database/database.db3"*/svAddredd.c_str());
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(this, httpresponse_selector(ModeSelectScene::serverCallback));
	auto client = HttpClient::getInstance();
	client->enableCookies(NULL);
	client->send(request);
	request->release();
}

void ModeSelectScene::serverCallback(HttpClient* client, HttpResponse* response)
{
	if (!response)
	{
		log("No Response from server");
		return;
	}
	if (response->getResponseCode() == 200)
	{
		auto fileUtils = FileUtils::getInstance();
		std::string filepath = fileUtils->getWritablePath() + "database.db3";
		std::vector<char> *resData = response->getResponseData();
		FILE *fp = fopen(filepath.c_str(), "w");
		if (!fp) {
			log("can not create file %s", filepath.c_str());
			return;
		}
		fwrite(resData->data(), 1, resData->size(), fp);
		fclose(fp);
		log("save file into %s", filepath.c_str());
	}
	else {
		
		if (_retryCount <= Configuration::getInstance()->getValue("retry").asInt())
		{
			log("Fail to conect to server %d time", _retryCount);
			_retryCount++;
			log("Retry....");
			downloadDatabase();
		}
		else {
			log("CONNECT TO SERVER FAILED. DATABASE  UPDATE FAILED");
		}
	}
}

/**
* serverからのemit("hello")をここでlisten
*/
void ModeSelectScene::onReceiveEvent(SIOClient* client, const std::string& data){

	CCLOG("---->onReceiveEvent");
	log("%s", data.c_str());
	rapidjson::Document doc;
	doc.Parse<rapidjson::kParseDefaultFlags>(data.c_str());
	rapidjson::Value &val = doc["args"];
	std::string value = val[rapidjson::SizeType(0)]["value"].GetString();
	addTalkOther(value);
};

//------------------------------------------------------
// ここからUI周り

/**
* textFieldの処理
*/
void ModeSelectScene::textFieldEvent(Ref *pSender, TextField::EventType type)
{
	TextField* text;
	std::string sendText;
	switch (type)
	{
		// IMEが閉じた時 
	case TextField::EventType::DETACH_WITH_IME:
		text = (TextField*)pSender;

		sendText = "[{\"value\":\"" + text->getStringValue() + "\"}]";
		//_client->emit("hello", sendText);
		TestServer::getInstance()->sendMessageWithName("hello", sendText);
		addTalkPlayer(text->getString());
		break;
	default:
		break;
	}
}

/**
* プレイヤーUI
*/
void ModeSelectScene::addTalkPlayer(const std::string& str){
	Size size = Director::getInstance()->getVisibleSize();

	DrawNode* draw = DrawNode::create();

	int originalX = PLAYER_TEXT_X;
	int originalY = size.height - (TEXT_H * (index + 1));

	int x = originalX - 290;
	int y = originalY - 60;
	int w = 300;
	int h = 60;

	Vec2 points[] = {
		Vec2(x, y),
		Vec2(x + w, y),
		Vec2(x + w, y + h),
		Vec2(x, y + h),
	};

	this->addChild(draw);
	draw->drawPolygon(points, 4, Color4F(0, 0.5, 0, 1), 1, Color4F(0, 0, 1, 1));

	auto text = Text::create(str, "", 40);
	text->setTextHorizontalAlignment(TextHAlignment::RIGHT);
	text->setAnchorPoint(Point(1.0, 1.0));
	text->setPosition(Point(originalX, originalY));

	this->addChild(text,999);
	index++;
}

/**
* その他UI
*/
void ModeSelectScene::addTalkOther(const std::string& str){
	Size size = Director::getInstance()->getVisibleSize();

	DrawNode* draw = DrawNode::create();

	int originalX = OTHER_TEXT_X;
	int originalY = size.height - (TEXT_H * (index + 1));

	int x = originalX - 10;
	int y = originalY - 60;
	int w = 300;
	int h = 60;

	Vec2 points[] = {
		Vec2(x, y),
		Vec2(x + w, y),
		Vec2(x + w, y + h),
		Vec2(x, y + h),
	};

	this->addChild(draw);
	draw->drawPolygon(points, 4, Color4F(0.5, 0, 0, 1), 1, Color4F(1, 0, 0, 1));

	auto text = Text::create(str, "Meiryo", 40);
	text->setTextHorizontalAlignment(TextHAlignment::LEFT);
	text->setAnchorPoint(Point(0.0, 1.0));
	text->setPosition(Point(originalX, originalY));
	text->setColor(Color3B(255, 255, 0));
	this->addChild(text,1000);
	index++;
}
