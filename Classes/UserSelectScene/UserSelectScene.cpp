#pragma execution_character_set("utf-8")
#include "UserSelectScene.h"

/*Scene * UserSelect::createScene()
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
*/
bool UserSelect::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	/*For test*/
	bool flg = ListUserAPI::getInstance()->setLoadDataCompledCallback([&](vector<RoomUser> callbackData) {
		_userList = callbackData;
		createContent();
	});
	if (flg) {
		_userList = ListUserAPI::getInstance()->getListUserData();
		createContent();
	}
	return true;
}

void UserSelect::onBackButtonClick(Ref *pSender)
{
	if (NodeServer::getInstance())
	{
		NodeServer::destroyInstance();
		RoomUserModel::destroyInstance();
	}
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
		backGround->setTitleText(_userList[i].name.c_str());
		backGround->addTouchEventListener(CC_CALLBACK_2(UserSelect::userSelectCallback, this));
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

void UserSelect::userSelectCallback(Ref *pSender, Widget::TouchEventType type)
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
		UserModel::getInstance()->setUserInfo(_userList[bt->getTag()]);
		UserModel::getInstance()->setRoomId(1);
		//UserLoginAPI::getInstance()->setLoginCompletedCallback([&](){
			Document doc;
			doc.SetObject();
			Document::AllocatorType& allo = doc.GetAllocator();
			auto uif = UserModel::getInstance()->getUserInfo();
			doc.AddMember("user_id", uif.user_id,allo);
			doc.AddMember("room_id", uif.room_id, allo);
			string uu = UserModel::getInstance()->getUuId().c_str();
			doc.AddMember("uuid",uu.c_str() , allo);
			StringBuffer buff;
			Writer<StringBuffer> wt(buff);
			doc.Accept(wt);

			//log("emit data: %s", buff.GetString());
			auto client = NodeServer::getInstance()->getClient();
			client->emit("connect_begin", buff.GetString());
			int roomId = uif.room_id;
			client->on("connect_begin_end", [&, roomId ](SIOClient* client, const std::string& data) {
				log("connect begin end data: %s", data.c_str());
				RoomUserModel::getInstance()->initDataWhenJoinRoom(data);
				Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene(UserModel::getInstance()->getUserInfo().user_id)));
			});

		//});
// 		StartAPI::getInstance()->setStartAPICallback([&,bt]() {
// 		});

		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
