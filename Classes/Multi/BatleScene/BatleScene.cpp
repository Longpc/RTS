#include "BatleScene.h"


#define MOVE_SPEED 250
#define IMAGE_SCALE 0.6
#define ANIMETE_DELAY 0.25

#define LOW 1
#define MID 2
#define HIGH 3

#define ICON 10

Scene* BatleScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Vect::ZERO);
	auto lay = BatleScene::create();
	lay->savePhysicWorld(scene->getPhysicsWorld());
	scene->addChild(lay);
	return scene;
}

bool BatleScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	_menu->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);

	_imagePath = "image/unit_new/move/red/";

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(BatleScene::nextButtonCallback, this));
	addChild(nextButton, 10);

	createContent();
	return true;
}

void BatleScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene()));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}
void BatleScene::onEnter()
{
	LayerBase::onEnter();
	srand(time(NULL));
	time(&timer);
	timeinfo = localtime(&timer);
	scheduleUpdate();
}

void BatleScene::update(float delta)
{
	updateMiniMap();
	updateTime();
}

void BatleScene::updateTime()
{
	time_t currTimer;
	time(&currTimer);

	int seconds = ceil(difftime(currTimer, timer));
	log("%d", seconds);
	_timeViewLabel->setString(makeTimeString(seconds).c_str());


}
string BatleScene::makeTimeString(int second) {
	stringstream timeString;
	int remainSecond = second % 60;
	int minus = (second / 60);
	int hous = (second / 3600);
	
	if (hous >= 10) {
		timeString << hous << ":";
	}
	else {
		timeString << "0" << hous << ":";
	}

	if (minus >= 10) {
		timeString << minus << ":";
	}
	else {
		timeString << "0" << minus << ":";
	}

	if (remainSecond >= 10) {
		timeString << remainSecond;
	}
	else {
		timeString << "0" << remainSecond;
	}

	return timeString.str().c_str();
}
void BatleScene::createContent()
{
	_battleBackround = Node::create();
	_battleBackround->setPosition(Vec2::ZERO);
	_battleBackround->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	addChild(_battleBackround);

	auto part1 = createBackground(_visibleSize);
	part1->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_battleBackround->addChild(part1);
	auto part2 = createBackground(_visibleSize);
	part2->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_battleBackround->addChild(part2);
	auto part3 = createBackground(_visibleSize);
	part3->setAnchorPoint(Vec2::ANCHOR_BOTTOM_RIGHT);
	_battleBackround->addChild(part3);
	auto part4 = createBackground(_visibleSize);
	part4->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_battleBackround->addChild(part4);

	createPhysicBolder();

	testObject = Sprite::create("image/unit_new/move/red/unit_00_08_1.png");
	_battleBackround->addChild(testObject,MID);
	testObject->setPosition(Vec2(100, 100));
	testObject->setPhysicsBody(PhysicsBody::createCircle(45, PhysicsMaterial(1, 0, 1)));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->setScale(IMAGE_SCALE);
	//auto action = AnimationFromImage::createAnimation(1, "image/unit_new/move/red/");
	
	/*auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_01_%d.png", i);
		string p = "image/unit_new/move/red/";
		p.append(szName);
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(0.2f);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);*/
	//auto action = Animate::create(createAnimationWithDefine(1,"image/unit_new/move/red/"));
	
	//testObject->runAction(RepeatForever::create(action));


	auto folow = Follow::create(testObject);
	folow->setTag(121);
	_battleBackround->runAction(folow);
	auto bolder = Node::create();
	//bolder->setPosition(Vec2::ZERO);
	//_battleBackround->addChild(bolder);
	//bolder->runAction(folow->clone());
	//bolder->setPhysicsBody(PhysicsBody::createEdgeBox(Size(2 * part1->getContentSize()), PhysicsMaterial(1, 1, 1)));
	_battleBackround->setPhysicsBody(PhysicsBody::createEdgeBox(Size(1, 1), PhysicsMaterial(1, 1, 1)));



	float baseMargin = 15;
	Sprite *topMenu = Sprite::create("image/screen/battle/status_parts.png");
	topMenu->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	topMenu->setPosition(Vec2(0, _visibleSize.height));
	addChild(topMenu);

	_hpSlider = Slider::create();
	_hpSlider->loadBarTexture("image/screen/battle/hp_base.png");
	_hpSlider->setPercent(100);
	_hpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_hpSlider->loadProgressBarTexture("image/screen/battle/hp.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_hpSlider->setPosition(Vec2(topMenu->getContentSize().width / 2 + 25, _visibleSize.height - 22));
	addChild(_hpSlider);

	Vec2 slotPos = Vec2(45, topMenu->getContentSize().height/2);

	_characterImageViewNode = ClippingNode::create();
	_characterImageViewNode->setAlphaThreshold(0);
	_characterImageViewNode->setPosition(Vec2::ZERO);
	
	string path = _imagePath;
	path.append("unit_00_02_1.png");

	auto mask = Sprite::create("image/screen/battle/icon_slot.png");
	mask->setPosition(slotPos);
	_characterImageViewNode->setStencil(mask);

	auto icon = Sprite::create(path.c_str());
	icon->setPosition(slotPos+Vec2(5,-10));
	icon->setScale(IMAGE_SCALE);
	icon->setTag(ICON);
	_characterImageViewNode->addChild(icon);
	topMenu->addChild(_characterImageViewNode);



	_menuButton = Button::create();
	_menuButton->loadTextureNormal("image/screen/battle/menu_btn.png");
	_menuButton->addTouchEventListener(CC_CALLBACK_2(BatleScene::menuButtonCallback, this));
	_menuButton->setPosition(Vec2(_menuButton->getContentSize().width / 2 + 10, _visibleSize.height - topMenu->getContentSize().height));
	_menuButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	addChild(_menuButton);

	Sprite *timeViewContainer = Sprite::create("image/screen/battle/time_parts.png");
	timeViewContainer->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	timeViewContainer->setPosition(Vec2(_visibleSize.width - baseMargin, _visibleSize.height - baseMargin));
	addChild(timeViewContainer);

	_miniMap = Sprite::create("image/screen/battle/map.png");
	_miniMap->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	_miniMap->setPosition(timeViewContainer->getPosition() - Vec2(0, 50));
	addChild(_miniMap);

	_selectRect = Sprite::create("image/screen/battle/map_frame.png");
	_selectRect->setPosition(Vec2(_miniMap->getContentSize().width / 2, _miniMap->getContentSize().height / 2 + _selectRect->getContentSize().height / 2));
	_miniMap->addChild(_selectRect);

	_mini_Icon = Sprite::create("mini_icon.png");
	_mini_Icon->setPosition(Vec2(_selectRect->getContentSize().width / 2, _selectRect->getContentSize().height / 2));
	_selectRect->addChild(_mini_Icon);


	_timeViewLabel = Label::create("00:00:00", "", 25);
	_timeViewLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	_timeViewLabel->setPosition(Vec2(timeViewContainer->getContentSize().width / 2, timeViewContainer->getContentSize().height / 2));
	timeViewContainer->addChild(_timeViewLabel);

	string skill1ImagePath = "image/screen/battle/skill_icon_1.png";
	string skill2ImagePath = "image/screen/battle/skill_icon_2.png";
	string skill3ImagePath = "image/screen/battle/skill_icon_3.png";
	string skill4ImagePath = "image/screen/battle/skill_icon_4.png";

	_skill1Button = Button::create();
	_skill1Button->loadTextureNormal(skill1ImagePath.c_str());
	_skill1Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	Size baseSize = _skill1Button->getContentSize();
	_skill1Button->setPosition(Vec2(_visibleSize.width / 2 - 1.5 * baseSize.width - 20, baseSize.height / 2 + baseMargin));
	addChild(_skill1Button);

	_skill2Button = Button::create();
	_skill2Button->loadTextureNormal(skill2ImagePath.c_str());
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill4ButtonCallback, this));
	_skill2Button->setPosition(Vec2(_visibleSize.width / 2 - 0.5 *baseSize.width - 10, baseSize.height / 2 + baseMargin));
	addChild(_skill2Button);

	_skill3Button = Button::create();
	_skill3Button->loadTextureNormal(skill3ImagePath.c_str());
	_skill3Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill3ButtonCallback, this));
	_skill3Button->setPosition(Vec2(_visibleSize.width / 2 + 0.5*baseSize.width+10, baseSize.height / 2 + baseMargin));
	addChild(_skill3Button);

	_skill4Button = Button::create();
	_skill4Button->loadTextureNormal(skill4ImagePath.c_str());
	_skill4Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill4ButtonCallback, this));
	_skill4Button->setPosition(Vec2(_visibleSize.width / 2 + 1.5 * baseSize.width + 20, baseSize.height / 2 + baseMargin));
	addChild(_skill4Button);

	_manaSlider = Slider::create();
	_manaSlider->loadBarTexture("image/screen/battle/mp_base.png");
	_manaSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_manaSlider->loadProgressBarTexture("image/screen/battle/mp.png");
	_manaSlider->setPercent(100);
	_manaSlider->setPosition(Vec2(_visibleSize.width / 2, baseSize.height + baseMargin*2));
	addChild(_manaSlider);
}


Sprite* BatleScene::createBackground(Vec2 pos)
{
	auto sp = Sprite::create("image/screen/battle/bg.png");
	sp->setPosition(pos);
	return sp;
}
void BatleScene::createPhysicBolder()
{
	auto bottomB = createHBolder();
	bottomB->setPosition(Vec2(_visibleSize.width, 30));
	_battleBackround->addChild(bottomB);

	auto topB = createHBolder();
	topB->setPosition(Vec2(_visibleSize.width, _visibleSize.height * 2));
	_battleBackround->addChild(topB);

	auto leftB = createVBolder();
	leftB->setPosition(Vec2(-10, _visibleSize.height));
	_battleBackround->addChild(leftB);

	auto rightB = createVBolder();
	rightB->setPosition(Vec2(_visibleSize.width * 2, _visibleSize.height));
	_battleBackround->addChild(rightB);

	createRandomRock();
}

Node* BatleScene::createHBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(_visibleSize.width*2, 20), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;
}

Node* BatleScene::createVBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(20, _visibleSize.height*2), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;

}
void BatleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

bool BatleScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	_touchStartPoint = touch->getLocation();
	return true;
}

void BatleScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	fakeZOrder();
	auto distanVector = touch->getLocation() - _touchStartPoint;
	testObject->getPhysicsBody()->setVelocity(Vect(MOVE_SPEED*cos(distanVector.getAngle()),MOVE_SPEED*sin(distanVector.getAngle())));
	//testObject->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
	_mini_Icon->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
	log("%f", _mini_Icon->getRotation());
	int direc = detectDirectionBaseOnTouchAngle(_mini_Icon->getRotation());
	if(direc != 0) actionCharacter(direc);
	//_touchStartPoint = touch->getLocation();
	
}
int BatleScene::detectDirectionBaseOnTouchAngle(float angle)
{
	if(caculAvgAngle(0,angle)) return 8;
	if (caculAvgAngle(45,angle)) return 9;
	if(caculAvgAngle(90,angle)) return 6;
	if(caculAvgAngle(135,angle)) return 3;
	if(caculAvgAngle(180,angle)) return 2;
	if(caculAvgAngle(225,angle)) return 1;
	if(caculAvgAngle(-90,angle)) return 4;
	if (caculAvgAngle(-45,angle))
	{
		return 7;
	}
	return 0;
}
bool BatleScene::caculAvgAngle(int avg, float angle)
{
	if (angle > avg - 22 && angle < avg + 22) return true;
	return false;
}
void BatleScene::actionCharacter(int directionId)
{
	if (testObject->getNumberOfRunningActions() > 0) {
		if (testObject->getActionByTag(directionId) != nullptr) {
			log("!!!");
			return;
		}
	}
	testObject->stopAllActions();
	auto action = Animate::create(createAnimationWithDefine(directionId, _imagePath));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	testObject->runAction(repeat);
}
void BatleScene::updateMiniMap()
{
	if (testObject == nullptr) return;
	auto objectPos = testObject->getPosition();

	float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
	float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
	_selectRect->setPosition(Vec2(objectPos.x*positionXScaleRate,objectPos.y*positionYScaleRate));

}


void BatleScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	testObject->getPhysicsBody()->setVelocity(Vect::ZERO);
	testObject->stopAllActions();
}

void BatleScene::menuButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_myWorld->getDebugDrawMask() == PhysicsWorld::DEBUGDRAW_ALL) {
			_myWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
		}
		else {
			_myWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
		}

		changeImagePathforTest();
		break; 
	}

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::savePhysicWorld(PhysicsWorld *world)
{
	_myWorld = world;
}

Animation* BatleScene::createAnimationWithDefine(int imageId, string path)
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_%d.png",imageId, i);
		string p = path;
		p.append(szName);
		log("%s", p.c_str());
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}

void BatleScene::changeImagePathforTest()
{
	int a = random(1, 5);
	switch (a)
	{
	case 1:
		_imagePath = "image/unit_new/move/red/";
		break;
	case  2:
		_imagePath = "image/unit_new/move/green/";
		break;
	case 3:
		_imagePath = "image/unit_new/move/blue/";
		break;
	case 4:
		_imagePath = "image/unit_new/move/black/";
		break;
	case 5:
		_imagePath = "image/unit_new/move/purple/";
		break;
	default:
		_imagePath = "image/unit_new/move/red/";
		break;
	}
	
	_characterImageViewNode->removeChildByTag(ICON);
	string path = _imagePath;
	path.append("unit_00_02_1.png");
	Vec2 slotPos = Vec2(45, 45);
	auto icon = Sprite::create(path.c_str());
	icon->setPosition(slotPos + Vec2(5, -10));
	icon->setScale(IMAGE_SCALE);
	icon->setTag(ICON);
	_characterImageViewNode->addChild(icon);

}

void BatleScene::createRandomRock()
{
	for (int i = 1; i < 6; i++)
	{
		auto sp = Sprite::create("stone.png");
		MyBodyParser::getInstance()->parseJsonFile("json/stone.json");
		auto body = MyBodyParser::getInstance()->bodyFormJson(sp, "stone");
		body->setDynamic(false);
		body->getShape(0)->setRestitution(0);
		body->setGravityEnable(false);
		sp->setPhysicsBody(body);
		
		sp->setPosition(Vec2(random(0.1f, 0.9f)*_visibleSize.width * 2,random(0.1f,0.9f)*_visibleSize.height*2));
		_allStone.push_back(sp);
		_battleBackround->addChild(_allStone.back(),LOW);
	}
}

void BatleScene::fakeZOrder()
{
	for (auto &stone : _allStone)
	{
		if (stone->getPositionY() < testObject->getPositionY() - 25)
		{
			stone->setZOrder(HIGH);
		}
		else {
			stone->setZOrder(LOW);
		}
	}
}














