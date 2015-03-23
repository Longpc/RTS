#pragma execution_character_set("utf-8")
#include "UnitSelectScene.h"


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
	UserDefault::getInstance()->setIntegerForKey("MODE", _pageFlg);

	_defaultLabel->setString("ユニットを選択して下さい");
	
	baseSlot1 = createSlotBaseSprite(Vec2(_visibleSize.width / 2 - 150, _visibleSize.height - 150));
	addChild(baseSlot1);
	baseSlot1->loadTextureNormal("image/screen/unitSelect/active.png");
	float baseX = baseSlot1->getContentSize().width / 2;
	baseSlot1->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit1Name = createUniNameLabel(Vec2(baseX, 0));
	baseSlot1->addChild(selectedUnit1Name);


	baseSlot2 = createSlotBaseSprite(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	addChild(baseSlot2);
	baseSlot2->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit2Name = createUniNameLabel(Vec2(baseX, 0));
	baseSlot2->addChild(selectedUnit2Name);

	baseSlot3 = createSlotBaseSprite(Vec2(_visibleSize.width / 2 + 150, _visibleSize.height - 150));
	addChild(baseSlot3);
	baseSlot3->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit3Name = createUniNameLabel(Vec2(baseX, 0));
	baseSlot3->addChild(selectedUnit3Name);

	_slot1 = createSlot(Vec2(_visibleSize.width / 2 - 150, _visibleSize.height - 150));
	_slot2 = createSlot(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	_slot3 = createSlot(Vec2(_visibleSize.width / 2 + 150, _visibleSize.height - 150));

	addChild(_slot1);
	addChild(_slot2);
	addChild(_slot3);
	//On creating Cliping Node Base. When it was completed. This code will be remove;
	button1 = Button::create();
	button1->loadTextureNormal("image/screen/unitSelect/slot.png");
	button1->setTouchEnabled(true);
	button1->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchUnitSlot1, this));
	button1->setPosition(Vec2(_visibleSize.width / 2 - 150, _visibleSize.height - 150));
	_slot1->addChild(button1,9);

	button2 = Button::create();
	button2->loadTextureNormal("image/screen/unitSelect/slot.png");
	button2->setTouchEnabled(true);
	button2->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchUnitSlot2, this));
	button2->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	_slot2->addChild(button2,9);

	button3 = Button::create();
	button3->loadTextureNormal("image/screen/unitSelect/slot.png");
	button3->setTouchEnabled(true);
	button3->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchUnitSlot3, this));
	button3->setPosition(Vec2(_visibleSize.width / 2 + 150, _visibleSize.height - 150));
	_slot3->addChild(button3,9);

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("image/button/new/button_decide.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, _visibleSize.height-100));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::nextButtonCallback, this));
	addChild(nextButton, 10);
	//auto menu = Menu::create(slot1, slot2, slot3, nullptr);
	///menu->setPosition(Vec2::ZERO);
	//addChild(menu);
	createAllUnitView();
	return true;
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
	scheduleUpdate();
}
bool MultiUnitSelectScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	if (_onTouchDisable) return false;
	_touchBeginPoint = touch->getLocation();
	//log("layer touch began");
	return true;
}
void MultiUnitSelectScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	//log("layer touch ended");
}

void MultiUnitSelectScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	//log("layer touch move");
	Vec2 distanVector = _touchBeginPoint - touch->getLocation();
	if (abs(distanVector.x) > _mainPage->getCustomScrollThreshold()) {
		_onTouchDisable = true;
	}

}


ClippingNode* MultiUnitSelectScene::createSlot(Vec2 position)
{
	auto clipNode = ClippingNode::create();
	clipNode->setAlphaThreshold(0);
	clipNode->setPosition(Vec2::ZERO);
	clipNode->setTag(111);
	clipNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto mask = Sprite::create("image/navigator/selct_scene_circle.png");
	mask->setPosition(position);
	clipNode->setStencil(mask);

	return clipNode;
}

void MultiUnitSelectScene::onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	log("slot1");
	_onSelectedSlot = 1;
	setSelectedSlot(1);
}

void MultiUnitSelectScene::onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	//log("slot2");
	_onSelectedSlot = 2;
	setSelectedSlot(2);
}

void MultiUnitSelectScene::onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	//log("slot3");
	_onSelectedSlot = 3;
	setSelectedSlot(3);
}

void MultiUnitSelectScene::onSelectUnit(int unitId)
{
	switch (_onSelectedSlot)
	{
	case 1:
	{
		displayUnit(button1, selectedUnit1Name,unitId);
		break;
	}
	case 2:
	{
		displayUnit(button2,selectedUnit2Name, unitId);
		break;
	}
	case 3:
	{
		displayUnit(button3, selectedUnit3Name,unitId);
		break;
	}
	default:
		break;
	}
}
void MultiUnitSelectScene::displayUnit(Button *parent,Label *textView, int unitId)
{
	parent->loadTextureNormal(_allUnitInfoNew[unitId].image);
	textView->setString(_allUnitInfoNew[unitId].name);

	if (_onSelectedSlot < 3) {
		_onSelectedSlot++;
		setSelectedSlot(_onSelectedSlot);
	}
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
		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene()));
		break;
	}
		
	default:
		break;
	}
}
void MultiUnitSelectScene::createAllUnitView()
{
	/*for (int i = 1; i < 6; i++)
	{
		std::stringstream path;
		path << "image/unit/" << i<<".png";

		UnitInfo temp;
		temp._name = "long";
		temp._imagePath = path.str().c_str();
		temp._attack = 100*i;
		temp._defence = 100*i;
		temp._hp = 200*i;
		temp._unitId = i;
		_allUnitInfo.push_back(temp);
	}*/
	auto spite = Sprite::create("image/screen/unitSelect/back.png");
	spite->setPosition(Vec2(_visibleSize.width/2,_visibleSize.height/2 - 120));
	addChild(spite);
	Size baseSize = spite->getContentSize();

	lArrow = Button::create("image/screen/unitSelect/left.png");
	lArrow->setPosition(Vec2(50, baseSize.height / 2));
	spite->addChild(lArrow);
	lArrow->setSwallowTouches(true);
	lArrow->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::leftArrowClickCallback, this));
	lArrow->setVisible(false);

	rArrow = Button::create("image/screen/unitSelect/right.png");
	rArrow->setPosition(Vec2(baseSize.width - 50, baseSize.height / 2));
	rArrow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	rArrow->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::rightArrowClickCallback, this));
	spite->addChild(rArrow);
	rArrow->setSwallowTouches(true);

	float space = baseSize.width - 150;

	_mainPage = PageView::create();
	_mainPage->setContentSize(Size(space, baseSize.height - 40));
	_mainPage->setPosition(Vec2(75,0));
	_mainPage->removeAllPages();

	_pageNum = (_allUnitInfoNew.size() / 4);
	if (_pageNum * 4 < _allUnitInfoNew.size()) {
		_pageNum += 1;
	}
	log("page num: %d", _pageNum);
	float baseX = space * 1 / 8;
	float spaceX = space * 1/4;
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
			if ((j + i * 4-1) < _allUnitInfoNew.size()) {
				auto sprite = Button::create();
				sprite->setTag(j + i * 4 - 1);
				sprite->loadTextureNormal(_allUnitInfoNew[j+i*4 - 1].image);
				sprite->setSwallowTouches(false);
				sprite->setScale(1.5);
				sprite->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchUnit, this));
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
	spite->addChild(_mainPage);



	/*auto scroll = extension::ScrollView::create();
	scroll->setViewSize(Size(spite->getContentSize().width - 100,spite->getContentSize().height));
	scroll->setPosition(Vec2::ZERO);
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::HORIZONTAL);
	scroll->updateInset();
	scroll->setVisible(true);

	spite->addChild(scroll);
	layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	layer->setContentSize(Size(_allUnitInfo.size()*BASE_SPACE_X, spite->getContentSize().height));
	
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	float baseMargin = 100;
	for (int i = 0; i < _allUnitInfo.size(); i++)
	{
		auto sprite = Button::create();
		sprite->setTag(_allUnitInfo[i]._unitId);
		sprite->loadTextureNormal(_allUnitInfo[i]._imagePath);
		sprite->setSwallowTouches(false);
		sprite->addTouchEventListener(CC_CALLBACK_2(MultiUnitSelectScene::onTouchUnit, this));
		int xValue = (i*0.5);
		int yValue = (i % 2);
		sprite->setPosition(Vec2(baseMargin +BASE_SPACE_X*xValue, layer->getContentSize().height-(baseMargin + sprite->getContentSize().height*(i%2)) ));
		layer->addChild(sprite);
	}
	layer->setPosition(Vec2(0, 0));
	*/

}

void MultiUnitSelectScene::onTouchUnit(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable == true) return;
	
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		//log("button begin");
	}
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//log("button end");
		Button* unit = dynamic_cast<Button*>(pSender);
		int tag = unit->getTag();
		/*Bellow code: don't trigger touch event for invisible unit*/
		int curPageIndex = _mainPage->getCurPageIndex();
		if (tag >= (curPageIndex + 1) * 4) return;
		if (tag < curPageIndex * 4) return;

		_onSelectedUnitTag = tag;
		_onTouchDisable = true;
		auto dialog = UnitDetailDialog::create(_allUnitInfoNew[tag], CC_CALLBACK_2(MultiUnitSelectScene::decideCallBack, this), CC_CALLBACK_2(MultiUnitSelectScene::cancelCallBack, this));
		getParent()->addChild(dialog);
		
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiUnitSelectScene::decideCallBack(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		_decidedUnitId = _allUnitInfoNew[_onSelectedUnitTag].id;
		onSelectUnit(_onSelectedUnitTag);
		_onTouchDisable = false;
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

void MultiUnitSelectScene::cancelCallBack(Ref *pSender, Widget::TouchEventType type)
{
	_onTouchDisable = false;
}

void MultiUnitSelectScene::update(float delta)
{
	
}

void MultiUnitSelectScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable == true)
	{
		return;
	}
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_decidedUnitId == 0) break;
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, SkillSelectScene::createScene(_decidedUnitId)));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}


Button* MultiUnitSelectScene::createSlotBaseSprite(Vec2 pos)
{
	auto sp = Button::create();
	sp->loadTextureNormal("image/screen/unitSelect/inactive.png");
	sp->setEnabled(false);
	sp->setPosition(pos);
	return sp;
}

Sprite* MultiUnitSelectScene::createUnitNameBg(Vec2 pos)
{
	auto sp = Sprite::create("image/screen/unitSelect/name.png");
	sp->setPosition(pos);
	return sp;
}

Label* MultiUnitSelectScene::createUniNameLabel(Vec2 pos)
{
	auto lb = Label::create("", JAPANESE_FONT_1_BOLD, 20);
	lb->setPosition(pos);
	lb->setHorizontalAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::BLACK);
	return lb;
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

void MultiUnitSelectScene::setSelectedSlot(int slotNum)
{
	switch (slotNum)
	{
	case 1:
	{
		baseSlot1->loadTextureNormal("image/screen/unitSelect/active.png");
		baseSlot2->loadTextureNormal("image/screen/unitSelect/inactive.png");
		baseSlot3->loadTextureNormal("image/screen/unitSelect/inactive.png");
		break;
	}
	case 2:
	{
		baseSlot2->loadTextureNormal("image/screen/unitSelect/active.png");
		baseSlot1->loadTextureNormal("image/screen/unitSelect/inactive.png");
		baseSlot3->loadTextureNormal("image/screen/unitSelect/inactive.png");
		break;
	}
	case 3:
	{
		baseSlot3->loadTextureNormal("image/screen/unitSelect/active.png");
		baseSlot1->loadTextureNormal("image/screen/unitSelect/inactive.png");
		baseSlot2->loadTextureNormal("image/screen/unitSelect/inactive.png");
		break;
	}
	default:
		break;
	}
}

void MultiUnitSelectScene::getDataFromDataBase()
{
/*#define DATABASE_NAME "database.db3"
#define TABLE_NAME "unit"
	sqlite3 *data = SqlUtil::openData(DATABASE_NAME);
	string sql = "select * from unit";
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	for (auto &item : a)
	{
		UnitInforNew temp;
		temp.id = DataUtils::stringToFloat(item[0].c_str());
		temp.name = item[1];
		temp.hp = DataUtils::stringToFloat(item[2].c_str());
		temp.hp_restore = DataUtils::stringToFloat(item[3].c_str());
		temp.mp = DataUtils::stringToFloat(item[4].c_str());
		temp.mp_restore = DataUtils::stringToFloat(item[5].c_str());
		temp.attack_dame = DataUtils::stringToFloat(item[6].c_str());
		temp.defence = DataUtils::stringToFloat(item[7].c_str());
		temp.attack_sight = DataUtils::stringToFloat(item[8].c_str());
		temp.move_speed = DataUtils::stringToFloat(item[9].c_str());
		temp.attr = DataUtils::stringToFloat(item[10].c_str());
		temp.type = DataUtils::stringToFloat(item[11].c_str());
		temp.image = item[12].c_str();
		_allUnitInfoNew.push_back(temp);
		for (int i = 0; i < item.size(); i ++)
		{
			log("%s", item[i].c_str());

		}
		log("________________________");
	}

	*/
	_allUnitInfoNew = UnitData::getAllUnitData();
}

void MultiUnitSelectScene::leftArrowClickCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_mainPage->scrollToPage(_mainPage->getCurPageIndex() - 1);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiUnitSelectScene::rightArrowClickCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_mainPage->scrollToPage(_mainPage->getCurPageIndex() + 1);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}







