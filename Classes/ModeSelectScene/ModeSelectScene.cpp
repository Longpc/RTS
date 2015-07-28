#pragma execution_character_set("utf-8")
#include "ModeSelectScene.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

const static int PLAYER_TEXT_X = 900;
const static int OTHER_TEXT_X = 50;
const static int TEXT_H = 60;

using namespace cocos2d::network;
using namespace rapidjson;
/*Scene * ModeSelectScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ModeSelectScene::create();
	
	scene->addChild(layer);

	return scene;
}
*/
bool ModeSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	/*auto console = Director::getInstance()->getConsole();
	console->listenOnTCP(1234);*/
	//_loadingSp = LoadingSprite::getInstance();
	//addChild(_loadingSp);
	//_loadingSp->setPosition(Vec2::ZERO);


	auto config = Configuration::getInstance();
	if (config->getValue("ServerAddress").isNull())
	{
		config->loadConfigFile("configs/config-test-ok.plist");
	}
	/*if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	{
		downloadDatabase();
	}*/

	HttpClientBase::getInstance()->postAPIWithMethodNameAndParam("uuid.php", "uuid", [&](HttpClient *cl, HttpResponse* response){
		if (response->getResponseCode() != 200) {
			log("HttpClientBase------>connect failed");
			HttpClientBase::destroyInstance();
			//LoadingSprite::getInstance()->showLoadingFail();
			return;

		}
		std::vector<char>* data = response->getResponseData();
		std::string result(data->begin(), data->end());

		log("UUID: %s", result.c_str());
		UserModel::getInstance()->setUuId(result.c_str());
		auto ifo = UserModel::getInstance()->getUserInfo();
		ifo._uuid = result.c_str();
		UserModel::getInstance()->setUserInfo(ifo);

		StartAPI::getInstance()->setStartAPICallback([&]() {
			log("Start callback");
			//LoadingSprite::getInstance()->destroyInstance();
		});
	});

	
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

	/*auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener([&](Ref *p, ui::Widget::TouchEventType type){
		switch (type)
		{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
			break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case cocos2d::ui::Widget::TouchEventType::ENDED:
		{
			NodeServer::getInstance();
			auto bt = (Button*)p;
			bt->runAction(RepeatForever::create(Sequence::create(CallFuncN::create([&](Ref* p){
				NodeServer::getInstance()->getClient()->emit("test", "this is test data");
				
			}), DelayTime::create(0.01), nullptr)));

			break;
		}
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
			break;
		default:
			break;
		}
	});
	addChild(nextButton, 10);*/



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
		if (checkRoomMember() == true && StartAPI::getInstance()->getServerCallbackFlg()) {
			NodeServer::createInstance();
			Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, UserSelect::createScene()));
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
		if (StartAPI::getInstance()->getServerCallbackFlg() == true)
		{
			Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1, SOLO_MODE)));
		}
		else {
			log("Waiting for StartAPI callback");
		}
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

/*void ModeSelectScene::downloadDatabase()
{
	auto request = new HttpRequest();
	string svAddredd = Configuration::getInstance()->getValue("ServerAddress").asString();
	log("%s", svAddredd.c_str());*/
	//request->setUrl(/*"http://tore.f-2.jp/mokha/server/download/innolab/database/database.db3"*/svAddredd.c_str());
	/*request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(this, httpresponse_selector(ModeSelectScene::serverCallback));
	auto client = HttpClient::getInstance();
	client->enableCookies(NULL);
	client->send(request);
	request->release();
}*/

/*void ModeSelectScene::serverCallback(HttpClient* client, HttpResponse* response)
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
}*/

