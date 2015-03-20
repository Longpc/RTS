#include "BatleScene.h"


#define MOVE_SPEED 250
#define IMAGE_SCALE 0.6
#define ANIMETE_DELAY 0.25
#define ATTACK_ANIMATION_DELAY 1

#define LOW 1
#define MID 2
#define HIGH 3

#define ICON 10

#define RESPAWN_DELAY 0.6f

Scene* BatleScene::createScene(int selectedUnitId, vector<SkillInfoNew> playerSkills)
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setGravity(Vect::ZERO);
	auto lay = BatleScene::create(selectedUnitId, playerSkills);
	lay->savePhysicWorld(scene->getPhysicsWorld());
	scene->addChild(lay);
	return scene;
}

BatleScene* BatleScene::create(int unitId,vector<SkillInfoNew> skills)
{
	BatleScene *layer = new BatleScene();
	if (layer && layer->init(unitId,skills)) {
		layer->autorelease();
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}

bool BatleScene::init(int unitId,vector<SkillInfoNew> skills)
{
	if (!LayerBase::init()) {
		return false;
	}

	if (UserDefault::getInstance()->getIntegerForKey(MOVE_KEY) == 0)
	{
		UserDefault::getInstance()->setIntegerForKey(MOVE_KEY, MOVE_AUTO);
		_moveMode = MOVE_AUTO;
	}
	else {
		_moveMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);
	}

	_menu->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_selectedUnitId = unitId;

	_playerSkills = skills;

	///INIT DATA FOR ALL UNIT IN BATTLE
	_mainCharacterData = getUnitDataFromDataBase(_selectedUnitId);
	_mainCharacterSkillData = getUnitSkillFromDataBase(_selectedUnitId);




	_allAlliedUnitData.push_back(_mainCharacterData);

	_allAlliedUnitCurrentHp.push_back(_mainCharacterData.hp);

	//Bellow vector is store  list of enemy unit id.
	vector<int> a;
	_allEnemyUnitData = getEnemyUnitsData(a);

	_redTeamTowerData = UnitData::getTowerDataByTeamFlg(TEAM_FLG_RED);
	_blueTeamTowerData = UnitData::getTowerDataByTeamFlg(TEAM_FLG_BLUE);



	//////////
	_moveImagePath = "image/unit_new/move/red/";
	_attackImagePath = "image/unit_new/attack/red/";

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(BatleScene::nextButtonCallback, this));
	addChild(nextButton, 10);

	auto physicDebug = Button::create();
	physicDebug->loadTextureNormal("CloseNormal.png");
	physicDebug->setPosition(Vec2(_visibleSize.width - 100, 70));
	physicDebug->setTouchEnabled(true);
	physicDebug->addTouchEventListener(CC_CALLBACK_2(BatleScene::debugPhysicButtonCallback, this));
	addChild(physicDebug, 10);

	auto changeImage = Button::create();
	changeImage->loadTextureNormal("CloseNormal.png");
	changeImage->setPosition(Vec2(_visibleSize.width - 150, 70));
	changeImage->setTouchEnabled(true);
	changeImage->addTouchEventListener(CC_CALLBACK_2(BatleScene::changeImageButtonCallback, this));
	addChild(changeImage, 10);

	createContent();
	return true;
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

	auto redTower = Sprite::create("tower_red.png");
	redTower->setPosition(Vec2(_visibleSize.width, _visibleSize.height * 2 - 250));
	//redTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(redTower);
	MyBodyParser::getInstance()->parseJsonFile("json/tower.json");
	PhysicsBody* redTBody = MyBodyParser::getInstance()->bodyFormJson(redTower, "tower");
	redTBody->setDynamic(false);
	redTBody->setGravityEnable(false);
	redTower->setPhysicsBody(redTBody);
	
	auto redTHpBar = Slider::create();
	redTHpBar->loadBarTexture("image/screen/battle/mini_hp_base.png");
	redTHpBar->setPercent(100);
	//hpB->loadSlidBallTextureNormal("image/screen/battle/test.png");
	redTHpBar->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	//_hpSlider->setContentSize(Size(372, 12));
	redTHpBar->setPosition(Vec2(redTower->getContentSize().width / 2, redTower->getContentSize().height - 10));

	redTower->addChild(redTHpBar);
	redTower->setTag(ENEMY_NUM);

	auto blueTower = Sprite::create("tower_blue.png");
	blueTower->setPosition(Vec2(_visibleSize.width, 250));
	//blueTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(blueTower);
	PhysicsBody* blueTBody = MyBodyParser::getInstance()->bodyFormJson(blueTower, "tower");
	blueTBody->setDynamic(false);
	blueTBody->setGravityEnable(false);
	blueTower->setPhysicsBody(blueTBody);

	auto blueTHpBar = Slider::create();
	blueTHpBar->loadBarTexture("image/screen/battle/mini_hp_base.png");
	blueTHpBar->setPercent(100);
	blueTHpBar->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	blueTHpBar->setPosition(Vec2(blueTower->getContentSize().width / 2, blueTower->getContentSize().height - 10));
	blueTower->addChild(blueTHpBar);
	blueTower->setTag(ENEMY_NUM);

	_allStone.push_back(redTower);
	_allStone.push_back(blueTower);


	testObject = Sprite::create("image/unit_new/move/red/unit_00_08_1.png");
	_battleBackround->addChild(testObject, MID);
	testObject->setPosition(Vec2(100, 100));
	testObject->setPhysicsBody(PhysicsBody::createCircle(50, PhysicsMaterial(1, 0, 1)));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->setScale(IMAGE_SCALE);

	_mainCharacterMiniHpBar = Slider::create();
	_mainCharacterMiniHpBar->loadBarTexture("image/screen/battle/mini_hp_base.png");
	_mainCharacterMiniHpBar->setPercent(100);
	//_miniHpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterMiniHpBar->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_mainCharacterMiniHpBar->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height - 10));
	testObject->addChild(_mainCharacterMiniHpBar);

	_allAlliedUnitHpBar.push_back(_mainCharacterMiniHpBar);
	_allAlliedUnitSprite.push_back(testObject);

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

	_mainCharacterHpBar = Slider::create();
	_mainCharacterHpBar->loadBarTexture("image/screen/battle/hp_base.png");
	_mainCharacterHpBar->setPercent(100);
	//_hpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterHpBar->loadProgressBarTexture("image/screen/battle/hp.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_mainCharacterHpBar->setPosition(Vec2(topMenu->getContentSize().width / 2 + 25, _visibleSize.height - 22));
	addChild(_mainCharacterHpBar);

	_hpViewLabel = Label::create(DataUtils::numberToString(_mainCharacterData.hp), "", 20);
	_hpViewLabel->setColor(Color3B::GREEN);
	_hpViewLabel->setContentSize(Size(150, 50));
	_hpViewLabel->setPosition(Vec2(_mainCharacterHpBar->getContentSize()) - Vec2(0, 50));
	_mainCharacterHpBar->addChild(_hpViewLabel);


	Vec2 slotPos = Vec2(45, topMenu->getContentSize().height / 2);

	_characterImageViewNode = ClippingNode::create();
	_characterImageViewNode->setAlphaThreshold(0);
	_characterImageViewNode->setPosition(Vec2::ZERO);

	string path = _moveImagePath;
	path.append("unit_00_02_1.png");

	auto mask = Sprite::create("image/screen/battle/icon_slot.png");
	mask->setPosition(slotPos);
	_characterImageViewNode->setStencil(mask);

	auto icon = Sprite::create(_mainCharacterData.image);
	icon->setPosition(slotPos + Vec2(0,0));
	icon->setScale(1.4);
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
	_miniMap->addChild(_selectRect, 2);

	_mainCharacterAvata = Sprite::create("mini_icon.png");
	_mainCharacterAvata->setPosition(Vec2(_selectRect->getContentSize().width / 2, _selectRect->getContentSize().height / 2));
	_selectRect->addChild(_mainCharacterAvata);




	_timeViewLabel = Label::create("00:00:00", "", 25);
	_timeViewLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	_timeViewLabel->setPosition(Vec2(timeViewContainer->getContentSize().width / 2, timeViewContainer->getContentSize().height / 2));
	timeViewContainer->addChild(_timeViewLabel);

	string skill1ImagePath = _mainCharacterSkillData[0].icon;
	string skill2ImagePath = _mainCharacterSkillData[1].icon;

	string skill3ImagePath = _playerSkills[0].icon;
	string skill4ImagePath = _playerSkills[1].icon;



	_skill1Button = Button::create();
	_skill1Button->loadTextureNormal(skill1ImagePath.c_str());
	_skill1Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	Size baseSize = _skill1Button->getContentSize();
	_skill1Button->setTag(TAG_SKILL_1);
	_skill1Button->setPosition(Vec2(_visibleSize.width / 2 - 1.5 * baseSize.width - 20, baseSize.height / 2 + baseMargin));
	addChild(_skill1Button);
	displaySkillMpInButton(_skill1Button, _mainCharacterSkillData[0].mp_cost);

	_skill2Button = Button::create();
	_skill2Button->loadTextureNormal(skill2ImagePath.c_str());
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill2Button->setTag(TAG_SKILL_2);
	_skill2Button->setPosition(Vec2(_visibleSize.width / 2 - 0.5 *baseSize.width - 10, baseSize.height / 2 + baseMargin));
	addChild(_skill2Button);
	displaySkillMpInButton(_skill2Button, _mainCharacterSkillData[1].mp_cost);

	_skill3Button = Button::create();
	_skill3Button->loadTextureNormal(skill3ImagePath.c_str());
	_skill3Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill3Button->setTag(TAG_SKILL_3);
	_skill3Button->setPosition(Vec2(_visibleSize.width / 2 + 0.5*baseSize.width + 10, baseSize.height / 2 + baseMargin));
	addChild(_skill3Button);
	displaySkillMpInButton(_skill3Button, _playerSkills[0].mp_cost);

	_skill4Button = Button::create();
	_skill4Button->loadTextureNormal(skill4ImagePath.c_str());
	_skill4Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill4Button->setTag(TAG_SKILL_4);
	_skill4Button->setPosition(Vec2(_visibleSize.width / 2 + 1.5 * baseSize.width + 20, baseSize.height / 2 + baseMargin));
	addChild(_skill4Button);
	displaySkillMpInButton(_skill4Button, _playerSkills[1].mp_cost);

	_mainCharacterMpBar = Slider::create();
	_mainCharacterMpBar->loadBarTexture("image/screen/battle/mp_base.png");
	//_manaSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterMpBar->loadProgressBarTexture("image/screen/battle/mp.png");
	_mainCharacterMpBar->setPercent(100);
	_mainCharacterMpBar->setPosition(Vec2(_visibleSize.width / 2, baseSize.height + baseMargin * 2));
	addChild(_mainCharacterMpBar);

	_mpViewlabel = Label::create(DataUtils::numberToString(_mainCharacterData.mp), "", 20);
	_mpViewlabel->setColor(Color3B::BLUE);
	_mpViewlabel->setContentSize(Size(150, 50));
	_mpViewlabel->setPosition(Vec2(_mainCharacterMpBar->getContentSize()) + Vec2(50, 0));
	_mainCharacterMpBar->addChild(_mpViewlabel);

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
	_autoAttackArea->setVisible(false);

	float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
	float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);

	auto node = Node::create();
	node->setPosition(Vec2::ZERO);
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		auto sp = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
		sp->setPosition(Vec2(2 * _visibleSize.width*random(0.2f, 0.8f), 2 * _visibleSize.height*random(0.2f, 0.8f)));
		sp->setScale(IMAGE_SCALE);
		sp->setTag(i);

		auto hpB = Slider::create();
		hpB->loadBarTexture("image/screen/battle/mini_hp_base.png");
		hpB->setPercent(100);
		//hpB->loadSlidBallTextureNormal("image/screen/battle/test.png");
		hpB->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
		//_hpSlider->setContentSize(Size(372, 12));
		hpB->setPosition(Vec2(sp->getContentSize().width / 2, sp->getContentSize().height - 10));
		_allEnemyHpBar.push_back(hpB);
		sp->addChild(_allEnemyHpBar.back(), 100);
		_allEnemyUnitSprite.push_back(sp);
		_battleBackround->addChild(_allEnemyUnitSprite.back(), MID);

		auto enemyIcon = Sprite::create("image/screen/battle/enemyicon.png");
		enemyIcon->setOpacity(255);
		enemyIcon->setPosition(Vec2(sp->getPositionX()*positionXScaleRate, sp->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon);
		node->addChild(_allEnemyIconInMinimap.back(), 1);
		_allEnemyAttachDelay.push_back(false);
	}
	_miniMap->addChild(node, 1);
	/*_testAttackTarget = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
	_testAttackTarget->setPosition(_visibleSize);
	_testAttackTarget->setScale(IMAGE_SCALE);
	_battleBackround->addChild(_testAttackTarget);*/

	auto enemyIcon1 = Sprite::create("image/screen/battle/enemyicon.png");
	enemyIcon1->setOpacity(255);

	switch (_currentPlayerTeamFlg)
	{
	case TEAM_FLG_RED:
		_allAlliedUnitSprite.push_back(redTower);
		_allAlliedUnitHpBar.push_back(redTHpBar);
		_allAlliedUnitData.push_back(_redTeamTowerData);
		_allAlliedUnitCurrentHp.push_back(_redTeamTowerData.hp);

		_allEnemyAttachDelay.push_back(false);
		_allEnemyCurentHp.push_back(_blueTeamTowerData.hp);
		_allEnemyHpBar.push_back(blueTHpBar);
		_allEnemyUnitData.push_back(_blueTeamTowerData);
		_allEnemyUnitSprite.push_back(blueTower);

		
// 		float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
// 		float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
		enemyIcon1->setPosition(Vec2(blueTower->getPositionX()*positionXScaleRate, blueTower->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon1);
		node->addChild(_allEnemyIconInMinimap.back(), 1);

		break;
	case TEAM_FLG_BLUE:
		_allAlliedUnitSprite.push_back(blueTower);
		_allAlliedUnitHpBar.push_back(blueTHpBar);
		_allAlliedUnitData.push_back(_blueTeamTowerData);
		_allAlliedUnitCurrentHp.push_back(_blueTeamTowerData.hp);

		_allEnemyAttachDelay.push_back(false);
		_allEnemyCurentHp.push_back(_redTeamTowerData.hp);
		_allEnemyHpBar.push_back(redTHpBar);
		_allEnemyUnitData.push_back(_redTeamTowerData);
		_allEnemyUnitSprite.push_back(redTower);


		enemyIcon1->setPosition(Vec2(redTower->getPositionX()*positionXScaleRate, redTower->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon1);
		node->addChild(_allEnemyIconInMinimap.back(), 1);

		break;
	default:
		break;
	}


}

void BatleScene::displaySkillMpInButton(Button *parent, int mp)
{
	auto lb = Label::create(DataUtils::numberToString(mp), "", 25);
	lb->setColor(Color3B(0,0,243));
	lb->setPosition(Vec2(parent->getContentSize() / 2) - Vec2(0, 30));
	lb->setTag(TAG_MP_LABEL);
	parent->addChild(lb);
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
	topB->setPosition(Vec2(_visibleSize.width, _visibleSize.height * 2 + 50));
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
	auto wallBd = PhysicsBody::createBox(Size(_visibleSize.width * 2, 100), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;
}

Node* BatleScene::createVBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(100, _visibleSize.height * 2), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;

}


void BatleScene::onEnter()
{
	LayerBase::onEnter();
	srand(time(NULL));
	time(&timer);
	timeinfo = localtime(&timer);
	_characterCurentMp = _mainCharacterData.mp;
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		_allEnemyCurentHp.push_back(_allEnemyUnitData[i].hp);
		_allEnemyCurentMp.push_back(_allEnemyUnitData[i].mp);
	}

	scheduleUpdate();
}

void BatleScene::update(float delta)
{
	//log("Current Hp: %d", _characterCurentHp);
	updateMiniMap();
	updateTime();
	updateHpAndMpViewLabel();
	checkForAutoAttack();

}
void BatleScene::checkForAutoAttack()
{
	float area = IMAGE_SCALE*_autoAttackArea->getContentSize().width / 2 + 25;
	for (int i = 0; i < _allEnemyUnitSprite.size(); i++)
	{
		auto posDistan = _allEnemyUnitSprite[i]->getPosition() - testObject->getPosition();
		int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
		if (posDistan.length() < _mainCharacterData.attack_sight && _allEnemyUnitSprite[i]->isVisible()) {
			if (testObject->getActionByTag(_currentAttackActionTag) == nullptr && _onDelayAttackFlg == false) {
				//rotateCharacter(testObject,direc);
				auto ani = createAttackAnimationWithDefine(direc, _attackImagePath);
				//auto action = RepeatForever::create(Animate::create(ani)); //user repeat for change in future
				auto call1 = CallFuncN::create(CC_CALLBACK_0(BatleScene::characerAttackCallback, this));
				auto action = Sequence::create(Animate::create(ani),call1,nullptr);
				action->setTag(direc * 10);
				_currentAttackActionTag = direc * 10;
				testObject->runAction(action);
				_indexOfBeAttackEnemy = i;

				_onDelayAttackFlg = true;
				this->runAction(Sequence::create(DelayTime::create(_mainCharacterData.attack_delay), CallFuncN::create(CC_CALLBACK_0(BatleScene::removeceAttackDelayFlg, this)), nullptr));
			}
		}
		if (posDistan.length() < _allEnemyUnitData[i].attack_sight && _allEnemyUnitSprite[i]->isVisible()) {

			if (_allEnemyUnitSprite[i]->getNumberOfRunningActions() < 1 && _allEnemyAttachDelay[i] == false) {
				string path = "image/unit_new/attack/red/";
				auto target_ani = createAttackAnimationWithDefine(10 - direc, path);
				auto call2 = CallFuncN::create(CC_CALLBACK_1(BatleScene::enemyAttackCallback, this));
				auto action2 = Sequence::create(Animate::create(target_ani), call2, nullptr);

				auto forCallback = Sequence::create(DelayTime::create(_allEnemyUnitData[i].attack_delay), CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEnemyAttackDelayFlg, this)), nullptr);


				action2->setTag(1);
				//rotateCharacter(_alltargetUnit[i], 10 - direc);
				_allEnemyAttachDelay[i] = true;
				_allEnemyUnitSprite[i]->runAction(Spawn::create(action2, forCallback, nullptr));
				//_indexOfRunningActionTarget = i;
			}
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
	if (_allEnemyCurentHp[_indexOfBeAttackEnemy] > 0) {
		int dame = (_mainCharacterData.attack_dame - _allEnemyUnitData[_indexOfBeAttackEnemy].defence);
		if (dame <= 0) {
			dame = 1;
		}
		_allEnemyCurentHp[_indexOfBeAttackEnemy] -= dame;
		if (_allEnemyCurentHp[_indexOfBeAttackEnemy] <= 0)
		{
			enemyDieAction(_indexOfBeAttackEnemy);
			return;
		}
		_allEnemyHpBar[_indexOfBeAttackEnemy]->setPercent(ceil((_allEnemyCurentHp[_indexOfBeAttackEnemy] * 100.0f / _allEnemyUnitData[_indexOfBeAttackEnemy].hp)));
		showAttackDame(dame, _allEnemyUnitSprite[_indexOfBeAttackEnemy]->getPosition() + Vec2(0, 100),1);

	}
	else {
		enemyDieAction(_indexOfBeAttackEnemy);
	}
}

void BatleScene::enemyDieAction(int id)
{
	_allEnemyUnitSprite[id]->setVisible(false);
	_allEnemyIconInMinimap[id]->setVisible(false);
	_indexOfBeAttackEnemy = -1;
	testObject->stopActionByTag(_currentAttackActionTag);
	if (id == _allEnemyUnitData.size() - 1) {
		endBattle();
	}
}
void BatleScene::enemyAttackCallback(Ref *pSEnder)
{
	Sprite *_sprite = (Sprite*)pSEnder;
	_sprite->stopActionByTag(1);
	int id = _sprite->getTag();
	//log("enemy");
	if (_allAlliedUnitCurrentHp[0] > 0)
	{
		if (!_onRespwanFlg)
		{
			int dame = _allEnemyUnitData[id].attack_dame - _mainCharacterData.defence;
			if (dame <= 0)
			{
				dame = 1;
			}
			_allAlliedUnitCurrentHp[0] -= dame;
			if (_allAlliedUnitCurrentHp[0] <= 0) {
				runRespawnAction();
				return;
			}
			//log("Percent: %d", ceil(float(float(_characterCurentHp) / float(_unitData.hp)) * 100));
			_mainCharacterMiniHpBar->setPercent(ceil((_allAlliedUnitCurrentHp[0] * 100.0f / _mainCharacterData.hp)));
			_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());
			showAttackDame(dame, testObject->getPosition() + Vec2(0, 100),2);
		}
	}
	else {
		runRespawnAction();
	}
	

}
void BatleScene::removeEnemyAttackDelayFlg(Ref *pSender) {
	Sprite *_sprite = (Sprite*)pSender;
	int a = _sprite->getTag();
	if (a < _allEnemyAttachDelay.size()) {
		_allEnemyAttachDelay[a] = false;
	}


}
void BatleScene::showAttackDame(int dameValue, Vec2 pos,int type)
{
	if (dameValue < 0)
	{
		return;
	}
	auto action = Sequence::create(Spawn::create(ScaleBy::create(0.25f, 1.5), MoveBy::create(0.5f, Vec3(0, 100, 0)),nullptr), RemoveSelf::create(true), nullptr);
	//lab->runAction(action);

	auto s = LabelShow::createLabel(dameValue, type);
	s->setPosition(pos);
	_battleBackround->addChild(s,999);
	s->runAction(action);
}

void BatleScene::runRespawnAction()
{
	testObject->setPosition(Vec2(100, 100));
	_mainCharacterMiniHpBar->setPercent(100);
	_mainCharacterHpBar->setPercent(100);
	_mainCharacterMpBar->setPercent(100);
	_allAlliedUnitCurrentHp[0] = _mainCharacterData.hp;
	_characterCurentMp = _mainCharacterData.mp;
	auto action = Spawn::create(Blink::create(0.6, 6), Sequence::create(DelayTime::create(RESPAWN_DELAY), CallFuncN::create(CC_CALLBACK_0(BatleScene::removeReSpawnFlg, this)), nullptr), nullptr);
	testObject->runAction(action);
	_onRespwanFlg = true;
}

void BatleScene::removeceAttackDelayFlg() {
	testObject->stopActionByTag(_currentAttackActionTag);
	//_alltargetUnit[_indexOfBeAttackEnemy]->stopActionByTag(10 - (_currentAttackActionTag / 10));
	_onDelayAttackFlg = false;
}
void BatleScene::updateTime()
{
	time_t currTimer;
	time(&currTimer);

	int seconds = ceil(difftime(currTimer, timer));
	if (seconds != _oldSecond && (seconds % 5 == 0)) {
		_oldSecond = seconds;
		/*log("CALL");*/
		autoRestoreHpAndMp();
	}
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
		_mainCharacterAvata->setRotation(-(dis.getAngle() * RAD_DEG) + 90);
		int direc = detectDirectionBaseOnTouchAngle(_mainCharacterAvata->getRotation());
		if (direc == 0) return;
		rotateCharacter(testObject, direc);
		return;
	}
	fakeZOrder();
	auto distanVector = touch->getLocation() - _touchStartPoint;
	if (distanVector.length() < 200) {
		_touchMoveEndSprite->setPosition(touch->getLocation());
	}
	else {
		_touchMoveEndSprite->setPosition(_touchStartPoint +Vec2(200*cos(distanVector.getAngle()), 200*sin(distanVector.getAngle())));
	}
	_touchMoveEndSprite->setVisible(true);
	

	if (distanVector.length() < _touchMoveBeginSprite->getContentSize().width / 6) {
		testObject->getPhysicsBody()->setVelocity(Vect(0, 0));
		testObject->stopActionByTag(_currentMoveActionTag);
		return;
	}

	testObject->stopActionByTag(_currentAttackActionTag);
	testObject->getPhysicsBody()->setVelocity(Vect(_mainCharacterData.move_speed*cos(distanVector.getAngle()),_mainCharacterData.move_speed*sin(distanVector.getAngle())));
	_mainCharacterAvata->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
	//log("%f", _mini_Icon->getRotation());
	int direc = detectDirectionBaseOnTouchAngle(_mainCharacterAvata->getRotation());
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
		auto dialog = OptionDialog::create(CC_CALLBACK_2(BatleScene::optionDecideCallback, this), CC_CALLBACK_2(BatleScene::optionCancelCallback, this));
		getParent()->addChild(dialog);
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
		sp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		
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
		if (stone->getPositionY() - stone->getContentSize().height/2 < testObject->getPositionY() - testObject->getContentSize().height/2)
		{
			stone->setLocalZOrder(HIGH);
		}
		else {
			stone->setLocalZOrder(LOW);
		}
	}
	for (auto &enemy : _allEnemyUnitSprite)
	{
		if (enemy->getPositionY() > testObject->getPositionY()) {
			enemy->setLocalZOrder(LOW);
		}
		else {
			enemy->setLocalZOrder(HIGH);
		}

	}
}

void BatleScene::onPhysicContactBegin(const PhysicsContact &contact)
{

}

void BatleScene::runAttackAnimation()
{
	int direc = detectDirectionBaseOnTouchAngle(_mainCharacterAvata->getRotation());
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
	//log("Rotate direc %d", direc);
	auto animation = Animation::create();
	char szName[100] = { 0 };
	sprintf(szName, "unit_00_0%d_attack_%d.png", direc, 1);
	string p = _attackImagePath;
	p.append(szName);
	//log("%s", p.c_str());
	animation->addSpriteFrameWithFile(p.c_str());
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	target->runAction(Animate::create(animation));
}

void BatleScene::optionDecideCallback(Ref *pSEnder, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_moveMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::optionCancelCallback(Ref *pSEnder, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//_moveMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::removeReSpawnFlg()
{
	testObject->setVisible(true);
	_onRespwanFlg = false;
}

void BatleScene::debugPhysicButtonCallback(Ref *pSEnder, Widget::TouchEventType type)
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
			_autoAttackArea->setVisible(false);
		}
		else {
			_myWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
			_autoAttackArea->setVisible(true);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleScene::changeImageButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		changeImagePathforTest();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
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
		endBattle();
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}
UnitInforNew BatleScene::getUnitDataFromDataBase(int unitId)
{

	return UnitData::getUnitDataById(unitId);

}

vector<SkillInfoNew> BatleScene::getUnitSkillFromDataBase(int unitId)
{
	return SkillData::getUnitSkillsByUnitId(unitId);
}

vector<UnitInforNew> BatleScene::getEnemyUnitsData(vector<int> enemyIdList)
{
	vector<UnitInforNew> returnData;
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		returnData.push_back(UnitData::getUnitDataById(2));
	}
	

	return returnData;
}

void BatleScene::autoRestoreHpAndMp()
{ 
	_allAlliedUnitCurrentHp[0] += _mainCharacterData.hp_restore / RESTORE_MULTI;
	if (_allAlliedUnitCurrentHp[0] > _mainCharacterData.hp) {
		_allAlliedUnitCurrentHp[0] = _mainCharacterData.hp;
	}
	_characterCurentMp += _mainCharacterData.mp_restore/RESTORE_MULTI;
	if (_characterCurentMp > _mainCharacterData.mp)
	{
		_characterCurentMp = _mainCharacterData.mp;
	}

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		_allEnemyCurentHp[i] += _allEnemyUnitData[i].hp_restore/RESTORE_MULTI;
		if (_allEnemyCurentHp[i] > _allEnemyUnitData[i].hp) {
			_allEnemyCurentHp[i] = _allEnemyUnitData[i].hp;
		}
	}
	updateSlider();
}

void BatleScene::updateSlider()
{
	_mainCharacterMiniHpBar->setPercent(ceil((_allAlliedUnitCurrentHp[0] * 100.0f / _mainCharacterData.hp)));
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());

	_mainCharacterMpBar->setPercent(ceil((_characterCurentMp * 100.0f / _mainCharacterData.mp)));

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		_allEnemyHpBar[i]->setPercent(ceil((_allEnemyCurentHp[i] * 100.0f / _allEnemyUnitData[i].hp)));
	}
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
		//testObject->stopActionByTag(_currentMoveActionTag);
		//selectAttackTarget();
		
		Button* bt = dynamic_cast<Button*>(pSender);
		int tag = bt->getTag();
		//Progress Timer

		SkillInfoNew skill;
		if (tag == TAG_SKILL_1 || tag == TAG_SKILL_2)
		{
			skill = _mainCharacterSkillData[tag - 1];
		}
		else {
			skill = _playerSkills[tag - 3];
		}

		if (skill.target_type == TARGET_ONE && skill.skill_type == TYPE_ATTACK && _indexOfBeAttackEnemy < 0) {
			log("Invalid attack target");
			return;
		}
		if (skill.mp_cost <= _characterCurentMp){
			bt->setColor(Color3B::GRAY);
			bt->setTouchEnabled(false);
			bt->setEnabled(true);
			playSkill(skill);
			showCoolDown(bt, skill.cooldown);
			bt->runAction(Sequence::create(DelayTime::create(skill.cooldown), CallFuncN::create([&, tag](Ref *p) {
				removeSkillDisableFlg(tag);
			}), nullptr));
			
		}
		else {
			log("Not enough MP");
		}
		
		break; }
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
/*
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
		//testObject->stopActionByTag(_currentMoveActionTag);
		//selectAttackTarget();
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
		//testObject->stopActionByTag(_currentMoveActionTag);

		//selectAttackTarget();
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
		//testObject->stopActionByTag(_currentMoveActionTag);
		//selectAttackTarget();
		break; }

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
*/
void BatleScene::showCoolDown(Button *parentButton, int time)
{
	auto secondLabel = Label::create(DataUtils::numberToString(time), "", 40);
	secondLabel->setColor(Color3B::RED);
	secondLabel->setPosition(parentButton->getContentSize() / 2);
	parentButton->addChild(secondLabel);
	auto action = Repeat::create(Sequence::create(DelayTime::create(1), CallFuncN::create([&](Ref *pSender){
		Label* lb = dynamic_cast<Label*>(pSender);
		int t = DataUtils::stringToFloat(lb->getString());
		lb->setString(DataUtils::numberToString(t - 1));
	}),nullptr),time-1);
	auto action2 = Sequence::create(DelayTime::create(time), CallFuncN::create([&](Ref *pSEnder){
		Label* lb = dynamic_cast<Label*>(pSEnder);
		lb->removeFromParentAndCleanup(true);
	}), nullptr);
	secondLabel->runAction(Spawn::create(action, action2, nullptr));
}

void BatleScene::playSkill(SkillInfoNew skillData)
{
	SkillInfoNew skill = skillData;
	_characterCurentMp -= skill.mp_cost;
	_mainCharacterMpBar->setPercent(ceil(_characterCurentMp*100.0f / _mainCharacterData.mp));
	switch (skill.skill_type)
	{
	case TYPE_HELP:
		skillHelpAction(skill);
		break;
	case TYPE_RESTORE:
		skillRestoreAction(skill);
		break;
	case TYPE_ATTACK:
		skillAttackAction(skill);
		break;
	default:
		break;
	}

}

void BatleScene::removeSkillDisableFlg(int skillnum)
{
	switch (skillnum)
	{
	case TAG_SKILL_1:
		_skill1Button->setEnabled(true);
		_skill1Button->setTouchEnabled(true);
		_skill1Button->setColor(Color3B::WHITE);
		break;
	case TAG_SKILL_2:
		_skill2Button->setEnabled(true);
		_skill2Button->setTouchEnabled(true);
		_skill2Button->setColor(Color3B::WHITE);
		break;
	case TAG_SKILL_3:
		_skill3Button->setEnabled(true);
		_skill3Button->setTouchEnabled(true);
		_skill3Button->setColor(Color3B::WHITE);
		break;
	case TAG_SKILL_4:
		_skill4Button->setEnabled(true);
		_skill4Button->setTouchEnabled(true);
		_skill4Button->setColor(Color3B::WHITE);
		break;
	default:
		break;
	}
}

void BatleScene::skillRestoreAction(SkillInfoNew skillInfo)
{
	switch (skillInfo.target_type)
	{
	case TARGET_ALL:
		skillRestoreAll(skillInfo);
		break;
	case TARGET_ONE:
		skillRestoreOne(skillInfo);
		break;
	default:
		break;
	}


}

void BatleScene::skillHelpAction(SkillInfoNew skillInfo)
{
	switch (skillInfo.target_type)
	{
	case TARGET_ALL :
		skillHelpAll(skillInfo);
		break;
	case TARGET_ONE:
		skillHelpOne(skillInfo);
		break;
	default:
		break;
	}
}

void BatleScene::skillAttackAction(SkillInfoNew skillInfo)
{
	switch (skillInfo.target_type)
	{
	case TARGET_ALL:
		skillAttackAll(skillInfo);
		break;
	case TARGET_ONE:
		skillAttackOne(skillInfo);
	default:
		break;
	}
}

void BatleScene::skillRestoreAll(SkillInfoNew skillInfo)
{
	int value = 0;
	switch (skillInfo.dame_type)
	{
	case DAME_TYPE_PERCENT:
		value = ceil(1.0f*_mainCharacterData.hp *skillInfo.dame_value/100.0f);
		break;
	case DAME_TYPE_PURE:
		value = ceil(skillInfo.dame_value);
		break;
	}

	if (skillInfo.aoe > 0) {
		vector<int> allUnitIndex = detectUnitInAoe(skillInfo.aoe, ALLIED_FLAG);
		for (int &index : allUnitIndex)
		{
			_allAlliedUnitCurrentHp[index] += value;
			if (_allAlliedUnitCurrentHp[index] > _allAlliedUnitData[index].hp) {
				_allAlliedUnitCurrentHp[index] = _allAlliedUnitData[index].hp;
			}
			_allAlliedUnitHpBar[index]->setPercent(_allAlliedUnitCurrentHp[index] * 100.0f / _allAlliedUnitData[index].hp);
		}

		//RUN EFFECT AOE
	}
	else {
		for (int i = 0; i < _allAlliedUnitData.size(); i++)
		{
			_allAlliedUnitCurrentHp[i] += value;
			if (_allAlliedUnitCurrentHp[i] > _allAlliedUnitData[i].hp) {
				_allAlliedUnitCurrentHp[i] = _allAlliedUnitData[i].hp;
			}
			_allAlliedUnitHpBar[i]->setPercent(_allAlliedUnitCurrentHp[i] * 100.0f / _allAlliedUnitData[i].hp);

		}

		//RUN EFFECT HEAL ALL
		//_mainCharacterMiniHpBar->setPercent(_allAlliedUnitCurrentHp[0] * 100.0f / _mainCharacterData.hp);
	}


}

void BatleScene::skillRestoreOne(SkillInfoNew skillInfo)
{
	int value = 0;
	switch (skillInfo.dame_type)
	{
	case DAME_TYPE_PERCENT:
		value = ceil(1.0f*_mainCharacterData.hp *skillInfo.dame_value/100.0f);
		break;
	case DAME_TYPE_PURE:
		value = ceil(skillInfo.dame_value);
		break;
	}
	_allAlliedUnitCurrentHp[0] += value;
	if (_allAlliedUnitCurrentHp[0] > _mainCharacterData.hp) {
		_allAlliedUnitCurrentHp[0] = _mainCharacterData.hp;
	}
	_mainCharacterMiniHpBar->setPercent(_allAlliedUnitCurrentHp[0] * 100.0f / _mainCharacterData.hp);
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());
	//Run Effect heal one
}

void BatleScene::skillHelpAll(SkillInfoNew skillInfo)
{
	int value = 0;
}

void BatleScene::skillHelpOne(SkillInfoNew skillInfo)
{
	float value = 1.0f;
	switch (skillInfo.dame_type)
	{
	case DAME_TYPE_PERCENT:
		value = ceil(skillInfo.dame_value / 100.0f);
		break;
	case DAME_TYPE_PURE:
		break;
	default:
		break;
	}
	int saveValue = 0;
	switch (skillInfo.skill_help_type)
	{
	case SKILL_HELP_TYPE::HP:
		saveValue = _mainCharacterData.hp;
		_mainCharacterData.hp = ceil(1.0f*_mainCharacterData.hp * value);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.hp = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::HP_RESTORE:
		saveValue = _mainCharacterData.hp_restore;
		_mainCharacterData.hp_restore = ceil(1.0f*value*_mainCharacterData.hp_restore);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.hp_restore = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::MP:
		saveValue = _mainCharacterData.mp;
		_mainCharacterData.mp = ceil(1.0f*value*_mainCharacterData.mp);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.mp = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::MP_RESTORE:
		saveValue = _mainCharacterData.mp_restore;
		_mainCharacterData.mp_restore = ceil(1.0f*value*_mainCharacterData.mp_restore);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.mp_restore = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::ATTACK_DAME:
		saveValue = _mainCharacterData.attack_dame;
		_mainCharacterData.attack_dame = ceil(1.0f*value*_mainCharacterData.attack_dame);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.attack_dame = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::DEFENCE :
		log("increase defence by %f",value);
		saveValue = _mainCharacterData.defence;
		_mainCharacterData.defence = ceil(1.0f*value*_mainCharacterData.defence);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.defence = saveValue;
			log("remove defence buff");
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::ATTACK_RANGE:
		saveValue = _mainCharacterData.attack_sight;
		_mainCharacterData.attack_sight = ceil(1.0f*value*_mainCharacterData.attack_sight);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.attack_sight = saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::MOVESPEED:
		saveValue = _mainCharacterData.move_speed;
		_mainCharacterData.move_speed = ceil(1.0f *value*_mainCharacterData.move_speed);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.move_speed = saveValue;
		}), nullptr));
		break;
	default:
		break;
	}


}

void BatleScene::skillAttackAll(SkillInfoNew skillInfo)
{
	int value = 0;
	switch (skillInfo.dame_type)
	{
	case DAME_TYPE_PERCENT:
		value = _mainCharacterData.attack_dame*100.0f / skillInfo.dame_value;
		break;
	case DAME_TYPE_PURE:
		value = skillInfo.dame_value;
	default:
		break;
	}
	if (skillInfo.aoe > 0) {
		vector<int> unitIndex = detectUnitInAoe(skillInfo.aoe, ENEMY_FLAG);
		for (int &index : unitIndex)
		{
			int dame = (value - _allEnemyUnitData[index].defence);
			if (dame <= 0) dame = 1;
			_allEnemyCurentHp[index] -= dame;
			if (_allEnemyCurentHp[index] <= 0) {
				enemyDieAction(index);
				return;
			}
			if (_allEnemyUnitSprite[index]->isVisible())
			{
				_allEnemyHpBar[index]->setPercent(_allEnemyCurentHp[index] * 100.0f / _allEnemyUnitData[index].hp);
				showAttackDame(dame, _allEnemyUnitSprite[index]->getPosition() + Vec2(0, 100), 1);
			}
			//RUN EFFECET AOE
		}
	}
	else {
		for (int i = 0; i < _allEnemyUnitData.size(); i++)
		{
			int dame = (value - _allEnemyUnitData[i].defence);
			if (dame <= 0) dame = 1;
			_allEnemyCurentHp[i] -= dame;
			if (_allEnemyCurentHp[i] <= 0) {
				enemyDieAction(i);
				return;
			}
			if (_allEnemyUnitSprite[i]->isVisible())
			{
				_allEnemyHpBar[i]->setPercent(_allEnemyCurentHp[i] * 100.0f / _allEnemyUnitData[i].hp);
				showAttackDame(dame, _allEnemyUnitSprite[i]->getPosition() + Vec2(0, 100), 1);
			}
		}
		//RUN EFFECT ATTACK ALL
	}
	
	
}

void BatleScene::skillAttackOne(SkillInfoNew skillInfo)
{
	int value = 0;
	switch (skillInfo.dame_type)
	{
	case DAME_TYPE_PERCENT:
		value = _mainCharacterData.attack_dame*100.0f / skillInfo.dame_value;
		break;
	case DAME_TYPE_PURE:
		value = skillInfo.dame_value;
	default:
		break;
	}
	int dame = (value - _allEnemyUnitData[_indexOfBeAttackEnemy].defence);
	if (dame <= 0) dame = 1;
	_allEnemyCurentHp[_indexOfBeAttackEnemy] -= dame;
	if (_allEnemyCurentHp[_indexOfBeAttackEnemy] <= 0)
	{
		enemyDieAction(_indexOfBeAttackEnemy);
		return;
	}
	_allEnemyHpBar[_indexOfBeAttackEnemy]->setPercent(_allEnemyCurentHp[_indexOfBeAttackEnemy] * 100.0f / _allEnemyUnitData[_indexOfBeAttackEnemy].hp);
	showAttackDame(dame, _allEnemyUnitSprite[_indexOfBeAttackEnemy]->getPosition() + Vec2(0, 100), 1);
	//RUN EFFECT ATTACK ONE UNIT 
}

vector<int> BatleScene::detectUnitInAoe(float detectAoe, int unitFlg)
{
	vector<int> resultUnitId;
	vector<Sprite*> allUnit;
	switch (unitFlg)
	{
	case ENEMY_FLAG:
		allUnit = _allEnemyUnitSprite;
		break;
	case ALLIED_FLAG:
		allUnit = _allAlliedUnitSprite;
		break;
	default:
		break;
	}

	for (int i = 0; i < allUnit.size(); i++)
	{
		Vec2 distan = allUnit[i]->getPosition() - testObject->getPosition();
		if (distan.length() < detectAoe) {
			resultUnitId.push_back(i);
		}
	}

	return resultUnitId;
}

void BatleScene::updateHpAndMpViewLabel()
{
	string hp = DataUtils::numberToString(_allAlliedUnitCurrentHp[0]).append("/").append(DataUtils::numberToString(_allAlliedUnitData[0].hp));
	_hpViewLabel->setString(hp.c_str());
	string mp = DataUtils::numberToString(_characterCurentMp).append("/").append(DataUtils::numberToString(_allAlliedUnitData[0].mp));
	_mpViewlabel->setString(mp.c_str());
}

void BatleScene::endBattle()
{
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene()));
}






























