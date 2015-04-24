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
	bool flg = ListUserAPI::getInstance()->setLoadDataCompledCallback([&](vector<UserInfo> callbackData) {
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
		UserModel::getInstance()->setUserInfo(_userList[bt->getTag()]);
		UserModel::getInstance()->setRoomId(random(1, 5));
		UserLoginAPI::getInstance()->setLoginCompletedCallback([&](){
			log("Login Completed");
			int roomId = UserModel::getInstance()->getRoomId();

			Document doc;
			doc.SetObject();
			Document::AllocatorType& allo = doc.GetAllocator();

			doc.AddMember("user_id", UserModel::getInstance()->getUserInfo()._id,allo);
			doc.AddMember("room_id", roomId, allo);
			doc.AddMember("uuid", UserModel::getInstance()->getUuId().c_str(), allo);

			StringBuffer buff;
			Writer<StringBuffer> wt(buff);
			doc.Accept(wt);


			auto client = NodeServer::getInstance()->getClient();
			log("buff string: %s", buff.GetString());
			client->emit("connect_begin", buff.GetString());
			client->on("connect_begin_end", [&, roomId](SIOClient* client, const std::string& data) {
				log("connect end data: %s", data.c_str());
				RoomModel::getInstance();
				/*UserModel::getInstance()->setRoomId(roomId);*/
				//add self info to room user list
				RoomUser temp;
				temp._uuid = UserModel::getInstance()->getUuId();
				temp.room_id = roomId;
				temp.user_id = UserModel::getInstance()->getUserInfo()._id;
				temp.team_id = 0;
				temp._ready = 0;
				temp.state = 0;
				RoomModel::getInstance()->addUserInfoToUserList(temp);
			});

		});
// 		StartAPI::getInstance()->setStartAPICallback([&,bt]() {
// 		});
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene(_userList[bt->getTag()]._id)));

		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
