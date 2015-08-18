#pragma execution_character_set("utf-8")
#include "UnitSelectScene.h"

MultiUnitSelectScene::~MultiUnitSelectScene()
{
	NotificationCenter::getInstance()->removeAllObservers(this);
}

Scene * MultiUnitSelectScene::createScene(int roomId, int pageFlg)
{
	auto scene = Scene::create();
	auto layer = MultiUnitSelectScene::create(roomId,pageFlg);

	scene->addChild(layer);

	return scene;
}


MultiUnitSelectScene * MultiUnitSelectScene::create(int roomId, int pageFlg)
{
	MultiUnitSelectScene *layer = new MultiUnitSelectScene();
	if (layer->init(roomId,pageFlg))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

bool MultiUnitSelectScene::init(int roomId,int pageFlg)
{
	if (!LayerBase::init()) {
		return false;
	}
	getDataFromDataBase();
	_roomId = roomId;
	_pageFlg = pageFlg;
	_onTouchDisable = false;
	auto a = UserModel::getInstance()->getUserInfo();
	_userNameLabel->setString(a.name.c_str());
	_allSelectedSkilId.push_back(-1);
	_allSelectedSkilId.push_back(-1);

	UserDefault::getInstance()->setIntegerForKey("MODE", _pageFlg);

	_defaultLabel->setString("ユニットを選択して下さい");

	nextButton = Button::create();
	nextButton->loadTextureNormal("image/screen/battle_disable.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, _visibleSize.height-100));
	nextButton->setTouchEnabled(false);
	nextButton->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::sendReadyButtonCallback, this));
	addChild(nextButton, 10); 
	
	_pagebackGround = Sprite::create("image/screen/unitSelect/back.png");
	_pagebackGround->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 120));
	addChild(_pagebackGround);
	Size baseSize = _pagebackGround->getContentSize();

	lArrow = Button::create("image/screen/unitSelect/left.png");
	lArrow->setPosition(Vec2(50, baseSize.height / 2));
	_pagebackGround->addChild(lArrow);
	lArrow->setSwallowTouches(true);
	lArrow->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::arrowButonClick, this, -1));
	lArrow->setVisible(false);

	rArrow = Button::create("image/screen/unitSelect/right.png");
	rArrow->setPosition(Vec2(baseSize.width - 50, baseSize.height / 2));
	rArrow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	rArrow->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::arrowButonClick, this, 1));
	_pagebackGround->addChild(rArrow);
	rArrow->setSwallowTouches(true);

	
	for (int i = 0; i < 3; i ++) 
	{
		auto group = UnitInfoGroup::create();
		group->setPosition(Vec2(_visibleSize.width / 2 + (200 * i - 220), _visibleSize.height - 150));
		this->addChild(group, 100);
		_allUnitGroup.push_back(group);
	}
	if (_pageFlg == MULTI_MODE)
	{
		//handler for another player select information
		NotificationCenter::getInstance()->addObserver(this, CC_CALLFUNCO_SELECTOR(MultiUnitSelectScene::updateContent), TEAM_DATA_UPDATE_MSG, nullptr);
		//initial show for team select status when connect
		updateContent(nullptr);
		//handler for another player select status( ready or not)
		auto sv = NodeServer::getInstance()->getClient();
		sv->on("connect_ready", [&](SIOClient * c, const string& data) {
			CC_UNUSED_PARAM(c);
			log("public ready data: %s", data.c_str());
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Parse JSOn error");
				return;
			}
			if (doc.IsObject())
			{
				int team_id = doc["team_id"].GetInt();
				string uuid = doc["uuid"].GetString();
				if (team_id == UserModel::getInstance()->getUserInfo().team_id)
				{
					for (auto &group : _allUnitGroup)
					{
						if (strcmp(group->getName().c_str(), uuid.c_str()) == 0)
						{
							group->showReadyForBattle(true);
							break;
						}
					}
				}
				auto roomUserList = RoomUserModel::getInstance()->getRoomUserList();
				for (int i = 0; i < roomUserList.size(); i ++)
				{
					if (strcmp(roomUserList[i]._uuid.c_str(), uuid.c_str()) == 0)
					{
						roomUserList[i]._ready = 1;
						log("set ready");
						break;
					}
				}
				RoomUserModel::getInstance()->setRoomUserList(roomUserList);
				RoomUserModel::getInstance()->updateTeamUserList();
			}
		});
		sv->on("public_not_ready", [&](SIOClient *c, const string& data) {
			CC_UNUSED_PARAM(c);
			log("public un ready data: %s", data.c_str());
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Parse JSOn error");
				return;
			}
			if (doc.IsObject())
			{
				int team_id = doc["team_id"].GetInt();
				string uuid = doc["uuid"].GetString();
				if (team_id == UserModel::getInstance()->getUserInfo().team_id)
				{
					for (auto &group : _allUnitGroup)
					{
						if (strcmp(group->getName().c_str(), uuid.c_str()) == 0)
						{
							group->showReadyForBattle(false);
							//if current player
							break;
						}
					}
				}
				auto roomUserList = RoomUserModel::getInstance()->getRoomUserList();
				for (int i = 0; i < roomUserList.size(); i++)
				{
					if (strcmp(roomUserList[i]._uuid.c_str(), uuid.c_str()) == 0)
					{
						roomUserList[i]._ready = 0;
						log("unset ready");
						break;
					}
				}
				RoomUserModel::getInstance()->setRoomUserList(roomUserList);
				RoomUserModel::getInstance()->updateTeamUserList();
			}
		});
	}
	else
	{
		//in solo mode
		string uu = UserModel::getInstance()->getUuId().c_str();
		_allUnitGroup[0]->setName(uu.c_str());
		_allUnitGroup[0]->setSelected(true);
		_allUnitGroup[0]->setPlayerNameLabel("My Player");
		_allUnitGroup[0]->addUnitButtonClickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchUnitButton, this));
		_allUnitGroup[0]->addSkill1ButtonCLickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchSkill1Button, this));
		_allUnitGroup[0]->addSkill2ButonClickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchSkill2Button, this));

		_allUnitGroup[1]->setSelected(false);
		_allUnitGroup[2]->setSelected(false);
		
	}
	createPageView(1);

	checkFlag();
	return true;
}

void MultiUnitSelectScene::updateContent(Ref *p)
{
	CC_UNUSED_PARAM(p);
	if (_pageFlg == SOLO_MODE) return;
	//for show detail in unit slot
	auto userInfo = UserModel::getInstance()->getUserInfo();
	auto uuid = UserModel::getInstance()->getUuId();
	vector<RoomUser> teamInfo = {};
	switch (userInfo.team_id)
	{
	case TEAM_FLG_BLUE:
		teamInfo = RoomUserModel::getInstance()->getBlueTeamUserList();
		break;
	case TEAM_FLG_RED:
		teamInfo = RoomUserModel::getInstance()->getRedTeamUserList();
		break;
	}
	
	/*int size = _allUnitGroup.size();
	if (teamInfo.size() < size)
	{
		size = teamInfo.size();
		for (int i = size-1; i < _allUnitGroup.size(); i ++)
		{
			_allUnitGroup[i]->resetDefaultStatus();
		} 
	}*/
	for (auto &group : _allUnitGroup)
	{
		group->resetDefaultStatus();
	}
	for (int i = 0; i < teamInfo.size(); i++)
	{
		_allUnitGroup[i]->setPlayerNameLabel(teamInfo[i].name.c_str());
		_allUnitGroup[i]->setName(teamInfo[i]._uuid.c_str());
		if (teamInfo[i].name ==userInfo.name)
		{
			_allUnitGroup[i]->setSelected(true);
			_allUnitGroup[i]->addUnitButtonClickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchUnitButton, this));
			_allUnitGroup[i]->addSkill1ButtonCLickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchSkill1Button, this));
			_allUnitGroup[i]->addSkill2ButonClickCallback(CC_CALLBACK_0(MultiUnitSelectScene::onTouchSkill2Button, this));
		
		}
		else {
			_allUnitGroup[i]->setSelected(false);
		}
		log("team ready: %s  - %d", teamInfo[i]._uuid.c_str(), teamInfo[i]._ready);
		if (teamInfo[i]._ready == 1) {
			_allUnitGroup[i]->showReadyForBattle(true);
		}
		else
		{
			_allUnitGroup[i]->showReadyForBattle(false);
		}
	}
	auto unitList = RoomUserModel::getInstance()->getTeamUnitList();
	auto skillList = RoomUserModel::getInstance()->getTeamSkillList();
	/*if (unitList.size() < _allUnitGroup.size()) {
		for (int i = unitList.size(); i < _allUnitGroup.size(); i++)
		{
			_allUnitGroup[i]->getUnitButton()->resetClickableButton();
			_allUnitGroup[i]->getSkillButon(1)->resetClickableButton();
			_allUnitGroup[i]->getSkillButon(2)->resetClickableButton();
		}
	}*/

	for (auto &unit : unitList)
	{
		for (auto &group : _allUnitGroup)
		{
			if (strcmp(unit.uuid.c_str(), group->getName().c_str()) == 0) {
				group->setUnitIcon(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(unit.mst_unit_id).c_str());
				if (strcmp(unit.uuid.c_str(), uuid.c_str()) == 0) _sendUnitFlg = true;
				break;
			}
		}
	}
	
	for (auto &g : _allUnitGroup)
	{
		int found = 0;
		for (int i = 0; i < skillList.size() && found < 2; i ++)
		{
			if (strcmp(skillList[i].uuid.c_str(), g->getName().c_str()) == 0) {
				g->setSkillIcon(skillList[i].skill_index+1, UserSkillModel::getInstance()->getSkillInfoById(skillList[i].mst_skill_id).skill_icon_path.c_str());
				if (strcmp(skillList[i].uuid.c_str(), uuid.c_str()) == 0) _sendSkillFlg = true;
				found++;
			}
		}
	}
	checkFlag();
}

void MultiUnitSelectScene::checkFlag()
{
	if (_sendSkillFlg && _sendUnitFlg && !_isSentRequest) {
		nextButton->setTouchEnabled(true);
		nextButton->loadTextureNormal("image/screen/battle.png");
	}
	else {
		nextButton->setTouchEnabled(false);
		nextButton->loadTextureNormal("image/screen/battle_disable.png");
	}
}
void MultiUnitSelectScene::onEnter()
{
	LayerBase::onEnter();
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(MultiUnitSelectScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(MultiUnitSelectScene::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(MultiUnitSelectScene::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	touchListener->setSwallowTouches(false);
}
bool MultiUnitSelectScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	CC_UNUSED_PARAM(unused_event);
	if (_onTouchDisable) return false;
	_touchBeginPoint = touch->getLocation();
	//log("layer touch began");
	return true;
}
void MultiUnitSelectScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	//log("layer touch ended");
	CC_UNUSED_PARAM(touch);
	CC_UNUSED_PARAM(unused_event);
}

void MultiUnitSelectScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	CC_UNUSED_PARAM(unused_event);
	//log("layer touch move");
	Vec2 distanVector = _touchBeginPoint - touch->getLocation();
	if (abs(distanVector.x) > _mainPage->getCustomScrollThreshold()) {
		_onTouchDisable = true;
	}

}

void MultiUnitSelectScene::onTouchUnitButton()
{
	if (_onTouchDisable) return;
	//show Unit Select page view
	createPageView(1);
	_defaultLabel->setString("ユニットを選択して下さい");
	_onSelectSkillButtonIndex = -1;
}

void MultiUnitSelectScene::onTouchSkill1Button()
{
	if (_onTouchDisable) return;
	//show Skill select page view with select target is skill 1 button
	if (_onSelectSkillButtonIndex < 0)
	{
		createPageView(2);
	}
	_defaultLabel->setString("スキルを選択して下さい");
	_onSelectSkillButtonIndex = 0;
}

void MultiUnitSelectScene::onTouchSkill2Button()
{
	if (_onTouchDisable) return;
	//show Skill select page view with select target is skill 1 button
	if (_onSelectSkillButtonIndex < 0)
	{
		createPageView(2);
	}
	_defaultLabel->setString("スキルを選択して下さい");
	_onSelectSkillButtonIndex = 1;
}

void MultiUnitSelectScene::onBackButtonClick(Ref *pSender)
{
	if (_onTouchDisable) return;
	switch (_pageFlg)
	{
	case SOLO_MODE:
	{
		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, ModeSelectScene::createScene()));
		break;
	}
	case MULTI_MODE:
	{
		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene(/*global uerId*/0)));
		break;
	}
		
	default:
		break;
	}
}

void MultiUnitSelectScene::createPageView(int type)
{
	Size baseSize = _pagebackGround->getContentSize();
	float space = baseSize.width - 150;
	if (_pagebackGround->getChildByTag(11110) != nullptr)
	{
		_mainPage->removeFromParent();
		_mainPage = nullptr;
	}
	_mainPage = PageView::create();
	_mainPage->setTag(11110);
	_mainPage->setContentSize(Size(space, baseSize.height - 40));
	_mainPage->setPosition(Vec2(75,0));
	_mainPage->removeAllPages();

	int size = 0;
	if (type == 1)
	{
		size = _allUnitInfoNew.size();
	}
	else
	{
		size = _allSkillInfo.size();
	}

	_pageNum = (size / 4);
	if (_pageNum * 4 < size) {
		_pageNum += 1;
	}
	if (_pageNum < 2) rArrow->setVisible(false);
	log("page num: %d", _pageNum);
	float baseX = space * 1 / 8;
	float spaceX = space * 1 / 4;
	for (int i = 0; i < _pageNum; i++)
	{
		HBox *lay = HBox::create();
		auto mum = Layer::create();
		lay->setContentSize(Size(baseSize.width - 150, baseSize.height - 40));
		mum->setContentSize(lay->getContentSize());
		lay->addChild(mum);
		lay->setSwallowTouches(false);
		mum->setSwallowsTouches(false);
		for (int j = 1; j < 5; j++)
		{
			int index = j + i * 4 - 1;
			if (index < size) {
				auto sprite = Button::create();
				sprite->setTag(index);
				if (type == 1)
				{
					sprite->loadTextureNormal(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(_allUnitInfoNew[j + i * 4 - 1].mst_unit_id).c_str());
					sprite->setScale(5.0f);
				}
				else
				{
					sprite->loadTextureNormal(_allSkillInfo[index].skill_icon_path);
				}
				sprite->setSwallowTouches(false);
				sprite->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchPageItem, this, type));
				int yValue = lay->getContentSize().height / 2 + 20;
				sprite->setPosition(Vec2(baseX + spaceX *(j-1), yValue));
				mum->addChild(sprite);
			}
		}
		_mainPage->insertPage(lay, i);
		_mainPage->setSwallowTouches(false);
		_mainPage->setUsingCustomScrollThreshold(true);
		_mainPage->setCustomScrollThreshold(space / 6);
	}
	//_mainPage->removePageAtIndex(0);
	_mainPage->addEventListener(CC_CALLBACK_2(MultiUnitSelectScene::pageViewEvent, this));
	_pagebackGround->addChild(_mainPage);

	lArrow->setVisible(false);
	rArrow->setVisible(true);
}

void MultiUnitSelectScene::onTouchPageItem(Ref *pSender, Widget::TouchEventType type, int pageType)
{
	if (_onTouchDisable == true) return;
	
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//log("button end");
		Button* unit = dynamic_cast<Button*>(pSender);
		int tag = unit->getTag();
		/*Bellow code: don't trigger touch event for invisible unit*/
		int curPageIndex = _mainPage->getCurPageIndex();
		if (tag >= (curPageIndex + 1) * 4) return;
		if (tag < curPageIndex * 4) return;

		_onTouchDisable = true;
		if (pageType == 1)
		{
			getParent()->addChild(UnitDetailDialog::create(_allUnitInfoNew[tag], CC_CALLBACK_0(MultiUnitSelectScene::decideCallBack, this, tag,pageType), CC_CALLBACK_0(MultiUnitSelectScene::cancelCallBack, this)));
			//_sendUnitFlg = false;
		}
		else
		{
			getParent()->addChild(SkillDetailDialog::create(_allSkillInfo[tag], CC_CALLBACK_0(MultiUnitSelectScene::decideCallBack, this, tag, pageType), CC_CALLBACK_0(MultiUnitSelectScene::cancelCallBack, this)));
			//_sendSkillFlg = false;
		}
		if (_pageFlg == MULTI_MODE)
		{
			auto a = UserModel::getInstance()->getUserInfo();
			Document doc;
			doc.SetObject();
			Document::AllocatorType& allo = doc.GetAllocator();

			doc.AddMember("room_id", a.room_id, allo);
			doc.AddMember("team_id", a.team_id, allo);
			doc.AddMember("user_id", a.user_id, allo);
			string uu = UserModel::getInstance()->getUuId().c_str();
			doc.AddMember("uuid", uu.c_str(), allo);

			StringBuffer buff;
			Writer<StringBuffer> wt(buff);
			doc.Accept(wt);

			auto client = NodeServer::getInstance()->getClient();
			_isSentRequest = false;
			checkFlag();
			client->emit("connect_not_ready", buff.GetString());
		}
		
		break;
	}
	default:
		break;
	}
}

void MultiUnitSelectScene::decideCallBack(int index, int pageType)
{
	_isSentRequest = false;
	_onTouchDisable = false;
	if (pageType == 1)
	{
		UserModel::getInstance()->setUserUnitsInfo(_allUnitInfoNew[index]);
		UserModel::getInstance()->setSelectedUnitId(_allUnitInfoNew[index].mst_unit_id);
		if (_pageFlg == MULTI_MODE) 
		{
			_sendUnitFlg = false;
			sendSelectUnitInfo(_allUnitInfoNew[index].mst_unit_id);
		}
		else
		{
			//show unit infor in solo mode
			_allUnitGroup[0]->setUnitIcon(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(_allUnitInfoNew[index].mst_unit_id).c_str());
			_sendUnitFlg = true;
		}
		createPageView(2);
		_onSelectSkillButtonIndex = 0;
		_defaultLabel->setString("スキルを選択して下さい");
	}
	else 
	{
		if (_onSelectSkillButtonIndex >= 0) {
			_allSelectedSkilId[_onSelectSkillButtonIndex] = index;
			//for show skill icon in local
			for (auto &group : _allUnitGroup)
			{
				string uu = UserModel::getInstance()->getUuId().c_str();
				if (strcmp(uu.c_str(), group->getName().c_str()) == 0)
				{
					group->setSkillIcon(_onSelectSkillButtonIndex + 1, UserSkillModel::getInstance()->getSkillInfoById(_allSkillInfo[index].mst_skill_id).skill_icon_path.c_str());
				}

			}
			if (_onSelectSkillButtonIndex == 0) {
				_onSelectSkillButtonIndex = 1;
			}
			sendSElectSkillInfo();
		}
	}
	checkFlag();
}

void MultiUnitSelectScene::cancelCallBack()
{
	_onTouchDisable = false;
}

void MultiUnitSelectScene::sendReadyButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable == true || _isSentRequest) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_sendSkillFlg && _sendUnitFlg)
		{
			if (_pageFlg == MULTI_MODE)
			{
				auto a = UserModel::getInstance()->getUserInfo();
				Document doc;
				doc.SetObject();
				Document::AllocatorType& allo = doc.GetAllocator();

				doc.AddMember("room_id", a.room_id, allo);
				doc.AddMember("team_id", a.team_id, allo);
				doc.AddMember("user_id", a.user_id, allo);
				doc.AddMember("unit_id", UserModel::getInstance()->getSelectedUnitId(), allo);
				string uu = UserModel::getInstance()->getUuId().c_str();
				doc.AddMember("uuid", uu.c_str(), allo);

				StringBuffer buff;
				Writer<StringBuffer> wt(buff);
				doc.Accept(wt);

				auto client = NodeServer::getInstance()->getClient();
				_isSentRequest = true;
				checkFlag();
				client->emit("connect_ready", buff.GetString());
				client->on("room_public_battle_start", CC_CALLBACK_2(MultiUnitSelectScene::startBattleCallback, this));
			}
			else {
				startBattle();
			}
		}
		break;
	}
	default:
		break;
	}
}
void MultiUnitSelectScene::startBattleCallback(SIOClient* client, const std::string& data)
{
	BattleModel::getInstance()->parserJsonToInitData(data.c_str());
	startBattle();
}

void MultiUnitSelectScene::startBattle()
{
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BattleScene::createScene()));
}

void MultiUnitSelectScene::pageViewEvent(Ref *pSender, PageView::EventType type)
{

	switch (type)
	{
	case cocos2d::ui::PageView::EventType::TURNING:
	{
		PageView* pageView = dynamic_cast<PageView*>(pSender);
		int pageIndex = pageView->getCurPageIndex();
		log("page %d",pageIndex);
		lArrow->setVisible(true);
		rArrow->setVisible(true);
		_onTouchDisable = false;
		if (pageIndex == 0) {
			lArrow->setVisible(false);
		}
		if (pageIndex == _pageNum - 1) {
			rArrow->setVisible(false);
		}
		break;
	}
	default:
		break;
	}
}

void MultiUnitSelectScene::getDataFromDataBase()
{
	_allUnitInfoNew = UserUnitModel::getInstance()->getUserUnitList();
	_allSkillInfo = UserSkillModel::getInstance()->getPlayerSkillsList();
}

void MultiUnitSelectScene::arrowButonClick(Ref *pSender, Widget::TouchEventType type, int pageScroll)
{
	CC_UNUSED_PARAM(pSender);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_mainPage->scrollToPage(_mainPage->getCurPageIndex() + pageScroll);
		break;
	}
	default:
		break;
	}
}



void MultiUnitSelectScene::sendSelectUnitInfo(int unitId)
{
	if (unitId == 0) return;

	
	auto userData = UserModel::getInstance()->getUserInfo();
	auto unitData = UserUnitModel::getInstance()->getUnitInfoById(unitId);
	if (_pageFlg == MULTI_MODE)
	{
		Document doc;
		doc.SetObject();
		Document::AllocatorType& allo = doc.GetAllocator();

		doc.AddMember("room_id", userData.room_id, allo);
		doc.AddMember("user_id", userData.user_id, allo);
		doc.AddMember("team_id", userData.team_id, allo);
		doc.AddMember("unit_id", unitId, allo);
		doc.AddMember("mst_unit", *UserUnitModel::getInstance()->convertFromUserUnitInfoToJson(unitData, allo), allo);
		string uu = UserModel::getInstance()->getUuId().c_str();
		doc.AddMember("uuid", uu.c_str(), allo);
		StringBuffer buff;
		Writer<StringBuffer> writer(buff);
		doc.Accept(writer);

		auto client = NodeServer::getInstance()->getClient();
		if (client == nullptr) return;
		log("Unit Selected End data: %s", buff.GetString());
		client->emit("connect_select_unit", buff.GetString());
		client->on("connect_select_unit_end", [&](SIOClient* client, const std::string& data) {
			CC_UNUSED_PARAM(client);
			log("select unit end data: %s", data.c_str());
			_sendUnitFlg = true;
			checkFlag();
			RoomUserModel::getInstance()->parseTeamData(data);
		});
	}
}

void MultiUnitSelectScene::sendSElectSkillInfo()
{
	vector<UserSkillInfo> skills;
	for (auto &var : _allSelectedSkilId)
	{
		if (var < 0)
		{
			return;
		}
		skills.push_back(_allSkillInfo[var]);
	}

	if (UserDefault::getInstance()->getIntegerForKey("MODE") == SOLO_MODE) {
		UserModel::getInstance()->setSelectedSkillList(skills);
		_sendSkillFlg = true;
		return;
	}
	else {
		_sendSkillFlg = false;
		auto a = UserModel::getInstance()->getUserInfo();
		Document doc;
		doc.SetObject();
		Document::AllocatorType& allo = doc.GetAllocator();

		doc.AddMember("room_id", a.room_id, allo);
		doc.AddMember("team_id", a.team_id, allo);
		doc.AddMember("user_id", a.user_id, allo);
		doc.AddMember("unit_id", UserModel::getInstance()->getSelectedUnitId(), allo);
		rapidjson::Value listSkill;
		vector<int> listSkillId;
		listSkill.SetArray();
		for (int i = 0; i < skills.size(); i++)
		{
			listSkill.PushBack(skills[i].mst_skill_id, allo);
			listSkillId.push_back(skills[i].mst_skill_id);
		}
		doc.AddMember("player_skill_list", listSkill, allo);
		string uu = UserModel::getInstance()->getUuId().c_str();
		doc.AddMember("uuid", uu.c_str(), allo);

		StringBuffer buff;
		Writer<StringBuffer> wt(buff);
		doc.Accept(wt);

		auto client = NodeServer::getInstance()->getClient();
		log("send select skills data: %s", buff.GetString());
		client->emit("connect_select_skill", buff.GetString());
		string temp = buff.GetString();
		BattleModel::getInstance()->setPlayerSkills(listSkillId);
		client->on("connect_select_skill_end", [&, temp](SIOClient* client, const std::string& data) {
			CC_UNUSED_PARAM(client);
			log("select skill end data: %s", data.c_str());
			RoomUserModel::getInstance()->parseTeamData(data);
			_sendSkillFlg = true;
			checkFlag();
		});
	}
}












