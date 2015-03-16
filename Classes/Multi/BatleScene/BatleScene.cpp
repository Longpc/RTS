#include "BatleScene.h"


#define MOVE_SPEED 250
#define IMAGE_SCALE 0.6
#define ANIMETE_DELAY 0.25
#define ATTACK_ANIMATION_DELAY 1

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

	_moveImagePath = "image/unit_new/move/red/";
	_attackImagePath = "image/unit_new/attack/red/";

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
	checkForAutoAttack();
}
void BatleScene::checkForAutoAttack()
{
	float area = IMAGE_SCALE*_autoAttackArea->getContentSize().width / 2 + 25;
	for (int i = 0; i < _alltargetUnit.size(); i++)
	{
		auto posDistan = _alltargetUnit[i]->getPosition() - testObject->getPosition();

		if (posDistan.length() < area && _alltargetUnit[i]->isVisible()) {
			if (testObject->getActionByTag(_currentAttackActionTag) == nullptr && _onDelayAttackFlg == false) {
				int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
				//rotateCharacter(testObject,direc);
				auto ani = createAttackAnimationWithDefine(direc, _attackImagePath);
				//auto action = RepeatForever::create(Animate::create(ani)); //user repeat for change in future
				auto call1 = CallFuncN::create(CC_CALLBACK_0(BatleScene::characerAttackCallback, this));
				auto action = Sequence::create(Animate::create(ani),call1,nullptr);
				action->setTag(direc * 10);
				_currentAttackActionTag = direc * 10;
				testObject->runAction(action);

				string path = "image/unit_new/attack/red/";
				auto target_ani = createAttackAnimationWithDefine(10 - direc, path);
				auto call2 = CallFuncN::create(CC_CALLBACK_0(BatleScene::enemyAttackCallback, this));
				auto action2 = Sequence::create(Animate::create(target_ani),call2,nullptr);
				action2->setTag(direc);
				//rotateCharacter(_alltargetUnit[i], 10 - direc);
				_alltargetUnit[i]->runAction(action2);
				_indexOfRunningActionTarget = i;

				_onDelayAttackFlg = true;
				this->runAction(Sequence::create(DelayTime::create(ATTACK_ANIMATION_DELAY), CallFuncN::create(CC_CALLBACK_0(BatleScene::removeceAttackDelayFlg, this)), nullptr));
			}
		}
		else {
			//testObject->stopActionByTag(_currentAttackActionTag);
		}
	}
	/*auto posDistan = _testAttackTarget->getPosition() - testObject->getPosition();

	if (posDistan.length() < area ) {
		//log("Auto attack: %f",posDistan.getAngle()*RAD_DEG);
		int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
		//log("direc %d ",direc);
		if (testObject->getActionByTag(_currentAttackActionTag) == nullptr && _onDelayAttackFlg == false) {
			auto ani = createAttackAnimationWithDefine(direc, _attackImagePath);
			//auto action = RepeatForever::create(Animate::create(ani)); //user repeat for change in future
			auto action = Animate::create(ani);
			action->setTag(direc * 10);
			_currentAttackActionTag = direc * 10;
			testObject->runAction(action);

			string path = "image/unit_new/attack/red/";
			auto target_ani = createAttackAnimationWithDefine(10 - direc, path);
			auto action2 = Animate::create(target_ani);
			action2->setTag(direc);

			_testAttackTarget->runAction(action2);

			_onDelayAttackFlg = true;
			this->runAction(Sequence::create(DelayTime::create(ATTACK_ANIMATION_DELAY), CallFuncN::create(CC_CALLBACK_0(BatleScene::removeceAttackDelayFlg, this)), nullptr));
		}
	}
	else {
		testObject->stopActionByTag(_currentAttackActionTag);
	}*/
}
void BatleScene::characerAttackCallback()
{
	//log("charater");
	if (_allEnemyHpBar[_indexOfRunningActionTarget]->getPercent() > 0) {
		_allEnemyHpBar[_indexOfRunningActionTarget]->setPercent(_allEnemyHpBar[_indexOfRunningActionTarget]->getPercent() - 20);
		showAttackDame(20, _alltargetUnit[_indexOfRunningActionTarget]->getPosition() + Vec2(0, 100));

	}
	else {
		_alltargetUnit[_indexOfRunningActionTarget]->setVisible(false);
		_allEnemyIconInMinimap[_indexOfRunningActionTarget]->setVisible(false);
	}
}

void BatleScene::enemyAttackCallback()
{
	//log("enemy");
	if (_miniHpSlider->getPercent() > 0)
	{
		_miniHpSlider->setPercent(_miniHpSlider->getPercent() - 10);
		_hpSlider->setPercent(_miniHpSlider->getPercent());
		showAttackDame(10, testObject->getPosition() + Vec2(0, 100));
	}
	else {
		testObject->setPosition(Vec2(100, 100));
		_miniHpSlider->setPercent(100);
		_hpSlider->setPercent(100);
	}

}

void BatleScene::showAttackDame(int dameValue, Vec2 pos)
{
	stringstream ss;
	ss << ""<<dameValue;
	auto lab = Label::create(ss.str().c_str(), "", 30);
	lab->setTextColor(Color4B::RED);
	lab->setPosition(pos);
	_battleBackround->addChild(lab,9999);
	auto action = Sequence::create(Spawn::create(ScaleBy::create(0.25f, 1.5), MoveBy::create(0.5f, Vec3(0, 100, 0)),nullptr), RemoveSelf::create(true), nullptr);
	lab->runAction(action);
}


void BatleScene::removeceAttackDelayFlg() {
	testObject->stopActionByTag(_currentAttackActionTag);
	_alltargetUnit[_indexOfRunningActionTarget]->stopActionByTag(10 - (_currentAttackActionTag / 10));
	_onDelayAttackFlg = false;
}
void BatleScene::updateTime()
{
	time_t currTimer;
	time(&currTimer);

	int seconds = ceil(difftime(currTimer, timer));
	//log("%d", seconds);
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
	testObject->setPhysicsBody(PhysicsBody::createCircle(50, PhysicsMaterial(1, 0, 1)));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->setScale(IMAGE_SCALE);

	_miniHpSlider = Slider::create();
	_miniHpSlider->loadBarTexture("image/screen/battle/mini_hp_base.png");
	_miniHpSlider->setPercent(100);
	_miniHpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_miniHpSlider->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_miniHpSlider->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height-10));
	testObject->addChild(_miniHpSlider);

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
	
	string path = _moveImagePath;
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
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill2ButtonCallback, this));
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

	auto action = RotateBy::create(1, 180);


	_touchMoveBeginSprite = Sprite::create("image/screen/battle/ui_move.png");
	_touchMoveBeginSprite->setVisible(false);
	addChild(_touchMoveBeginSprite);
	_touchMoveBeginSprite->runAction(RepeatForever::create(action));
	
	_touchMoveEndSprite = Sprite::create("image/screen/battle/ui_move_end.png");
	_touchMoveEndSprite->setVisible(false);
	addChild(_touchMoveEndSprite);
	_touchMoveEndSprite->runAction(RepeatForever::create(action->clone()));
	
	_selectTargetSprite = Sprite::create("image/screen/battle/select.png");
	_selectTargetSprite->setVisible(false);
	_battleBackround->addChild(_selectTargetSprite);

	_autoAttackArea = Sprite::create("image/screen/battle/select.png");
	testObject->addChild(_autoAttackArea);
	_autoAttackArea->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2 - 30));

	for (int i = 0; i < 10; i++)
	{
		auto sp = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
		sp->setPosition(Vec2(2*_visibleSize.width*random(0.2f, 0.8f), 2*_visibleSize.height*random(0.2f, 0.8f)));
		sp->setScale(IMAGE_SCALE);
		sp->setTag(i);

		auto hpB = Slider::create();
		hpB->loadBarTexture("image/screen/battle/mini_hp_base.png");
		hpB->setPercent(100);
		hpB->loadSlidBallTextureNormal("image/screen/battle/test.png");
		hpB->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
		//_hpSlider->setContentSize(Size(372, 12));
		hpB->setPosition(Vec2(sp->getContentSize().width / 2, sp->getContentSize().height - 10));
		_allEnemyHpBar.push_back(hpB);
		sp->addChild(_allEnemyHpBar.back(),100);
		_alltargetUnit.push_back(sp);
		_battleBackround->addChild(_alltargetUnit.back(),MID);

		auto enemyIcon = Sprite::create("image/screen/battle/enemyicon.png");
		enemyIcon->setOpacity(255);
		float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
		float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
		enemyIcon->setPosition(Vec2(sp->getPositionX()*positionXScaleRate, sp->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon);
		_miniMap->addChild(_allEnemyIconInMinimap.back(),-1);
	}
	/*_testAttackTarget = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
	_testAttackTarget->setPosition(_visibleSize);
	_testAttackTarget->setScale(IMAGE_SCALE);
	_battleBackround->addChild(_testAttackTarget);*/
	
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
	bottomB->setPosition(Vec2(_visibleSize.width, -50));
	_battleBackround->addChild(bottomB);

	auto topB = createHBolder();
	topB->setPosition(Vec2(_visibleSize.width, _visibleSize.height * 2+50));
	_battleBackround->addChild(topB);

	auto leftB = createVBolder();
	leftB->setPosition(Vec2(-50, _visibleSize.height));
	_battleBackround->addChild(leftB);

	auto rightB = createVBolder();
	rightB->setPosition(Vec2(_visibleSize.width * 2 + 50, _visibleSize.height));
	_battleBackround->addChild(rightB);

	createRandomRock();
}

Node* BatleScene::createHBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(_visibleSize.width*2, 100), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;
}

Node* BatleScene::createVBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(100, _visibleSize.height*2), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;

}
void BatleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		testObject->stopActionByTag(_currentMoveActionTag);
		selectAttackTarget();
		
		break; }
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		testObject->stopActionByTag(_currentMoveActionTag);
		selectAttackTarget();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		testObject->stopActionByTag(_currentMoveActionTag);

		selectAttackTarget();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		testObject->stopActionByTag(_currentMoveActionTag);
		selectAttackTarget();
		break; }

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

bool BatleScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	_touchStartPoint = touch->getLocation();
	if (_moveDisableFlg == false){
		_touchMoveBeginSprite->setPosition(_touchStartPoint);
		_touchMoveBeginSprite->setVisible(true);
	}
	else {
		_selectTargetSprite->setPosition(testObject->getPosition());
		_selectTargetSprite->setVisible(true);
	}
	
	return true;
}

void BatleScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	if (_moveDisableFlg == true) {
		_touchMoveBeginSprite->setVisible(false);
		_touchMoveEndSprite->setVisible(false);
		_selectTargetSprite->setVisible(true);
		auto dis = touch->getLocation() - testObject->getPosition();
		_selectTargetSprite->setPosition(_battleBackround->convertTouchToNodeSpace(touch));
		_mini_Icon->setRotation(-(dis.getAngle() * RAD_DEG) + 90);
		int direc = detectDirectionBaseOnTouchAngle(_mini_Icon->getRotation());
		if (direc == 0) return;
		rotateCharacter(testObject, direc);
		return;
	}
	fakeZOrder();
	_touchMoveEndSprite->setPosition(touch->getLocation());
	_touchMoveEndSprite->setVisible(true);
	auto distanVector = touch->getLocation() - _touchStartPoint;

	if (distanVector.length() < _touchMoveBeginSprite->getContentSize().width / 2) {
		testObject->getPhysicsBody()->setVelocity(Vect(0, 0));
		testObject->stopActionByTag(_currentMoveActionTag);
		return;
	}

	testObject->stopActionByTag(_currentAttackActionTag);
	testObject->getPhysicsBody()->setVelocity(Vect(MOVE_SPEED*cos(distanVector.getAngle()),MOVE_SPEED*sin(distanVector.getAngle())));
	_mini_Icon->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
	//log("%f", _mini_Icon->getRotation());
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
	if(caculAvgAngle(-90,angle) || caculAvgAngle(270,angle)) return 4;
	if (caculAvgAngle(-45,angle))
	{
		return 7;
	}
	return 8;
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
			//log("!!!");
			return;
		}
	}
	testObject->stopActionByTag(_currentMoveActionTag);
	auto action = Animate::create(createMoveAnimationWithDefine(directionId, _moveImagePath));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	_currentMoveActionTag = directionId;
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
	//log("touch END");
	if (_selectTargetSprite->isVisible()) {
		_selectTargetSprite->setVisible(false);
		runAttackAnimation();
		//_moveDisableFlg = false;
	}
	else {
		testObject->getPhysicsBody()->setVelocity(Vect::ZERO);
		testObject->stopActionByTag(_currentMoveActionTag);
		_touchMoveBeginSprite->setVisible(false);
		_touchMoveEndSprite->setVisible(false);
	}
	
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

Animation* BatleScene::createMoveAnimationWithDefine(int imageId, string path)
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_%d.png",imageId, i);
		string p = path;
		p.append(szName);
		//log("%s", p.c_str());
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}
Animation* BatleScene::createAttackAnimationWithDefine(int imageId, string path)
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_attack_%d.png", imageId, i);
		string p = path;
		p.append(szName);
		//log("%s", p.c_str());
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
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	case  2:
		_moveImagePath = "image/unit_new/move/green/";
		_attackImagePath = "image/unit_new/attack/green/";
		break;
	case 3:
		_moveImagePath = "image/unit_new/move/blue/";
		_attackImagePath = "image/unit_new/attack/blue/";
		break;
	case 4:
		_moveImagePath = "image/unit_new/move/black/";
		_attackImagePath = "image/unit_new/attack/black/";
		break;
	case 5:
		_moveImagePath = "image/unit_new/move/purple/";
		_attackImagePath = "image/unit_new/attack/purple/";
		break;
	default:
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	}
	
	_characterImageViewNode->removeChildByTag(ICON);
	string path = _moveImagePath;
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
		sp->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		
		sp->setPosition(Vec2(random(0.1f, 0.9f)*_visibleSize.width * 2,random(0.1f,0.9f)*_visibleSize.height*2));
		_allStone.push_back(sp);
		_battleBackround->addChild(_allStone.back(),LOW);
	}
	for (int i = 1; i < 6; i++)
	{
		auto tree = Sprite::create("tree.png");
		MyBodyParser::getInstance()->parseJsonFile("json/tree.json");
		auto body = MyBodyParser::getInstance()->bodyFormJson(tree, "tree");
		body->setDynamic(false);
		body->getShape(0)->setRestitution(0);
		body->setGravityEnable(false);
		tree->setPhysicsBody(body);
		tree->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		tree->setPosition(Vec2(random(0.1f, 0.9f)*_visibleSize.width * 2, random(0.1f, 0.9f)*_visibleSize.height * 2));
		_allStone.push_back(tree);
		_battleBackround->addChild(_allStone.back());
	}
}

void BatleScene::fakeZOrder()
{
	for (auto &stone : _allStone)
	{
		if (stone->getPositionY() < testObject->getPositionY() - 25)
		{
			stone->setLocalZOrder(HIGH);
		}
		else {
			stone->setLocalZOrder(LOW);
		}
	}
}

void BatleScene::onPhysicContactBegin(const PhysicsContact &contact)
{

}

void BatleScene::runAttackAnimation()
{
	int direc = detectDirectionBaseOnTouchAngle(_mini_Icon->getRotation());
	if (direc == 0) return;
	//testObject->stopAllActions();

	auto action = Animate::create(createAttackAnimationWithDefine(direc, _attackImagePath));
	float time = action->getDuration();
	testObject->runAction(action);
	CallFuncN *callF = CallFuncN::create(CC_CALLBACK_0(BatleScene::removeMoveDisableFlg, this));
	this->runAction(Sequence::create(DelayTime::create(time),callF,nullptr));

}

void BatleScene::removeMoveDisableFlg()
{
	_moveDisableFlg = false;
}

void BatleScene::selectAttackTarget()
{
	testObject->stopActionByTag(_currentMoveActionTag);
	_moveDisableFlg = true;
	_selectTargetSprite->setPosition(testObject->getPosition());
	_selectTargetSprite->setVisible(true);
}

void BatleScene::rotateCharacter(Sprite *target, int direc)
{
	log("Rotate direc %d", direc);
	auto animation = Animation::create();
	char szName[100] = { 0 };
	sprintf(szName, "unit_00_0%d_attack_%d.png", direc, 1);
	string p = _attackImagePath;
	p.append(szName);
	//log("%s", p.c_str());
	animation->addSpriteFrameWithFile(p.c_str());
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	target->runAction(Animate::create(animation));
}



















