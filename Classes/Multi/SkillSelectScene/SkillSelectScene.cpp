#pragma execution_character_set("utf-8")
#include "SkillSelectScene.h"
#define SLOT_NUMBER 2
Scene * SkillSelectScene::createScene()
{
	auto scene = Scene::create();
	auto lay = SkillSelectScene::create();
	scene->addChild(lay);

	return scene;
}

SkillSelectScene* SkillSelectScene::create()
{
	SkillSelectScene *layer = new SkillSelectScene();
	if (layer->init())
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

bool SkillSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}

	auto a = UserModel::getInstance()->getUserInfo();
	_userNameLabel->setString(a.name.c_str());

	getSkillDataFromDatabase();
	_selectedUnitId = UserModel::getInstance()->getSelectedUnitId();
	_allSelectedSkilId.push_back(-1);
	_allSelectedSkilId.push_back(-1);
	_defaultLabel->setString("スキルを選択して下さい");

	_skillSlot1 = ClipingButtonBase::create("image/screen/skillSelect/slot_rect.png", "image/screen/skillSelect/00_frame_inactive.png", "image/screen/skillSelect/00_frame.png");
	_skillSlot1->setPosition(Vec2(_visibleSize.width / 2 - BUTON_MARGIN / 2, _visibleSize.height - 150));
	addChild(_skillSlot1,100);
	_skillSlot1->setSelected(true);
	_skillSlot1->addTouchEvent(CC_CALLBACK_2(SkillSelectScene::onTouchUnitSlot1, this));
	auto _slot1BackGroundButton = _skillSlot1->getBackgroundButton();
	float baseX = _slot1BackGroundButton->getContentSize().width / 2;
	float baseY = -20;
	_slot1BackGroundButton->addChild(createUnitNameBg(Vec2(baseX, baseY)));
	_skill1NameLabel = createUniNameLabel(Vec2(baseX, baseY));
	_slot1BackGroundButton->addChild(_skill1NameLabel);


	_skillSlot2 = ClipingButtonBase::create("image/screen/skillSelect/slot_rect.png", "image/screen/skillSelect/00_frame_inactive.png", "image/screen/skillSelect/00_frame.png");
	_skillSlot2->setPosition(Vec2(_visibleSize.width / 2 + BUTON_MARGIN / 2, _visibleSize.height - 150));
	_skillSlot2->setSelected(false);
	_skillSlot2->addTouchEvent(CC_CALLBACK_2(SkillSelectScene::onTouchUnitSlot2, this));
	addChild(_skillSlot2, 100);
	auto _slot2BackGroundButton = _skillSlot2->getBackgroundButton();
	_slot2BackGroundButton->addChild(createUnitNameBg(Vec2(baseX, baseY)));
	_skill2NameLabel = createUniNameLabel(Vec2(baseX, baseY));
	_slot2BackGroundButton->addChild(_skill2NameLabel);

	_decideButton = Button::create();
	_decideButton->loadTextureNormal("image/button/new/button_decide.png");
	_decideButton->setPosition(Vec2(_visibleSize.width - 50, _visibleSize.height-100));
	_decideButton->setTouchEnabled(false);
	_decideButton->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::nextButtonCallback, this));
	addChild(_decideButton, 10);

	createAllUnitView();

	return true;
}

void SkillSelectScene::onEnter()
{
	LayerBase::onEnter();
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(SkillSelectScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(SkillSelectScene::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(SkillSelectScene::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	touchListener->setSwallowTouches(true);

	scheduleUpdate();
}
bool SkillSelectScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (_onTouchDisable) return false;
	//log("layer touch began");
	_beginTouchPoint = touch->getLocation();
	return true;
}
void SkillSelectScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	//log("layer touch ended");
}

void SkillSelectScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	//log("layer touch move");
	Vec2 distanVector = _beginTouchPoint - touch->getLocation();
	if (abs(distanVector.x ) > _mainPageView->getCustomScrollThreshold()) {
		_onTouchDisable = true;
	}
	
}
void SkillSelectScene::update(float delta)
{

}

void SkillSelectScene::onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	log("slot1");
	_onSelectedSlot = 1;
	setSelectedSlot(1);
}

void SkillSelectScene::onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	log("slot2");
	_onSelectedSlot = 2;
	setSelectedSlot(2);
}

/*void SkillSelectScene::onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	log("slot3");
	_onSelectedSlot = 3;
	setSelectedSlot(3);
}
*/
void SkillSelectScene::showSkill(Button *parent, Label *label, int skillIndex)
{
	parent->loadTextureNormal(_allSkillInfo[skillIndex].skill_icon_path);
	label->setString(_allSkillInfo[skillIndex].name);
	//parent->setScale(0.3);
	if (_onSelectedSlot < SLOT_NUMBER) {
		_onSelectedSlot++;
		setSelectedSlot(_onSelectedSlot);
	}
}

void SkillSelectScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable || _isSentRequest) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
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
			startBattle();
		}
		else {
			auto a = UserModel::getInstance()->getUserInfo();
			Document doc;
			doc.SetObject();
			Document::AllocatorType& allo = doc.GetAllocator();

			doc.AddMember("room_id", a.room_id, allo);
			doc.AddMember("team_id", a.team_id, allo);
			doc.AddMember("user_id", a.user_id, allo);
			doc.AddMember("unit_id", _selectedUnitId, allo);
			rapidjson::Value listSkill;
			vector<int> listSkillId;
			listSkill.SetArray();
			for (int i = 0; i < skills.size(); i++)
			{
				listSkill.PushBack(skills[i].mst_skill_id, allo);
				listSkillId.push_back(skills[i].mst_skill_id);

				//targetList.AddMember("target_unique_id", targetsId[i], allo);
			}
			doc.AddMember("player_skill_list", listSkill, allo);
			string uu = UserModel::getInstance()->getUuId().c_str();
			doc.AddMember("uuid", uu.c_str(), allo);

			StringBuffer buff;
			Writer<StringBuffer> wt(buff);
			doc.Accept(wt);

			auto client = NodeServer::getInstance()->getClient();
			client->emit("connect_select_skill", buff.GetString());
			_isSentRequest = true;
			string temp = buff.GetString();
			BattleModel::getInstance()->setPlayerSkills(listSkillId);
			client->on("connect_select_skill_end", [&, temp](SIOClient* client, const std::string& data) {
				log("select skill end data: %s", data.c_str());
				RoomUserModel::getInstance()->parseTeamData(data);
				client->emit("connect_ready", temp.c_str());
				client->on("room_public_battle_start", CC_CALLBACK_2(SkillSelectScene::startBattleCallback, this));
			});
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
void SkillSelectScene::onBackButtonClick(Ref *pSender)
{
	if (_onTouchDisable) return;
	int pageFlg = UserDefault::getInstance()->getIntegerForKey("MODE");
	if (pageFlg == MULTI_MODE) {
		/*Send event to delete selected unit data*/
		auto a = UserModel::getInstance()->getUserInfo();
		Document doc;
		doc.SetObject();
		Document::AllocatorType& allo = doc.GetAllocator();

		doc.AddMember("room_id", a.room_id, allo);
		doc.AddMember("team_id", a.team_id, allo);
		doc.AddMember("user_id", a.user_id, allo);
		doc.AddMember("unit_id", _selectedUnitId, allo);
		string uu = UserModel::getInstance()->getUuId().c_str();
		doc.AddMember("uuid", uu.c_str(), allo);

		StringBuffer buff;
		Writer<StringBuffer> wt(buff);
		doc.Accept(wt);

		/*auto client = NodeServer::getInstance()->getClient();
		client->emit("re_select_unit", buff.GetString());
		client->on("re_select_unit_end", [&, pageFlg](SIOClient * client, const string data) {*/
			Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1, pageFlg)));
		//});
	}
	else {
		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1, pageFlg)));
	}


	
}

void SkillSelectScene::decideCallBack(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		setSelectedSkillINfo(_onSelectSkillTag);
		_onTouchDisable = false;
		_isSentRequest = false;
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
	
}

void SkillSelectScene::cancelCallBack(Ref *pSender, Widget::TouchEventType type)
{
	_onTouchDisable = false;
}

void SkillSelectScene::onTouchUnit(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable == true) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		Button* unit = dynamic_cast<Button*>(pSender);
		int tag = unit->getTag();
		log("touch skill %d", tag);
		int curPageIndex = _mainPageView->getCurPageIndex();
		if (tag +1 > (curPageIndex + 1) * 4) return;
		if (tag + 1  <= curPageIndex * 4) return;

		_onSelectSkillTag = tag;
		_onTouchDisable = true;
		auto dialod = SkillDetailDialog::create(_allSkillInfo[tag], CC_CALLBACK_2(SkillSelectScene::decideCallBack, this), CC_CALLBACK_2(SkillSelectScene::cancelCallBack, this));
		getParent()->addChild(dialod);
		_decideButton->setTouchEnabled(false);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}

}

void SkillSelectScene::setSelectedSkillINfo(int skillindex)
{
	switch (_onSelectedSlot)
	{
	case 1:
	{
		showSkill(_skillSlot1->getClickableButton(),_skill1NameLabel, skillindex);
		_allSelectedSkilId[0] = skillindex;
		break;
	}
	case 2:
	{
		showSkill(_skillSlot2->getClickableButton(),_skill2NameLabel, skillindex);
		_allSelectedSkilId[1] = skillindex;
		break;
	}
	/*case 3:
	{
		displayUnit(button3, _skill3nameLabel,unitId);
		break;
	}*/
	default:
		break;
	}
	sendSelectedSkillList();
}

void SkillSelectScene::sendSelectedSkillList()
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
	}
	else {
		auto a = UserModel::getInstance()->getUserInfo();
		Document doc;
		doc.SetObject();
		Document::AllocatorType& allo = doc.GetAllocator();

		doc.AddMember("room_id", a.room_id, allo);
		doc.AddMember("team_id", a.team_id, allo);
		doc.AddMember("user_id", a.user_id, allo);
		doc.AddMember("unit_id", _selectedUnitId, allo);
		rapidjson::Value listSkill;
		vector<int> listSkillId;
		listSkill.SetArray();
		for (int i = 0; i < skills.size(); i++)
		{
			listSkill.PushBack(skills[i].mst_skill_id, allo);
			listSkillId.push_back(skills[i].mst_skill_id);

			//targetList.AddMember("target_unique_id", targetsId[i], allo);
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
		_isSentRequest = true;
		string temp = buff.GetString();
		BattleModel::getInstance()->setPlayerSkills(listSkillId);
		client->on("connect_select_skill_end", [&, temp](SIOClient* client, const std::string& data) {
			log("select skill end data: %s", data.c_str());
			_decideButton->setTouchEnabled(true);
		});
	}
}

Button* SkillSelectScene::createSlotBaseSprite(Vec2 pos)
{
	auto sp = Button::create();
	sp->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
	sp->setEnabled(false);
	sp->setPosition(pos);
	return sp;
}

Sprite* SkillSelectScene::createUnitNameBg(Vec2 pos)
{
	auto sp = Sprite::create("image/screen/unitSelect/name.png");
	sp->setPosition(pos);
	return sp;
}
Label* SkillSelectScene::createUniNameLabel(Vec2 pos)
{
	auto lb = Label::createWithSystemFont("", JAPANESE_FONT_1_BOLD, 20);
	lb->setPosition(pos);
	lb->setHorizontalAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::BLACK);
	return lb;
}

void SkillSelectScene::createAllUnitView()
{
	
	auto spite = Sprite::create("image/screen/unitSelect/back.png");
	spite->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 120));
	addChild(spite);
	Size baseSize = spite->getContentSize();

	lArrow = Button::create("image/screen/unitSelect/left.png");
	lArrow->setPosition(Vec2(50, baseSize.height / 2));
	spite->addChild(lArrow);
	lArrow->setSwallowTouches(true);
	lArrow->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::leftArrowClickCallback, this));
	lArrow->setVisible(false);

	rArrow = Button::create("image/screen/unitSelect/right.png");
	rArrow->setPosition(Vec2(baseSize.width - 50, baseSize.height / 2));
	rArrow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	spite->addChild(rArrow);
	rArrow->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::rightArrowClickCallback, this));
	rArrow->setSwallowTouches(true);

	float space = baseSize.width - 150;

	_mainPageView = PageView::create();
	_mainPageView->setContentSize(Size(space, baseSize.height - 40));
	_mainPageView->setPosition(Vec2(75, 0));
	_mainPageView->removeAllPages();

	_pageNum = (_allSkillInfo.size() / 4);
	if (_pageNum * 4 < _allSkillInfo.size()) {
		_pageNum += 1;
	}
	if (_pageNum < 2) {
		rArrow->setVisible(false);
	}
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
			if ((index) < _allSkillInfo.size()) {
				auto sprite = Button::create();
				sprite->setTag(index);
				sprite->loadTextureNormal(_allSkillInfo[index].skill_icon_path);
				sprite->setSwallowTouches(false);
				sprite->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnit, this));
				int yValue = lay->getContentSize().height / 2 + 20;
				sprite->setPosition(Vec2(baseX + spaceX *(j - 1), yValue));
				mum->addChild(sprite);
			}
		}
		_mainPageView->insertPage(lay, i);
		_mainPageView->setSwallowTouches(false);
		_mainPageView->setUsingCustomScrollThreshold(true);
		_mainPageView->setCustomScrollThreshold(space / 6);
	}
	//_mainPage->removePageAtIndex(0);
	_mainPageView->addEventListener(CC_CALLBACK_2(SkillSelectScene::pageViewEvent, this));
	spite->addChild(_mainPageView);
	
}

void SkillSelectScene::pageViewEvent(Ref *pSender, PageView::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::PageView::EventType::TURNING:
	{

		PageView* pageView = dynamic_cast<PageView*>(pSender);
		int pageIndex = pageView->getCurPageIndex();
		log("page %d", pageIndex);
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

void SkillSelectScene::setSelectedSlot(int slotNum)
{
	switch (slotNum)
	{
	case 1:
	{
		_skillSlot1->setSelected(true);
		_skillSlot2->setSelected(false);
// 		_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
// 		_slot2BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		//_slot3BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		break;
	}
	case 2:
	{
		_skillSlot2->setSelected(true);
		_skillSlot1->setSelected(false);
// 		_slot2BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
// 		_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		//_slot3BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		break;
	}
	/*case 3:
	{
		_slot3BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
		_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		_slot2BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		break;
	}*/
	default:
		break;
	}
}

void SkillSelectScene::getSkillDataFromDatabase()
{
	_allSkillInfo = UserSkillModel::getInstance()->getPlayerSkillsList();

}

void SkillSelectScene::leftArrowClickCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_mainPageView->scrollToPage(_mainPageView->getCurPageIndex() - 1);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void SkillSelectScene::rightArrowClickCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_mainPageView->scrollToPage(_mainPageView->getCurPageIndex() + 1);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void SkillSelectScene::startBattleCallback(SIOClient* client, const std::string& data)
{
	BattleModel::getInstance()->parserJsonToInitData(data.c_str());
	startBattle();
}

void SkillSelectScene::startBattle()
{
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BattleScene::createScene()));
}

SkillSelectScene::~SkillSelectScene()
{
	_onTouchDisable = false;
	_isSentRequest = false;
}
