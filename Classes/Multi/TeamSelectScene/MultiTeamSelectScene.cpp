#pragma execution_character_set("utf-8")
#include "MultiTeamSelectScene.h"

//FOR TEST
#include "Server/API/BattleAPI.h"

Scene * MultiTeamSelectScene::createScene(int userId)
{
	auto scene = Scene::create();
	auto layer = MultiTeamSelectScene::create();

	scene->addChild(layer);

	return scene;
}

bool MultiTeamSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	RoomUser a = UserModel::getInstance()->getUserInfo();
	if (_defaultLabel != nullptr) {
		_defaultLabel->setString("チームを選択してください");
	}
	//getUserInfor
	_userNameLabel->setString(a.name.c_str());
	char data1[100];
	sprintf(data1, "app_key=%s&user_id=%d", APP_KEY, UserModel::getInstance()->getUserInfo().user_id);
	//HttpClientBase::getInstance()->postAPIAddressAndParam("start.php", data1, [&](string a) {
	//	log("Start--->Callback data: %s", a.c_str());
		// 		rapidjson::Document doc;
		// 		doc.Parse<0>(a.c_str());
		// 		if (doc.HasParseError())
		// 		{
		// 			log("error in parse json");
		// 		}
		// 		if (doc.IsObject() && doc.HasMember("data")) {
		// 			for (int i = 0; i < doc["data"].Size(); i++)
		// 			{
		// 				UserInfo temp;
		// 				temp._id = DataUtils::stringToFloat(doc["data"][i]["user_id"].GetString());
		// 				temp._name = doc["data"][i]["name"].GetString();
		// 				_listUser.push_back(temp);
		// 			}
		// 		}
		/*if (_callBack != nullptr)
		{
		_callBack();
		}*/
	//});
	_blueTeamBg = Sprite::create("image/screen/base.png");
	_blueTeamBg->setPosition(Vec2(_visibleSize.width / 2,10+(_visibleSize.height - 100) * 1 / 4));
	addChild(_blueTeamBg, 1);
	_redTeamBg = Sprite::create("image/screen/base.png");
	_redTeamBg->setPosition(Vec2(_visibleSize.width / 2, 10+(_visibleSize.height - 100) * 3 / 4));
	addChild(_redTeamBg, 1);

	Button *blueButton = Button::create();
	blueButton->loadTextureNormal("image/button/new/blue.png");
	blueButton->setTouchEnabled(true);
	blueButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::blueTeamButtonCallback, this));
	blueButton->setPosition(Vec2(_visibleSize.width *3 / 4, _blueTeamBg->getPositionY()-blueButton->getContentSize().height/2));
	addChild(blueButton,2);

	Button *redButton = Button::create();
	redButton->loadTextureNormal("image/button/new/red.png");
	redButton->setTouchEnabled(true);
	redButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::redTeamButtonCallback, this));
	redButton->setPosition(Vec2(_visibleSize.width * 3 / 4, _redTeamBg->getPositionY() - redButton->getContentSize().height / 2));
	addChild(redButton,2);

	/*Debug dialog button
	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::testFullTeam, this));
	addChild(nextButton, 10);*/

	getRoomInfo(_curRoomId);
	getAndShowTeamInfo(TEAM_FLG_BLUE, _blueTeamBg);
	getAndShowTeamInfo(TEAM_FLG_RED, _redTeamBg);



	scheduleUpdate();
	return true;
}


void MultiTeamSelectScene::redTeamButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkTeamFull(TEAM_FLG_RED) == true) {
			enterTeam(TEAM_FLG_RED);
		}
		else {
			//SHOW DIALOG
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog, 100);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiTeamSelectScene::blueTeamButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkTeamFull(TEAM_FLG_BLUE) == true) {
			enterTeam(TEAM_FLG_BLUE);
		}
		else {
			//SHOW DIALOg
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog, 100);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiTeamSelectScene::getRoomInfo(int roomId)
{
	//get data from server
	//create sample data
	/*
	Old version had scrollview to show list user in current room
	for (int i = 0; i < 6;i++)
	{
		std::stringstream name;
		name << "User " << i;

		UserInfo temp;
		temp._name = name.str().c_str();
		temp._onTeam = i % 2;
		_roomUserInfo.push_back(temp);
	}

	auto roomLabel = LabelTTF::create("Room members", "fonts/Marker Felt.ttf", 30);
	roomLabel->setPosition(Vec2(roomLabel->getContentSize().width/2, 350));
	addChild(roomLabel);
	auto scroll = extension::ScrollView::create();
	scroll->setViewSize(Size(roomLabel->getContentSize().width, 300));
	scroll->setPosition(Vec2::ZERO);
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
	scroll->updateInset();
	scroll->setVisible(true);

	addChild(scroll);
	auto layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	layer->setContentSize(Size(roomLabel->getContentSize().width, _roomUserInfo.size()+50+100));
	layer->setPosition(Vec2(0, -layer->getContentSize().height / 2));
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	for (int i = 0; i < _roomUserInfo.size(); i++)
	{
		auto userName = LabelTTF::create(_roomUserInfo[i]._name, "fonts/Marker Felt.ttf", 25);
		userName->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		if (_roomUserInfo[i]._onTeam == 0) {
			userName->setOpacity(70);
		}
		userName->setPosition(Vec2(0, layer->getContentSize().height- 50 * i+1));
		layer->addChild(userName);
	}
	*/
	auto roomUserList = RoomModel::getInstance()->getRoomUserList();
	vector<RoomUser> tempList1;
	vector<RoomUser> tempList2;
	for (int i = 0; i < roomUserList.size(); i++)
	{
		if (roomUserList[i].team_id == TEAM_FLG_BLUE)
		{
			roomUserList[i].name = ListUserAPI::getInstance()->getUserNameByUserId(roomUserList[i].user_id);
			tempList1.push_back(roomUserList[i]);
			continue;
		}
		if (roomUserList[i].team_id == TEAM_FLG_RED) {
			roomUserList[i].name = ListUserAPI::getInstance()->getUserNameByUserId(roomUserList[i].user_id);
			tempList2.push_back(roomUserList[i]);
			continue;
		}
	}
	RoomModel::getInstance()->setBlueTeamUserList(tempList1);
	RoomModel::getInstance()->setRedTeamUserList(tempList2);

}
void MultiTeamSelectScene::getAndShowTeamInfo(int teamId,Sprite *parent)
{
	parent->removeAllChildren();
	//get data form server

	vector<RoomUser> teamInfo = {};
	switch (teamId)
	{
	case TEAM_FLG_BLUE:
		teamInfo = RoomModel::getInstance()->getBlueTeamUserList();
		break;
	case TEAM_FLG_RED:
		teamInfo = RoomModel::getInstance()->getRedTeamUserList();
		break;
	}
	std::stringstream memNum;
	memNum << "メンバー数: "<< teamInfo.size();
	auto teamLabel = Label::createWithSystemFont(memNum.str().c_str(), JAPANESE_FONT_1_BOLD, 30);
	teamLabel->setPosition(Vec2(50, parent->getContentSize().height - 20));
	teamLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	teamLabel->setColor(Color3B::BLACK);
	teamLabel->setTag(REMOVE_CHILD);
	parent->addChild(teamLabel);

	float baseX = parent->getContentSize().width*1/3 - 45;
	float baseY = parent->getContentSize().height * 2 / 3 - 10;
	
	int maxSize = 3;
	int listSize = (maxSize < teamInfo.size()) ? maxSize : teamInfo.size();
	vector<Sprite*> listback;
	for (int i = 0; i < maxSize; i++)
	{
		listback.push_back(createUsernameBackground(Vec2(baseX, baseY *(maxSize - i) / maxSize - (i * 5))));
		listback.back()->setTag(REMOVE_CHILD);
		parent->addChild(listback.back());
	}

	for (int i = 0; i < listSize; i++)
	{
		listback[i]->addChild(createLabelWithStringandPosition(teamInfo[i].name, Vec2(10, listback[i]->getContentSize().height / 2)));
	}

	/*auto scroll = extension::ScrollView::create();
	scroll->setViewSize(Size(teamLabel->getContentSize().width, 150));
	scroll->setPosition(Vec2(teamLabel->getPositionX(),50));
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
	scroll->updateInset();
	scroll->setVisible(true);

	addChild(scroll);
	auto layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	layer->setContentSize(Size(teamLabel->getContentSize().width, 150));
	layer->setPosition(Vec2(teamLabel->getPositionX(), -layer->getContentSize().height / 2));
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	for (int i = 0; i < _blueTeamUserInfo.size(); i++)
	{
		auto userName = LabelTTF::create(_blueTeamUserInfo[i]._name, "fonts/Marker Felt.ttf", 25);
		userName->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		userName->setPosition(Vec2(0, layer->getContentSize().height - 50 * i + 1));
		layer->addChild(userName);
	}
	*/
}

bool MultiTeamSelectScene::checkTeamFull(int teamId)
{
	return _onAccess;
}

void MultiTeamSelectScene::enterTeam(int teamId)
{
	//send event to NodeServer
	Document doc;
	doc.SetObject();
	auto a = UserModel::getInstance()->getUserInfo();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("user_id", a.user_id, allo);
	doc.AddMember("room_id", a.room_id, allo);
	doc.AddMember("team_id", teamId, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid",uu.c_str() , allo);
	//set team
	RoomModel::getInstance()->setTeamForUserByUserId(a.room_id, a.user_id, teamId);


	StringBuffer buff;
	Writer<StringBuffer> wt(buff);
	doc.Accept(wt);

	auto client = NodeServer::getInstance()->getClient();
	client->emit("connect_select_team", buff.GetString());
	client->on("connect_select_team_end", [&,teamId](SIOClient* client, const std::string& data) {
		log("select team end data: %s", data.c_str());
		UserModel::getInstance()->setTeamId(teamId);
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(teamId, MULTI_MODE)));
	});

}
void MultiTeamSelectScene::onBackButtonClick(Ref *pSender)
{
	auto a = UserModel::getInstance()->getUserInfo();
	string uu = UserModel::getInstance()->getUuId().c_str();
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", a.user_id, allo);
	doc.AddMember("room_id", a.room_id, allo);




	Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, UserSelect::createScene()));
}

void MultiTeamSelectScene::testFullTeam(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_onAccess) {
			_onAccess = false;
		}
		else {
			_onAccess = true;
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

Sprite* MultiTeamSelectScene::createUsernameBackground(Vec2 position)
{
	auto sp = Sprite::create("image/navigator/user_name.png");
	sp->setPosition(position);
	return sp;
}

Label* MultiTeamSelectScene::createLabelWithStringandPosition(string text, Vec2 pos)
{
	auto lb = Label::createWithSystemFont(text.c_str(), JAPANESE_FONT_1_BOLD, 25);
	lb->setHorizontalAlignment(TextHAlignment::LEFT);
	lb->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	lb->setColor(Color3B::BLACK);
	lb->setPosition(pos);
	return lb;
}

void MultiTeamSelectScene::update(float delta)
{
	//update team user list
	_tempCount += delta;
	//log("Delta Time: %f", _tempCount);
	if (_tempCount > 1)
	{
		getRoomInfo(_curRoomId);
		getAndShowTeamInfo(TEAM_FLG_BLUE, _blueTeamBg);
		getAndShowTeamInfo(TEAM_FLG_RED, _redTeamBg);
		_tempCount = 0;
	}
}



