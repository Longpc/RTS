#include "SkillSelectScene.h"
#define SLOT_NUMBER 2
Scene * SkillSelectScene::createScene(int unitId)
{
	auto scene = Scene::create();
	auto lay = SkillSelectScene::create(unitId);
	scene->addChild(lay);

	return scene;
}

SkillSelectScene* SkillSelectScene::create(int unit)
{
	SkillSelectScene *layer = new SkillSelectScene();
	if (layer->init(unit))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return NULL;
}

bool SkillSelectScene::init(int unit)
{
	if (!LayerBase::init()) {
		return false;
	}
	_unitInfo = unit;
	_defaultLabel->setString("Please select skill");

	_slot1BackGroundButton = createSlotBaseSprite(Vec2(_visibleSize.width / 2 - BUTON_MARGIN/2, _visibleSize.height - 150));
	addChild(_slot1BackGroundButton);
	_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
	float baseX = _slot1BackGroundButton->getContentSize().width / 2;
	float baseY = -20;
	_slot1BackGroundButton->addChild(createUnitNameBg(Vec2(baseX, baseY)));
	_skill1NameLabel = createUniNameLabel(Vec2(baseX, baseY));
	_slot1BackGroundButton->addChild(_skill1NameLabel);


	_slot2BackGroundButton = createSlotBaseSprite(Vec2(_visibleSize.width / 2+BUTON_MARGIN/2, _visibleSize.height - 150));
	addChild(_slot2BackGroundButton);
	_slot2BackGroundButton->addChild(createUnitNameBg(Vec2(baseX, baseY)));
	_skill2NameLabel = createUniNameLabel(Vec2(baseX, baseY));
	_slot2BackGroundButton->addChild(_skill2NameLabel);

	/*_slot3BackGroundButton = createSlotBaseSprite(Vec2(_visibleSize.width / 2 + BUTON_MARGIN, _visibleSize.height - 150));
	addChild(_slot3BackGroundButton);
	_slot3BackGroundButton->addChild(createUnitNameBg(Vec2(baseX, baseY)));
	_skill3nameLabel = createUniNameLabel(Vec2(baseX, baseY));
	_slot3BackGroundButton->addChild(_skill3nameLabel);*/

	_slot1 = createSlot(Vec2(_visibleSize.width / 2 - BUTON_MARGIN/2, _visibleSize.height - 150));
	_slot2 = createSlot(Vec2(_visibleSize.width / 2 + BUTON_MARGIN/2, _visibleSize.height - 150));
	//_slot3 = createSlot(Vec2(_visibleSize.width / 2 + BUTON_MARGIN, _visibleSize.height - 150));

	addChild(_slot1);
	addChild(_slot2);
	//addChild(_slot3);


	button1 = Button::create();
	button1->loadTextureNormal("image/screen/skillSelect/face_button.png");
	button1->setTouchEnabled(true);
	button1->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnitSlot1, this));
	button1->setPosition(Vec2(_visibleSize.width / 2 - BUTON_MARGIN/2, _visibleSize.height - 150));
	_slot1->addChild(button1);

	button2 = Button::create();
	button2->loadTextureNormal("image/screen/skillSelect/face_button.png");
	button2->setTouchEnabled(true);
	button2->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnitSlot2, this));
	button2->setPosition(Vec2(_visibleSize.width / 2+BUTON_MARGIN/2, _visibleSize.height - 150));
	_slot2->addChild(button2);

	/*button3 = Button::create();
	button3->loadTextureNormal("image/screen/skillSelect/face_button.png");
	button3->setTouchEnabled(true);
	button3->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnitSlot3, this));
	button3->setPosition(Vec2(_visibleSize.width / 2 + BUTON_MARGIN, _visibleSize.height - 150));
	_slot3->addChild(button3);*/

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("image/button/new/button_decide.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, _visibleSize.height-100));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::nextButtonCallback, this));
	addChild(nextButton, 10);

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
	if (abs(distanVector.x ) > _mainPage->getCustomScrollThreshold()) {
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
void SkillSelectScene::displayUnit(Button *parent, LabelTTF *label, int unitId)
{
	parent->loadTextureNormal(_allSkillInfo[unitId]._imagePath);
	label->setString(_allSkillInfo[unitId]._name);
	//parent->setScale(0.3);
	if (_onSelectedSlot < SLOT_NUMBER) {
		_onSelectedSlot++;
		setSelectedSlot(_onSelectedSlot);
	}
}

void SkillSelectScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onTouchDisable) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleScene::createScene()));
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
	Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1,pageFlg)));
}

void SkillSelectScene::decideCallBack(Ref *pSender, Widget::TouchEventType type)
{
	onSelectUnit(_onSelectedUnitId);
	_onTouchDisable = false;
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
		log("touch unit %d", tag);
		int curPageIndex = _mainPage->getCurPageIndex();
		if (tag > (curPageIndex + 1) * 4) return;
		if (tag <= curPageIndex * 4) return;

		_onSelectedUnitId = tag-1;
		_onTouchDisable = true;
		auto dialod = SkillDetailDialog::create(_allSkillInfo[tag-1], CC_CALLBACK_2(SkillSelectScene::decideCallBack, this), CC_CALLBACK_2(SkillSelectScene::cancelCallBack, this));
		getParent()->addChild(dialod);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}

}

void SkillSelectScene::onSelectUnit(int unitId)
{
	switch (_onSelectedSlot)
	{
	case 1:
	{
		displayUnit(button1,_skill1NameLabel, unitId);
		break;
	}
	case 2:
	{
		displayUnit(button2,_skill2NameLabel, unitId);
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
LabelTTF* SkillSelectScene::createUniNameLabel(Vec2 pos)
{
	auto lb = LabelTTF::create("", "fonts/Marker Felt.ttf", 20);
	lb->setPosition(pos);
	lb->setHorizontalAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::BLACK);
	return lb;
}

void SkillSelectScene::createAllUnitView()
{
	for (int i = 1; i < 11; i++)
	{
		SkillInfo temp;
		temp._name = "Skill";
		temp._skillId = i;
		temp._description = "this skill is . ........";
		std::stringstream path;
		path << "image/skillnew/buff/" << i << ".png";
		temp._imagePath = path.str().c_str();
		_allSkillInfo.push_back(temp);
	}
	auto spite = Sprite::create("image/screen/unitSelect/back.png");
	spite->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 120));
	addChild(spite);
	Size baseSize = spite->getContentSize();

	lArrow = Button::create("image/screen/unitSelect/left.png");
	lArrow->setPosition(Vec2(50, baseSize.height / 2));
	spite->addChild(lArrow);
	lArrow->setSwallowTouches(true);
	lArrow->setVisible(false);

	rArrow = Button::create("image/screen/unitSelect/right.png");
	rArrow->setPosition(Vec2(baseSize.width - 50, baseSize.height / 2));
	rArrow->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
	spite->addChild(rArrow);
	rArrow->setSwallowTouches(true);

	float space = baseSize.width - 150;

	_mainPage = PageView::create();
	_mainPage->setContentSize(Size(space, baseSize.height - 40));
	_mainPage->setPosition(Vec2(75, 0));
	_mainPage->removeAllPages();

	_pageNum = (_allSkillInfo.size() / 4);
	if (_pageNum * 4 < _allSkillInfo.size()) {
		_pageNum += 1;
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
			if ((j + i * 4 - 1) < _allSkillInfo.size()) {
				auto sprite = Button::create();
				sprite->setTag(_allSkillInfo[j + i * 4 - 1]._skillId);
				sprite->loadTextureNormal(_allSkillInfo[j + i * 4 - 1]._imagePath);
				sprite->setSwallowTouches(false);
				sprite->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnit, this));
				int yValue = lay->getContentSize().height / 2 + 20;
				sprite->setPosition(Vec2(baseX + spaceX *(j - 1), yValue));
				mum->addChild(sprite);
			}
		}
		_mainPage->insertPage(lay, i);
		_mainPage->setSwallowTouches(false);
		_mainPage->setUsingCustomScrollThreshold(true);
		_mainPage->setCustomScrollThreshold(space / 6);
	}
	//_mainPage->removePageAtIndex(0);
	_mainPage->addEventListener(CC_CALLBACK_2(SkillSelectScene::pageViewEvent, this));
	spite->addChild(_mainPage);
	/*auto spite = Sprite::create("image/navigator/dialog_background.png");
	spite->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height / 2 - 100));
	addChild(spite);

	auto scroll = extension::ScrollView::create();
	scroll->setViewSize(spite->getContentSize());
	scroll->setPosition(Vec2::ZERO);
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::HORIZONTAL);
	scroll->updateInset();
	scroll->setVisible(true);

	spite->addChild(scroll);
	auto layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	layer->setContentSize(Size(_allSkillInfo.size()*BASE_SPACE_X, spite->getContentSize().height));
	
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	float baseMargin = 120;
	for (int i = 0; i < _allSkillInfo.size(); i++)
	{
		auto sprite = Button::create();
		//sprite->setScale(0.5);
		sprite->setTag(_allSkillInfo[i]._skillId);
		sprite->loadTextureNormal(_allSkillInfo[i]._imagePath);
		sprite->setSwallowTouches(false);
		int xValue = (i*0.5);
		sprite->addTouchEventListener(CC_CALLBACK_2(SkillSelectScene::onTouchUnit, this));
		sprite->setPosition(Vec2(baseMargin + BASE_SPACE_X*xValue, layer->getContentSize().height-(baseMargin + BASE_SPACE_X*(i % 2))));
		layer->addChild(sprite);
	}
	layer->setPosition(Vec2(0, 0));*/
}

ClippingNode* SkillSelectScene::createSlot(Vec2 position)
{
	auto clipNode = ClippingNode::create();
	clipNode->setAlphaThreshold(0);
	clipNode->setPosition(Vec2::ZERO);
	clipNode->setTag(111);
	clipNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto mask = Sprite::create("image/screen/skillSelect/slot_rect.png");
	mask->setPosition(position);
	clipNode->setStencil(mask);

	return clipNode;
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
		_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
		_slot2BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		//_slot3BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
		break;
	}
	case 2:
	{
		_slot2BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame.png");
		_slot1BackGroundButton->loadTextureNormal("image/screen/skillSelect/00_frame_inactive.png");
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
