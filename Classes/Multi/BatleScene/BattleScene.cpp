﻿#include "BattleScene.h"


Scene* BattleScene::createScene()
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setGravity(Vect::ZERO);
	auto lay = BattleScene::create();
	lay->savePhysicWorld(scene->getPhysicsWorld());
	scene->addChild(lay);
	return scene;
}

BattleScene* BattleScene::create()
{
	BattleScene *layer = new BattleScene();
	if (layer && layer->init()) {
		layer->autorelease();
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}

bool BattleScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	_gameMode = UserDefault::getInstance()->getIntegerForKey("MODE");
	UserDefault::getInstance()->setIntegerForKey(SCR_ORIEN_K, SCREEN_VERTICAL);
	_menu->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_onDestructCalled = false;
	_selectedUnitId = UserModel::getInstance()->getSelectedUnitId();
	setUnitStatus(1);
	setCannonFlg(false);
	if (_gameMode == MULTI_MODE)
	{
		for (auto &sk : BattleModel::getInstance()->getPlayerSkills())
		{
			_playerSkills.push_back(UserSkillModel::getInstance()->getSkillInfoById(sk));
		}
	}
	else {
		_playerSkills = UserModel::getInstance()->getSelectedSkillList();
	}
	
	///INIT DATA FOR ALL UNIT IN BATTLE
	auto _mainCharacterData = getUnitDataFromDataBase(_selectedUnitId);
	_mainCharacterData.uuid = UserModel::getInstance()->getUuId();

	_mainCharacterSkillData = getUnitSkillFromDataBase(_mainCharacterData);
	_allAlliedUnitData.push_back(_mainCharacterData);
	_saveMainStatusData = _mainCharacterData;
	_mainCharacerMaxMp = _mainCharacterData.mp;
	if (_gameMode == MULTI_MODE)
	{
		//Bellow vector is store  list of enemy unit
		vector<Room_User_Unit_Model> a;
		//Bellow vector is store list of allied unit
		vector<Room_User_Unit_Model> b;

		auto listUser = BattleModel::getInstance()->getRoomUserList();
		//list of all unit that join in the battle
		auto listUserUnit = BattleModel::getInstance()->getRoomUserUnitList();
		int currentTeamId = UserModel::getInstance()->getUserInfo().team_id;
		if (currentTeamId == TEAM_FLG_BLUE) {
			_currentPlayerTeamFlg = TEAM_FLG_BLUE;
			_currentEnemyTeamFlg = TEAM_FLG_RED;
		}
		else {
			_currentPlayerTeamFlg = TEAM_FLG_RED;
			_currentEnemyTeamFlg = TEAM_FLG_BLUE;
		}

		for (auto &userUnit : listUserUnit)
		{
			if (userUnit.team_id != currentTeamId)
			{
				a.push_back(userUnit);

			}
			else {
				if (strcmp(userUnit.uuid.c_str(), UserModel::getInstance()->getUuId().c_str()) != 0)
				{
					b.push_back(userUnit);
				}
			}
		}
		_allEnemyUnitData = getUnitsDataListByUnitIdIdList(a);

		//first element of allied unit list is the main character info
		

		for (auto &alliedUnit : getUnitsDataListByUnitIdIdList(b))
		{
			_allAlliedUnitData.push_back(alliedUnit);
		}
	}
	else
	{
		_currentPlayerTeamFlg = TEAM_FLG_BLUE;
		_currentEnemyTeamFlg = TEAM_FLG_RED;
	}

	//For test
	_redTeamTowerData = UserUnitModel::getInstance()->getUnitInfoById(1);
	_redTeamTowerData.hp = 5000;
	_redTeamTowerData.attack_range = 200;
	_redTeamTowerData.attack_speed = 5;
	_redTeamTowerData.uuid = "redTower";
	if (_gameMode == SOLO_MODE) {
		_redTeamTowerData.attack = 1000;
	}

	_blueTeamTowerData = UserUnitModel::getInstance()->getUnitInfoById(2);

	_blueTeamTowerData.hp = 5000;
	_blueTeamTowerData.attack_range = 200;
	_blueTeamTowerData.attack_speed = 5;
	_blueTeamTowerData.uuid = "blueTower";

	changeAnimationImagePathByUnitId(_selectedUnitId);

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(BattleScene::nextButtonCallback, this));
	addChild(nextButton, 10);

	auto physicDebug = Button::create();
	physicDebug->loadTextureNormal("CloseNormal.png");
	physicDebug->setPosition(Vec2(_visibleSize.width - 100, 70));
	physicDebug->setTouchEnabled(true);
	physicDebug->addTouchEventListener(CC_CALLBACK_2(BattleScene::debugPhysicButtonCallback, this));
	addChild(physicDebug, 10);

	auto checkMaptest = Button::create();
	checkMaptest->loadTextureNormal("CloseNormal.png");
	checkMaptest->setPosition(Vec2(_visibleSize.width - 150, 70));
	checkMaptest->setTouchEnabled(true);
	checkMaptest->addTouchEventListener(CC_CALLBACK_2(BattleScene::checkMapTestButtonClick, this));
	addChild(checkMaptest, 10);

	createContent();

	if (UserDefault::getInstance()->getIntegerForKey(MOVE_KEY) == 0) {
		UserDefault::getInstance()->setIntegerForKey(MOVE_KEY, MOVE_AUTO);
		_moveMode = MOVE_AUTO;
	}
	else {
		_moveMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);

		if (_moveMode == MOVE_CIRCLE) {
			_circleType = UserDefault::getInstance()->getIntegerForKey(MOVE_CIRCLE_TYPE);
			_circleProperty = UserDefault::getInstance()->getIntegerForKey(NOVE_CIRCLE_PROPERTY);
			if (_circleType != 0 && _circleProperty != 0) {
				if (_checkMiniCircleFlg == true && _miniCircle != nullptr) {
					_miniCircle->removeFromParentAndCleanup(true);
					_miniUnit->removeFromParentAndCleanup(true);
					_checkMiniCircleFlg = false;
				}

				createMiniControlUnit(_circleType);
			}

		}
	}
	testObject->setMoveMode(_moveMode);
	/*size - 1 because we don't save dame dead by tower*/
	for (int i = 0; i < _allAlliedUnitData.size() - 1; i++)
	{
		UserBattleInfo info;
		info.name = _allAlliedUnitData[i].name;
		info.unitId = _allAlliedUnitData[i].mst_unit_id;
		info.imagePath = UserUnitModel::getInstance()->getUnitImageByMstUnitItD(_allAlliedUnitData[i].mst_unit_id);
		//_saveBattleInfoAlliedTeam.push_back(info);
	}
	/*Size - 1 because we don't save dame dead by tower*/
	for (int i = 0; i < _allEnemyUnitData.size() - 1; i++)
	{
		UserBattleInfo enemy;
		enemy.name = _allEnemyUnitData[i].name;
		enemy.unitId = _allEnemyUnitData[i].mst_unit_id;
		//_saveBattleInfoEnemyTeam.push_back(enemy);
	}
	auto folow = Follow::create(testObject, Rect(FOLLOW_MARGIN,0,_myMap->getContentSize().width - FOLLOW_MARGIN, _myMap->getContentSize().height));
	folow->setTag(121);
	_battleBackround->runAction(folow);
	auto bolder = Node::create();
	/*Scene main schedule update init*/
	//setScale(0.75f);
	
	scheduleUpdate();

	if (_gameMode == SOLO_MODE) {
		runAction(RepeatForever::create(Sequence::create(DelayTime::create(1.0f), CallFuncN::create(CC_CALLBACK_0(BattleScene::neutralUnitMoveInSoloMod,this)),nullptr)));
	}

	return true;
}

void BattleScene::createWormHole()
{
	auto redHole = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/wormhole_red.png");
	auto blueHole = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/wormhole_blue.png");
	auto action = RepeatForever::create(RotateBy::create(1, 135));
	for (int i = 0; i < 2; i++)
	{
		/*Create red wormHole*/
		auto parentNode1 = Node::create();
		parentNode1->setScaleY(0.75f);
		parentNode1->setPosition((Vec2(_myMap->getBoundingBox().size) - Vec2(1200, 200))*i + Vec2(600, 100));
		_battleBackround->addChild(parentNode1);
		auto redHoleSprite = Sprite::createWithTexture(redHole);
		redHoleSprite->setPosition(Vec2::ZERO);
		redHoleSprite->setName("OPEN");
		auto action1 = action->clone();
		action1->setTag(WORMHOLDROTATE);
		redHoleSprite->runAction(action1);
		_wormHoleList.push_back(redHoleSprite);
		parentNode1->addChild(redHoleSprite);

		/*Create blue wormHole*/
		auto blueHoleSprite = Sprite::createWithTexture(blueHole);
		blueHoleSprite->setPosition(Vec2::ZERO);
		blueHoleSprite->setName("OPEN");
		auto action2 = action->clone();
		action2->setTag(WORMHOLDROTATE);
		blueHoleSprite->runAction(action2);
		
		_wormHoleList.push_back(blueHoleSprite);
		auto parentNode2 = Node::create();
		parentNode2->setScaleY(0.75f);
		parentNode2->setPosition(Vec2((i + 1) % 2 * (_myMap->getBoundingBox().size.width - 1200) + 600, (i % 2) * (_myMap->getBoundingBox().size.height - 200) + 100));
		parentNode2->addChild(blueHoleSprite);
		_battleBackround->addChild(parentNode2);
	}
}

void BattleScene::createBoudingWall()
{
	/*Create bounding wall*/
	auto wallTexture = Director::getInstance()->getTextureCache()->addImage("wall.png");
	auto wall1 = Sprite::createWithTexture(wallTexture);
	auto wallBd = PhysicsBody::createBox(wall1->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd->setDynamic(false);
	wallBd->setCollisionBitmask(0x1111);
	wallBd->setContactTestBitmask(0x1111);
	wall1->setPhysicsBody(wallBd);
	wall1->setTag(BOUND_BORDER_TAG);
	wall1->setScaleX((3 * _myMap->getContentSize().width / 5) / wallTexture->getContentSize().width);
	wall1->setPosition(Vec2(_myMap->getContentSize().width / 2, 2 * _myMap->getContentSize().height / 3));
	_battleBackround->addChild(wall1);

	auto wall2 = Sprite::createWithTexture(wallTexture);
	auto wallBd2 = PhysicsBody::createBox(wall2->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd2->setDynamic(false);
	wallBd2->setCollisionBitmask(0x1111);
	wallBd2->setContactTestBitmask(0x1111);
	wall2->setPhysicsBody(wallBd2);
	wall2->setTag(BOUND_BORDER_TAG);
	wall2->setScaleX((3 * _myMap->getContentSize().width / 5) / wallTexture->getContentSize().width);
	wall2->setPosition(Vec2(_myMap->getContentSize().width / 2, _myMap->getContentSize().height / 3));
	_battleBackround->addChild(wall2);

	auto textTureSmall = Director::getInstance()->getTextureCache()->addImage("wall_s.png");

	auto wall3 = Sprite::createWithTexture(textTureSmall);
	auto wallBd3 = PhysicsBody::createBox(wall3->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd3->setDynamic(false);
	wallBd3->setCollisionBitmask(0x1111);
	wallBd3->setContactTestBitmask(0x1111);
	wall3->setPhysicsBody(wallBd3);
	wall3->setTag(BOUND_BORDER_TAG);
	wall3->setScaleY(1.5f);
	wall3->setPosition(Vec2(_myMap->getContentSize().width / 5, _myMap->getContentSize().height / 3));
	_battleBackround->addChild(wall3);

	auto wall4 = Sprite::createWithTexture(textTureSmall);
	auto wallBd4 = PhysicsBody::createBox(wall4->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd4->setDynamic(false);
	wallBd4->setCollisionBitmask(0x1111);
	wallBd4->setContactTestBitmask(0x1111);
	wall4->setPhysicsBody(wallBd4);
	wall4->setTag(BOUND_BORDER_TAG);
	wall4->setScaleY(1.5f);
	wall4->setPosition(Vec2(_myMap->getContentSize().width * 4 / 5, _myMap->getContentSize().height * 2 / 3));
	_battleBackround->addChild(wall4);


	auto wall5 = Sprite::createWithTexture(textTureSmall);
	auto wallBd5 = PhysicsBody::createBox(wall5->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd5->setDynamic(false);
	wallBd5->setCollisionBitmask(0x1111);
	wallBd5->setContactTestBitmask(0x1111);
	wall5->setPhysicsBody(wallBd5);
	wall5->setTag(BOUND_BORDER_TAG);
	//wall5->setScaleX(0.5f);
	wall5->setPosition(Vec2(_myMap->getContentSize().width * 2 / 5, _myMap->getContentSize().height * 5 / 9));
	_battleBackround->addChild(wall5);

	auto wall6 = Sprite::createWithTexture(textTureSmall);
	auto wallBd6 = PhysicsBody::createBox(wall6->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd6->setDynamic(false);
	wallBd6->setCollisionBitmask(0x1111);
	wallBd6->setContactTestBitmask(0x1111);
	wall6->setPhysicsBody(wallBd6);
	wall6->setTag(BOUND_BORDER_TAG);
	//wall6->setScaleX(0.5f);
	wall6->setPosition(Vec2(_myMap->getContentSize().width * 3 / 5, _myMap->getContentSize().height * 4 / 9));
	_battleBackround->addChild(wall6);


	auto wall7 = Sprite::createWithTexture(textTureSmall);
	auto wallBd7 = PhysicsBody::createBox(wall7->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd7->setDynamic(false);
	wallBd7->setCollisionBitmask(0x1111);
	wallBd7->setContactTestBitmask(0x1111);
	wall7->setPhysicsBody(wallBd7);
	wall7->setTag(BOUND_BORDER_TAG);
	wall7->setScaleY(0.8f);
	wall7->setPosition(Vec2(_myMap->getContentSize().width * 1 / 5, _myMap->getContentSize().height - wall7->getBoundingBox().size.height / 2));
	_battleBackround->addChild(wall7);


	auto wall8 = Sprite::createWithTexture(textTureSmall);
	auto wallBd8 = PhysicsBody::createBox(wall8->getContentSize(), PhysicsMaterial(1, 1, 0));
	wallBd8->setDynamic(false);
	wallBd8->setCollisionBitmask(0x1111);
	wallBd8->setContactTestBitmask(0x1111);
	wall8->setPhysicsBody(wallBd8);
	wall8->setTag(BOUND_BORDER_TAG);
	wall8->setScaleY(0.8f);
	wall8->setPosition(Vec2(_myMap->getContentSize().width * 4 / 5, wall8->getBoundingBox().size.height / 2));
	_battleBackround->addChild(wall8);
}

void BattleScene::createNeutralTower()
{
	
	/*Create test neutral tower*/
	for (int i = 1; i < 6; i++)
	{
		auto node = Node::create();
		_battleBackround->addChild(node, 100);
		auto neutralTower1 = Tower::createTower(0);
		neutralTower1->setTag(0);
		
		MyBodyParser::getInstance()->parseJsonFile("json/tower.json");
		PhysicsBody* body = MyBodyParser::getInstance()->bodyFormJson(neutralTower1, "tower");
		body->setDynamic(false);
		body->setGravityEnable(false);
		body->setContactTestBitmask(0x1);
		neutralTower1->setPhysicsBody(body);

		_neutralTowerList.push_back(neutralTower1);
		neutralTower1->setName("NEUTRAL");
		if (i == 5) {
			node->setPosition(getPositionForTitleCoord(Vec2(35,25)));
		}
		else {
			node->setPosition(Vec2((1 + ((i - 1) % 2))*_myMap->getBoundingBox().size.width / 3, _myMap->getBoundingBox().size.height *((i / 3) * 4 + 1) / 6));
		}
		node->addChild(neutralTower1,1);
		auto posCoor = getTitleCoorForPosition(neutralTower1->getParent()->getPosition());
		log("Tower %d in pos: %f, %f", i, posCoor.x, posCoor.y);
		drawAreaRectangle(neutralTower1, 8);
		if (_screenMode == SCREEN_VERTICAL) {
			node->setRotation(-90.0f);
			neutralTower1->setPosition(Vec2(-40, 35));

		}
		else {
			node->setRotation(0);
			neutralTower1->setPosition(Vec2(-15, 5));

		}
		
	}
	log("neutral tower done");
}

void BattleScene::drawAreaRectangle(Sprite* tower, int offset)
{
	auto towerCoor = getTitleCoorForPosition(tower->getParent()->getPosition());
	Size titleSize = _myMap->getTileSize();
	Vec2 botL = towerCoor + Vec2(-offset/2, offset/2);
	Vec2 beginPos = getPositionForTitleCoord(botL) - Vec2(0,titleSize.height);
	if (_neutralTowerList.size() == 5) {
		beginPos += Vec2(0, titleSize.height);
	}
	Vec2 endPos = beginPos + offset * titleSize;
	DrawNode *draw = DrawNode::create();
	draw->drawRect(beginPos, endPos, Color4F::BLACK);
	_battleBackround->addChild(draw, tower->getLocalZOrder()-1);
}

void BattleScene::drawRectInMinimap(Vec2 beginTitleCoor, int offset)
{
	auto layer = _miniTMXMap->getLayer("layer");
	Size titleSize = _miniTMXMap->getTileSize();
	auto pos = layer->getTileAt(beginTitleCoor)->getPosition();
	Vec2 endPos = pos + offset * titleSize;
	DrawNode * draw = DrawNode::create();
	draw->drawRect(pos, endPos, Color4F::GRAY);
	_miniTMXMap->addChild(draw, 1);
}

void BattleScene::createNeutralUnit()
{
	auto iconTexture = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/neutral_icon.png");
	auto parentNode = Node::create();
	parentNode->setPosition(Vec2::ZERO);
	_miniTMXMap->addChild(parentNode);
	for (int i = 0; i < 2; i++)
	{

		auto unit = Character::createCharacter(8);
		unit->setScale(IMAGE_SCALE);
		unit->setBirdMode(false);
		if (_gameMode == SOLO_MODE) 
		{
			auto body = PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 0));
			body->setRotationEnable(false);
			body->setCollisionBitmask(0x00001);
			body->setContactTestBitmask(0x00001);
			body->setGravityEnable(false);
			unit->setPhysicsBody(body);
		}

		_neutralUnitList.push_back(unit);
		
		Vec2 titleCoor = Vec2(_myMap->getMapSize().width / 2, i*(_myMap->getMapSize().height - 16) + 8);
		Vec2 pos = getPositionForTitleCoord(titleCoor);
		//unit->setPosition(Vec2(_myMap->getBoundingBox().size.width / 2, i*(_myMap->getBoundingBox().size.height - 200) + 100));
		unit->setPosition(pos);
		unit->setName("NEUTRAL");
		unit->setTag(0);
		unit->setSaveAtttackTime(0);
		_battleBackround->addChild(unit,1);

		auto miniIcon = Sprite::createWithTexture(iconTexture);
		parentNode->addChild(miniIcon);
		_neutralUnitIconInMiniMap.push_back(miniIcon);

		vector<bool> saveOldPos = {};
		_saveOldPosOfNeutral.push_back(saveOldPos);

	}
	if (_gameMode == SOLO_MODE) {
		neutralUnitStatusChange(_neutralUnitList.back(), _currentEnemyTeamFlg, _neutralUnitList.size() - 1);
	}
	
}

void BattleScene::createCannon(int lisSize)
{
	for (int i = 0; i < lisSize; i++)
	{
		auto pr = Node::create();
		pr->setPosition(getPositionForTitleCoord(Vec2(35, 10 + (50 - 20)*i)));
		auto cannon = Cannon::createCannon();
		_cannonList.push_back(cannon);
		pr->addChild(cannon);
		_battleBackround->addChild(pr, 1);
		cannon->setPosition(-1*cannon->getContentSize()/2);
		drawAreaRectangle(cannon, 4);
	}
}

void BattleScene::createContent()
{
	//create e battle background
	auto realVisibleSize = _visibleSize / 0.75f;
	auto parentNode = Node::create();
	parentNode->setPosition((realVisibleSize - _visibleSize) / 2);
	addChild(parentNode);
	parentNode->setScale(0.75f);

	_battleBackround = Node::create();
	_battleBackround->setPosition(Vec2::ZERO);
	_battleBackround->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	parentNode->addChild(_battleBackround);
	//Load the MTC map
	string pathFIle = FileUtils::getInstance()->fullPathForFilename("map/test_batgiac.tmx");
	_myMap = TMXTiledMap::create(pathFIle.c_str());

	if (_myMap != nullptr)
	{
		_battleBackround->addChild(_myMap);
		Size CC_UNUSED s = _myMap->getContentSize();
		log("MAP ContentSize: %f, %f", s.width, s.height);
		Vec2 ts = _myMap->getTileSize();
		//log("Title Size: %f, %f", ts.x, ts.y);
		Vec2 ts2 = _myMap->getMapSize();
		log("map Size: %f, %f", ts2.x, ts2.y);
		auto& children = _myMap->getChildren();
		SpriteBatchNode* child = nullptr;

		for (const auto &obj : children) {
			child = static_cast<SpriteBatchNode*>(obj);
			child->getTexture()->setAntiAliasTexParameters();
		}

		_mapLayer = _myMap->getLayer("main_layer");
		auto ss = _mapLayer->getLayerSize();
		log("layer size: %f, %f", ss.width, ss.height);
		_blockLayer = _myMap->getLayer("block_layer");
		_blockLayer->setVisible(false);
	}
	else {
		log("null");
	}


	createWormHole();

	createBoudingWall();

	createPhysicBolder();

	auto blueTeamAreaNode = Node::create();
	blueTeamAreaNode->setPhysicsBody(PhysicsBody::createBox(Size(200, 150), PhysicsMaterial(1, 0, 0)));
	blueTeamAreaNode->setPosition(Vec2(75, _myMap->getContentSize().height / 2));
	blueTeamAreaNode->getPhysicsBody()->setCategoryBitmask(BLUETEAM_CONTACT_CATEGORY_BITMAP);
	blueTeamAreaNode->getPhysicsBody()->setCollisionBitmask(BLUETEAM_CONTACT_COLLISION_BITMAP);
	blueTeamAreaNode->getPhysicsBody()->setGravityEnable(false);
	blueTeamAreaNode->getPhysicsBody()->setDynamic(false);
	_battleBackround->addChild(blueTeamAreaNode);

	auto redTeamAreaNode = Node::create();
	redTeamAreaNode->setPhysicsBody(PhysicsBody::createBox(Size(200, 150), PhysicsMaterial(1, 0, 0)));
	redTeamAreaNode->getPhysicsBody()->setCategoryBitmask(REDTEAM_CONTACT_CATEGORY_BITMAP);
	redTeamAreaNode->getPhysicsBody()->setCollisionBitmask(REDTEAM_CONTACT_COLLISION_BITMAP);
	redTeamAreaNode->getPhysicsBody()->setGravityEnable(false);
	redTeamAreaNode->getPhysicsBody()->setDynamic(false);
	redTeamAreaNode->setPosition(Vec2(_myMap->getContentSize().width - 75, _myMap->getContentSize().height / 2));
	_battleBackround->addChild(redTeamAreaNode);

	//red Tower
	auto redTower = Tower::createTower(2);
	redTower->setTag(TOWER_TAG);
	redTower->setName("RED TOWER");
	auto redTowerParentNode = Node::create();
	redTowerParentNode->addChild(redTower);
	redTowerParentNode->setPosition(Vec2(_myMap->getContentSize().width - 200, _myMap->getContentSize().height / 2));
	//redTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(redTowerParentNode);
	MyBodyParser::getInstance()->parseJsonFile("json/tower.json");
	PhysicsBody* redTBody = MyBodyParser::getInstance()->bodyFormJson(redTower, "tower");
	redTBody->setDynamic(false);
	redTBody->setGravityEnable(false);
	redTBody->setContactTestBitmask(0x1);
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

	auto blueTowerParentNode = Node::create();
	
	auto blueTower = Tower::createTower(1);
	blueTower->setTag(TOWER_TAG);
	blueTower->setName("BLUE TOWER");
	blueTowerParentNode->setPosition(Vec2(200, _myMap->getContentSize().height / 2));
	//blueTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(blueTowerParentNode);
	blueTowerParentNode->addChild(blueTower);
	PhysicsBody* blueTBody = MyBodyParser::getInstance()->bodyFormJson(blueTower, "tower");
	blueTBody->setDynamic(false);
	blueTBody->setGravityEnable(false);
	blueTBody->setContactTestBitmask(0x1);
	blueTower->setPhysicsBody(blueTBody);

	auto blueTHpBar = Slider::create();
	blueTHpBar->loadBarTexture("image/screen/battle/mini_hp_base.png");
	blueTHpBar->setPercent(100);
	blueTHpBar->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	blueTHpBar->setPosition(Vec2(blueTower->getContentSize().width / 2, blueTower->getContentSize().height - 10));
	blueTower->addChild(blueTHpBar);
	blueTower->setTag(ENEMY_NUM);
	//_allStone.push_back(redTower);
	//_allStone.push_back(blueTower);
	Room_User_Unit_Model unitData1;
	if (_gameMode == MULTI_MODE)
	{
		auto uuId = UserModel::getInstance()->getUuId();
		unitData1 = BattleModel::getInstance()->getUnitInforByUuid(uuId.c_str());
	}
	else {
		unitData1.position_x = 100;
		unitData1.position_y = 1500;
		unitData1.direction = 9;
		unitData1.angle = 0;
	}
	// Here is setting position and direction for main unit. But by this way will make error to reference to the UserModel and BattleModel.

	int cTeamContactCategory;
	int cTeamContactCollision;
	int cEnemyContactCategory;
	int cEnemyContactCollision;

	if (_currentPlayerTeamFlg == TEAM_FLG_BLUE) {
		cTeamContactCategory = BLUETEAM_CONTACT_CATEGORY_BITMAP;
		cTeamContactCollision = BLUETEAM_CONTACT_COLLISION_BITMAP;
		cEnemyContactCategory = REDTEAM_CONTACT_CATEGORY_BITMAP;
		cEnemyContactCollision = REDTEAM_CONTACT_COLLISION_BITMAP;
	}
	else {
		cTeamContactCategory = REDTEAM_CONTACT_CATEGORY_BITMAP;
		cTeamContactCollision = REDTEAM_CONTACT_COLLISION_BITMAP;
		cEnemyContactCategory = BLUETEAM_CONTACT_CATEGORY_BITMAP;
		cEnemyContactCollision = BLUETEAM_CONTACT_COLLISION_BITMAP;
	}


	testObject = Character::createCharacter(_selectedUnitId);
	testObject->setTag(10);
	testObject->setBirdMode(false);
	//testObject->setPosition(_visibleSize);
	_battleBackround->addChild(testObject, MID);
	//testObject->setPosition(Vec2(_visibleSize.width, 100));
	testObject->setPosition(Vec2(unitData1.position_x, unitData1.position_y));
	testObject->rotateCharacter(unitData1.direction);
	
	testObject->setScale(IMAGE_SCALE);
	testObject->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0.95, 0.5)));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->getPhysicsBody()->setContactTestBitmask(0x1);
	testObject->getPhysicsBody()->setCategoryBitmask(cTeamContactCategory);
	testObject->getPhysicsBody()->setCollisionBitmask(cTeamContactCollision);
	testObject->setCharacterMoveSpeed(_allAlliedUnitData[0].move_speed);

	_mainCharacterMiniHpBar = Slider::create();
	_mainCharacterMiniHpBar->loadBarTexture("image/screen/battle/mini_hp_base.png");
	_mainCharacterMiniHpBar->setPercent(100);
	//_miniHpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterMiniHpBar->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_mainCharacterMiniHpBar->setScale(1 / (IMAGE_SCALE *1.5f));
	testObject->addChild(_mainCharacterMiniHpBar);
	_mainCharacterMiniHpBar->setPosition(Vec2(testObject->getContentSize().width / 2, 20));


	_allAlliedUnitHpBar.push_back(_mainCharacterMiniHpBar);
	_allAlliedUnitSprite.push_back(testObject);
	_battleBackround->setPhysicsBody(PhysicsBody::createEdgeBox(Size(1, 1), PhysicsMaterial(1, 1, 1)));

	/************************************************************************/
	/* bellow content position will decide by screen orirent mode           */
	/************************************************************************/
	float baseMargin = 15;
	_topMenuSprite = Sprite::create("image/screen/battle/status_parts.png");
	_topMenuSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	_topMenuSprite->setPosition(Vec2(0, _visibleSize.height));
	addChild(_topMenuSprite);

	_mainCharacterHpBar = Slider::create();
	_mainCharacterHpBar->loadBarTexture("image/screen/battle/hp_base.png");
	_mainCharacterHpBar->setPercent(100);
	//_hpSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterHpBar->loadProgressBarTexture("image/screen/battle/hp.png");
	//_hpSlider->setContentSize(Size(372, 12));
	_mainCharacterHpBar->setPosition(Vec2(_topMenuSprite->getContentSize().width / 2 + 25, _topMenuSprite->getContentSize().height - 22));
	_topMenuSprite->addChild(_mainCharacterHpBar);

	_hpViewLabel = Label::createWithSystemFont(DataUtils::numberToString(_allAlliedUnitData[0].hp), "", 20);
	_hpViewLabel->setColor(Color3B::GREEN);
	_hpViewLabel->setContentSize(Size(50, 50));
	_hpViewLabel->setPosition(Vec2(_mainCharacterHpBar->getContentSize()) - Vec2(0, 50));
	_mainCharacterHpBar->addChild(_hpViewLabel);
	Vec2 slotPos = Vec2(45, _topMenuSprite->getContentSize().height / 2);
	_characterImageViewNode = ClippingNode::create();
	_characterImageViewNode->setAlphaThreshold(0);
	_characterImageViewNode->setPosition(Vec2::ZERO);

	auto mask = Sprite::create("image/screen/battle/icon_slot.png");
	mask->setPosition(slotPos);
	_characterImageViewNode->setStencil(mask);

	auto icon = Sprite::create(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(_allAlliedUnitData[0].mst_unit_id));
	icon->setPosition(slotPos + Vec2(0, 0));
	icon->setScale(4.0f);
	icon->setTag(ICON);
	_characterImageViewNode->addChild(icon);
	_topMenuSprite->addChild(_characterImageViewNode);

	_menuButton = Button::create();
	_menuButton->loadTextureNormal("image/screen/battle/menu_btn.png");
	_menuButton->addTouchEventListener(CC_CALLBACK_2(BattleScene::menuButtonCallback, this));
	_menuButton->setPosition(Vec2(_menuButton->getContentSize().width / 2 + 10, _topMenuSprite->getContentSize().height - _topMenuSprite->getContentSize().height));
	_menuButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
	_topMenuSprite->addChild(_menuButton);

	auto birdButton = Button::create();
	birdButton->loadTextureNormal("bird.png");
	birdButton->setTouchEnabled(true);
	birdButton->addTouchEventListener(CC_CALLBACK_2(BattleScene::birdButtonCallback, this));
	_topMenuSprite->addChild(birdButton, 10);
	birdButton->setPosition(Vec2(130, _topMenuSprite->getContentSize().height - 125));



	Sprite *timeViewContainer = Sprite::create("image/screen/battle/time_parts.png");
	//timeViewContainer->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	timeViewContainer->setPosition(Vec2(_topMenuSprite->getContentSize().width*2/5,_topMenuSprite->getContentSize().height/2 - 3));
	_topMenuSprite->addChild(timeViewContainer);

	string pathFIle2 = FileUtils::getInstance()->fullPathForFilename("map/mini_map_white.tmx");
	_miniTMXMap = TMXTiledMap::create(pathFIle2.c_str());
	if (_miniTMXMap != nullptr)
	{
		_miniLayer = _miniTMXMap->getLayer("layer");
		addChild(_miniTMXMap,2);
		_miniLayer->getTexture()->setAntiAliasTexParameters();
		_miniTMXMap->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		//_miniTMXMap->setPosition(timeViewContainer->getPosition() - Vec2(0, 50));
		drawRectInMinimap(Vec2(42, 13), 8);
		drawRectInMinimap(Vec2(19, 13), 8);
		drawRectInMinimap(Vec2(19, 46), 8);
		drawRectInMinimap(Vec2(31, 30), 8);
		drawRectInMinimap(Vec2(42, 46), 8);
		drawRectInMinimap(Vec2(33, 13), 4);
		drawRectInMinimap(Vec2(33, 43), 4);
	}

	_selectRect = Sprite::create("image/screen/battle/map_frame.png");
	_selectRect->getTexture()->setAntiAliasTexParameters();
	_selectRect->setPosition(Vec2(_miniTMXMap->getContentSize().width / 2, _miniTMXMap->getContentSize().height / 2 + _selectRect->getContentSize().height / 2));
	_miniTMXMap->addChild(_selectRect, 2);

	_mainCharacterIconInMiniMap = Sprite::create("mini_icon.png");
	_mainCharacterIconInMiniMap->setPosition(Vec2(_selectRect->getContentSize().width / 2, _selectRect->getContentSize().height / 2));
	_selectRect->addChild(_mainCharacterIconInMiniMap);
	_mainCharacterIconInMiniMap->getTexture()->setAntiAliasTexParameters();
	_mainCharacterIconInMiniMap->setRotation(-(unitData1.angle) + 90);

	_timeViewLabel = Label::createWithSystemFont("00:00:00", "", 25);
	_timeViewLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	_timeViewLabel->setPosition(Vec2(timeViewContainer->getContentSize().width / 2, timeViewContainer->getContentSize().height / 2));
	timeViewContainer->addChild(_timeViewLabel);

	string skill1ImagePath = _mainCharacterSkillData[0].skill_icon_path;
	string skill2ImagePath = _mainCharacterSkillData[1].skill_icon_path;

	string skill3ImagePath = _playerSkills[0].skill_icon_path;
	string skill4ImagePath = _playerSkills[1].skill_icon_path;

	
	_skill1Button = Button::create();
	_skill1Button->loadTextureNormal(skill1ImagePath.c_str());
	_skill1Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	Size baseSize = _skill1Button->getContentSize();

	_skillButtonParentnode = Node::create();
	addChild(_skillButtonParentnode);


	if (_screenMode == SCREEN_VERTICAL)
	{
		_miniTMXMap->setPosition(Vec2(_miniTMXMap->getContentSize().width + baseMargin, _visibleSize.height - baseMargin));
		_skillButtonParentnode->setPosition(Vec2(_visibleSize.width - baseSize.width / 2 - baseMargin, _visibleSize.height / 2));
		_skillButtonParentnode->setRotation(-90.0f);
		_topMenuSprite->setRotation(-90.0f);
		_topMenuSprite->setPosition(Vec2(baseMargin, baseMargin));
	}
	else {
		_miniTMXMap->setPosition(Vec2(_visibleSize.width -baseMargin, _visibleSize.height - baseMargin));
		_skillButtonParentnode->setPosition(Vec2(_visibleSize.width / 2, baseMargin + baseSize.height / 2));
		_skillButtonParentnode->setRotation(0);
		_topMenuSprite->setRotation(0);
		_topMenuSprite->setPosition(Vec2(baseMargin, baseMargin));
	}
	/******************************************************************************************/

	_skill1Button->setTag(TAG_SKILL_1);
	_skill1Button->setSwallowTouches(true);
	_skill1Button->setPosition(Vec2(- 1.5 * baseSize.width - 20, 0));
	_skillButtonParentnode->addChild(_skill1Button);
	displaySkillMpInButton(_skill1Button, _mainCharacterSkillData[0].mp_cost);

	_skill2Button = Button::create();
	_skill2Button->loadTextureNormal(skill2ImagePath.c_str());
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill2Button->setTag(TAG_SKILL_2);
	_skill2Button->setSwallowTouches(true);
	_skill2Button->setPosition(Vec2(-0.5 *baseSize.width - 10, 0));
	_skillButtonParentnode->addChild(_skill2Button);
	displaySkillMpInButton(_skill2Button, _mainCharacterSkillData[1].mp_cost);

	_skill3Button = Button::create();
	_skill3Button->loadTextureNormal(skill3ImagePath.c_str());
	_skill3Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill3Button->setTag(TAG_SKILL_3);
	_skill3Button->setSwallowTouches(true);
	_skill3Button->setPosition(Vec2(0.5*baseSize.width + 10,0));
	_skillButtonParentnode->addChild(_skill3Button);
	displaySkillMpInButton(_skill3Button, _playerSkills[0].mp_cost);

	_skill4Button = Button::create();
	_skill4Button->loadTextureNormal(skill4ImagePath.c_str());
	_skill4Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill4Button->setTag(TAG_SKILL_4);
	_skill4Button->setSwallowTouches(true);
	_skill4Button->setPosition(Vec2(1.5 * baseSize.width + 20, 0));
	_skillButtonParentnode->addChild(_skill4Button);
	displaySkillMpInButton(_skill4Button, _playerSkills[1].mp_cost);

	_mainCharacterMpBar = Slider::create();
	_mainCharacterMpBar->loadBarTexture("image/screen/battle/mp_base.png");
	//_manaSlider->loadSlidBallTextureNormal("image/screen/battle/test.png");
	_mainCharacterMpBar->loadProgressBarTexture("image/screen/battle/mp.png");
	_mainCharacterMpBar->setPercent(100);
	_mainCharacterMpBar->setPosition(Vec2(0, baseSize.height /2 + 2*baseMargin));
	_skillButtonParentnode->addChild(_mainCharacterMpBar);

	_mpViewlabel = Label::createWithSystemFont(DataUtils::numberToString(_allAlliedUnitData[0].mp), "", 20);
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


	/*_autoAttackArea = Sprite::create("image/screen/battle/select.png");
	testObject->addChild(_autoAttackArea);
	_autoAttackArea->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2 - 30));
	_autoAttackArea->setVisible(false);*/

	float positionXScaleRate = _miniTMXMap->getContentSize().width / (_myMap->getContentSize().width);
	float positionYScaleRate = _miniTMXMap->getContentSize().height / (_myMap->getContentSize().height);

	auto node = Node::create();
	node->setPosition(Vec2::ZERO);
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		auto battleUnitInfo = BattleModel::getInstance()->getUnitInforByUuid(_allEnemyUnitData[i].uuid.c_str());
		auto sp = Character::createCharacter(_allEnemyUnitData[i].mst_unit_id);
		sp->setBirdMode(false);
		sp->rotateCharacter(battleUnitInfo.direction);
		sp->setPosition(Vec2(battleUnitInfo.position_x + (i - 1) * 70, battleUnitInfo.position_y));
		sp->setScale(IMAGE_SCALE);
		sp->setTag(i);
		sp->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 0)/*, Vec2(0, -50)*/));
		sp->getPhysicsBody()->setRotationEnable(false);
		sp->getPhysicsBody()->setGravityEnable(false);
		sp->getPhysicsBody()->setContactTestBitmask(0x1);
		sp->getPhysicsBody()->setCategoryBitmask(cEnemyContactCategory);
		sp->getPhysicsBody()->setCollisionBitmask(cEnemyContactCollision);
		auto hpB = Slider::create();
		hpB->loadBarTexture("image/screen/battle/mini_hp_base.png");
		hpB->setPercent(100);
		//hpB->loadSlidBallTextureNormal("image/screen/battle/test.png");
		hpB->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
		//_hpSlider->setContentSize(Size(372, 12));
		hpB->setPosition(Vec2(sp->getContentSize().width / 2, 20));
		hpB->setTag(HPBAR_TAG);
		hpB->setScale(1 / (IMAGE_SCALE*1.5f));
		_allEnemyHpBar.push_back(hpB);
		sp->addChild(_allEnemyHpBar.back(), 100);
		_allEnemyUnitSprite.push_back(sp);
		_battleBackround->addChild(_allEnemyUnitSprite.back(), MID);

		auto enemyIcon = Sprite::create("image/screen/battle/enemy_icon.png");
		enemyIcon->setOpacity(255);
		enemyIcon->getTexture()->setAntiAliasTexParameters();
		enemyIcon->setPosition(Vec2(sp->getPositionX()*positionXScaleRate, sp->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon);
		node->addChild(_allEnemyIconInMinimap.back(), 1);
		_allEnemyAttachDelay.push_back(false);

		vector<string> tmp = {};
		_enemyStatusImagePath.push_back(tmp);
	}

	vector<string> myList = {};
	_alliedStatusImagePath.push_back(myList);
	_miniTMXMap->addChild(node, 1);

	for (int i = 1; i < _allAlliedUnitData.size(); i++)
	{
		auto battleUnitInfo1 = BattleModel::getInstance()->getUnitInforByUuid(_allAlliedUnitData[i].uuid.c_str());
		auto sp1 = Character::createCharacter(_allAlliedUnitData[i].mst_unit_id);
		sp1->rotateCharacter(battleUnitInfo1.direction);
		sp1->setBirdMode(false);
		sp1->setPosition(Vec2(battleUnitInfo1.position_x + (i - 1) * 70, battleUnitInfo1.position_y));
		sp1->setScale(IMAGE_SCALE);
		sp1->setTag(i);
		sp1->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 0)/*, Vec2(0, -50)*/));
		sp1->getPhysicsBody()->setRotationEnable(false);
		sp1->getPhysicsBody()->setGravityEnable(false);
		sp1->getPhysicsBody()->setContactTestBitmask(0x1);
		sp1->getPhysicsBody()->setCategoryBitmask(cTeamContactCategory);
		sp1->getPhysicsBody()->setCollisionBitmask(cTeamContactCollision);
		auto hpB1 = Slider::create();
		hpB1->loadBarTexture("image/screen/battle/mini_hp_base.png");
		hpB1->setPercent(100);
		//hpB->loadSlidBallTextureNormal("image/screen/battle/test.png");
		hpB1->loadProgressBarTexture("image/screen/battle/mini_hp_gauge.png");
		//_hpSlider->setContentSize(Size(372, 12));
		hpB1->setPosition(Vec2(sp1->getContentSize().width / 2, 20));
		hpB1->setTag(HPBAR_TAG);
		hpB1->setScale(1 / (IMAGE_SCALE*1.5f));
		_allAlliedUnitHpBar.push_back(hpB1);
		sp1->addChild(_allAlliedUnitHpBar.back(), 100);
		_allAlliedUnitSprite.push_back(sp1);
		_battleBackround->addChild(_allAlliedUnitSprite.back(), MID);

		auto allied = Sprite::create("image/screen/battle/allied_icon.png");
		allied->setOpacity(255);
		allied->getTexture()->setAntiAliasTexParameters();
		allied->setPosition(Vec2(sp1->getPositionX()*positionXScaleRate, sp1->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(allied);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);

		vector<string> tmp1 = {};
		_alliedStatusImagePath.push_back(tmp1);
	}
	auto enemyIcon1 = Sprite::create("image/screen/battle/enemy_icon.png");
	enemyIcon1->setOpacity(255);

	auto alliedIcon = Sprite::create("image/screen/battle/allied_icon.png");
	alliedIcon->setOpacity(255);
	vector<string> tmp1 = {};
	vector<string> tmp2 = {};

	switch (_currentPlayerTeamFlg)
	{
	case TEAM_FLG_RED:
		_allAlliedUnitSprite.push_back(redTower);
		_allAlliedUnitHpBar.push_back(redTHpBar);
		_allAlliedUnitData.push_back(_redTeamTowerData);
		alliedIcon->setPosition(Vec2(redTower->getParent()->getPositionX()*positionXScaleRate, redTower->getParent()->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(alliedIcon);
		_alliedStatusImagePath.push_back(tmp2);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);


		_allEnemyAttachDelay.push_back(false);
		_allEnemyHpBar.push_back(blueTHpBar);
		_allEnemyUnitData.push_back(_blueTeamTowerData);
		_allEnemyUnitSprite.push_back(blueTower);

		_enemyStatusImagePath.push_back(tmp1);

		// 		float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
		// 		float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
		enemyIcon1->setPosition(Vec2(blueTower->getParent()->getPositionX()*positionXScaleRate, blueTower->getParent()->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon1);
		node->addChild(_allEnemyIconInMinimap.back(), 1);

		break;
	case TEAM_FLG_BLUE:
		_allAlliedUnitSprite.push_back(blueTower);
		_allAlliedUnitHpBar.push_back(blueTHpBar);
		_allAlliedUnitData.push_back(_blueTeamTowerData);
		alliedIcon->setPosition(Vec2(blueTower->getParent()->getPositionX()*positionXScaleRate, blueTower->getParent()->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(alliedIcon);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);
		_alliedStatusImagePath.push_back(tmp2);


		_allEnemyAttachDelay.push_back(false);
		_allEnemyHpBar.push_back(redTHpBar);
		_allEnemyUnitData.push_back(_redTeamTowerData);
		_allEnemyUnitSprite.push_back(redTower);
		_enemyStatusImagePath.push_back(tmp1);

		enemyIcon1->setPosition(Vec2(redTower->getParent()->getPositionX()*positionXScaleRate, redTower->getParent()->getPositionY()*positionYScaleRate));
		_allEnemyIconInMinimap.push_back(enemyIcon1);
		node->addChild(_allEnemyIconInMinimap.back(), 1);

		break;
	default:
		break;
	}

	//Init data
	for (int i = 0; i < _allAlliedUnitData.size(); i++)
	{
		_allAlliedUnitMaxHp.push_back(_allAlliedUnitData[i].hp);
	}
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemuUnitMaxHp.push_back(_allEnemyUnitData[i].hp);
	}
	/************************************************************************/
	/* CREATE NEUTRAL                                                       */
	/************************************************************************/

	createNeutralTower();

	createNeutralUnit();

	createCannon(2);

	//TODO HERE IS THE CODE TO TEST BATTLE MOVE SYNC FUNCTION
	if (_gameMode == MULTI_MODE)
	{
		auto sv = NodeServer::getInstance()->getClient();
		/************************************************************************/
		/* HANDLERS FOR BATTLE BROADCAST EVENTS                                                                     */
		/************************************************************************/
		/*Battle update event*/
		sv->on("battle_update", [&](SIOClient* client, const std::string data){
			//log("BATTLE UPDATE data: %s", data.c_str());
			//same data with battle sync but change for detect unit by uuid

			/*Check for battle destruct called*/
			if (_onDestructCalled) return;

			auto uuid = UserModel::getInstance()->getUuId();
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Parse JSOn error");
				return;
			}
			if (doc.IsObject()) {
				for (int i = 0; i < doc["user_unit"].Size(); i++) {
					//log("!EEEE");
					Room_User_Unit_Model tempUnit;
					tempUnit._id = doc["user_unit"][rapidjson::SizeType(i)]["_id"].GetString();
					tempUnit.status = doc["user_unit"][rapidjson::SizeType(i)]["status"].GetInt();
					tempUnit.mp = doc["user_unit"][rapidjson::SizeType(i)]["mp"].GetInt();
					tempUnit.hp = doc["user_unit"][rapidjson::SizeType(i)]["hp"].GetInt();
					tempUnit.position_x = doc["user_unit"][rapidjson::SizeType(i)]["position_x"].GetDouble();
					tempUnit.position_y = doc["user_unit"][rapidjson::SizeType(i)]["position_y"].GetDouble();
					tempUnit.direction = doc["user_unit"][rapidjson::SizeType(i)]["direction"].GetInt();
					tempUnit.team_id = doc["user_unit"][rapidjson::SizeType(i)]["team_id"].GetInt();
					tempUnit.room_id = doc["user_unit"][rapidjson::SizeType(i)]["room_id"].GetInt();
					tempUnit.user_id = doc["user_unit"][rapidjson::SizeType(i)]["user_id"].GetInt();
					tempUnit.mst_unit_id = doc["user_unit"][rapidjson::SizeType(i)]["mst_unit_id"].GetInt();
					tempUnit.uuid = doc["user_unit"][rapidjson::SizeType(i)]["uuid"].GetString();
					tempUnit.angle = doc["user_unit"][rapidjson::SizeType(i)]["angle"].GetDouble();
					tempUnit.moving = doc["user_unit"][rapidjson::SizeType(i)]["moving"].GetBool();

					BattleModel::getInstance()->updateUserUnit(tempUnit);
					string sv_uuid = doc["user_unit"][rapidjson::SizeType(i)]["uuid"].GetString();
					if (strcmp(sv_uuid.c_str(), uuid.c_str()) == 0) {
						//log("this is my unit");
						continue;
					}
					int teamId = UserModel::getInstance()->getUserInfo().team_id;

					//bellow code for sync position and move Animation
					vector<Sprite*> processUnitSprite = {};
					vector<UserUnitInfo> processUnitData = {};
					if (doc["user_unit"][rapidjson::SizeType(i)]["team_id"].GetInt() == teamId) {
						processUnitSprite = _allAlliedUnitSprite;
						processUnitData = _allAlliedUnitData;
					}
					else {
						processUnitSprite = _allEnemyUnitSprite;
						processUnitData = _allEnemyUnitData;
					}
					for (int j = 0; j < processUnitSprite.size(); j++)
					{
						if (processUnitSprite[j]->getTag() == TOWER_TAG) {
							continue;
						}
						if (strcmp(processUnitData[j].uuid.c_str(), sv_uuid.c_str()) == 0)
						{
							Character* cha = (Character*)processUnitSprite[j];
							cha->setPosition(Vec2(doc["user_unit"][rapidjson::SizeType(i)]["position_x"].GetDouble(), doc["user_unit"][rapidjson::SizeType(i)]["position_y"].GetDouble()));
							if (doc["user_unit"][rapidjson::SizeType(i)]["moving"].GetBool())
							{
								cha->actionMoveCharacter(doc["user_unit"][rapidjson::SizeType(i)]["direction"].GetInt());
							}
							else {
								cha->stopMoveAction();
								cha->getPhysicsBody()->setVelocity(Vec2::ZERO);
							}
						}
					}
				}
			}
			else {
				log("Error in JSON Object. Type: %d", doc.GetType());
			}
		});

		/*Unit move event public event*/
		sv->on("unit_move_end", [&](SIOClient* client, const string& data) {
			//log("Unit_move_end data: %s", data.c_str());
			/*check for battle scene ondestruct called*/
			if (_onDestructCalled) return;
			//testSprite->getPhysicsBody()->setVelocity(Vec2::ZERO);

		});

		/** handler for battle public attack event
		 *  to display another player attack animation
		 */
		sv->on("attack", [&](SIOClient *client, const string data) {
			log("Another attacks: %s", data.c_str());
			/*check for battle destruct called*/
			if (_onDestructCalled) return;

			auto uuid = UserModel::getInstance()->getUuId();
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Parse JSOn error");
				return;
			}
			if (doc.IsObject()) {
				int teamId = UserModel::getInstance()->getUserInfo().team_id;
				if (doc["team_id"].GetInt() == teamId) {
					rt_attackAnimationandLogic(doc, _allAlliedUnitSprite, &_allAlliedUnitData, _allEnemyUnitSprite, &_allEnemyUnitData);

				}
				else {
					rt_attackAnimationandLogic(doc, _allEnemyUnitSprite, &_allEnemyUnitData, _allAlliedUnitSprite, &_allAlliedUnitData);
				}

			}
		});
		/** Handler for battle public skill event
		 *  To display all skill animation and logic
		 */

		sv->on("play_skill_end", [&](SIOClient* client, const std::string data) {
			log("Skill event callback data: %s", data.c_str());
			/*check for battle scene destruct called*/
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("on: play_skill_end Parse JSOn error");
				return;
			}
			if (doc.IsObject()) {
				/************************************************************************/
				/* PARSE SKILL DATA                                                     */
				/************************************************************************/
				UserSkillInfo skill;
				skill.mst_skill_id = doc["mst_skill"]["mst_skill_id"].GetInt();
				skill.skill_type = doc["mst_skill"]["skill_type"].GetInt();
				skill.mp_cost = doc["mst_skill"]["mp_cost"].GetInt();
				skill.cooldown_time = doc["mst_skill"]["cooldown_time"].GetInt();
				skill.range_type = doc["mst_skill"]["range_type"].GetInt();
				skill.range_distance = doc["mst_skill"]["range_distance"].GetInt();
				skill.multi_effect = doc["mst_skill"]["multi_effect"].GetInt();
				skill.target_type = doc["mst_skill"]["target_type"].GetInt();
				skill.effect_type = doc["mst_skill"]["effect_type"].GetInt();
				skill.buff_effect_type = doc["mst_skill"]["buff_effect_type"].GetInt();
				skill.duration = doc["mst_skill"]["duration"].GetInt();
				skill.corrett_value = doc["mst_skill"]["corrett_value"].GetInt();
				skill.correct_type = doc["mst_skill"]["correct_type"].GetInt();

				/************************************************************************/
				/* PARSER UNIT THAT CAST SKILL                                                     */
				/************************************************************************/

				UserUnitInfo unit;
				unit.mst_unit_id = doc["mst_unit"]["mst_unit_id"].GetInt();
				unit.hp = doc["mst_unit"]["hp"].GetInt();
				unit.hp_heal = doc["mst_unit"]["hp_heal"].GetInt();
				unit.mp = doc["mst_unit"]["mp"].GetInt();
				unit.mp_heal = doc["mst_unit"]["mp_heal"].GetInt();
				unit.attack = doc["mst_unit"]["attack"].GetInt();
				unit.attack_range = doc["mst_unit"]["attack_range"].GetInt();
				unit.attack_speed = doc["mst_unit"]["attack_speed"].GetInt();
				unit.defence = doc["mst_unit"]["defence"].GetInt();
				unit.move_speed = doc["mst_unit"]["move_speed"].GetInt();
				unit.element = doc["mst_unit"]["element"].GetInt();

				/************************************************************************/
				/* Bellow code to detect unit that play skill in the local data       */
				/************************************************************************/
				Sprite* playObject;
				bool found = false;
				int playObjectTeam = 0;
				int saveIndex = -1;
				for (int i = 0; i < _allAlliedUnitData.size(); i++)
				{
					if (strcmp(_allAlliedUnitData[i].uuid.c_str(), doc["uuid"].GetString()) == 0) {
						playObject = _allAlliedUnitSprite[i];
						found = true;
						playObjectTeam = 1;
						saveIndex = i;
						log("Object in allied[%d]", i);
						break;
					}
				}
				if (!found)
				{
					for (int j = 0; j < _allEnemyUnitData.size(); j++)
					{
						if (strcmp(_allEnemyUnitData[j].uuid.c_str(), doc["uuid"].GetString()) == 0) {
							playObject = _allEnemyUnitSprite[j];
							found = true;
							playObjectTeam = 2;
							saveIndex = j;
							log("Object in enemy[%d]", j);
							break;
						}
					}
				}
				int team_id = doc["team_id"].GetInt();
				float randNum = doc["random"].GetDouble();
				string uuid = doc["uuid"].GetString();
				log("team Id: %d", team_id);
				playSkillLogicAndAnimation(playObject, skill, team_id, randNum, uuid.c_str(), saveIndex, unit);
			}
		});

		sv->on("tower_attack", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("on: play_skill_end Parse JSOn error");
				return;
			}
			if (doc.IsObject()) {
				string towerUUid = doc["tower"].GetString();
				string targetUuid = doc["target"].GetString();
				int teamId = doc["team_id"].GetInt();
				float randomNum = doc["rand_num"].GetDouble();
				//OK OK. NEXT WEEK: SYNC TOWER ATTACK ANIMATION

				if (teamId == _currentPlayerTeamFlg)
				{
					towerAttackLogic(_allAlliedUnitSprite.back(), _allAlliedUnitData.back(), _allEnemyUnitSprite, &_allEnemyUnitData, targetUuid.c_str(), randomNum);
				}
				else
				{
					towerAttackLogic(_allEnemyUnitSprite.back(), _allEnemyUnitData.back(), _allAlliedUnitSprite, &_allAlliedUnitData, targetUuid.c_str(), randomNum);
				}
			}
		});

		sv->on("set_title", [&](SIOClient* client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("on: set_title Parse JSOn error");
				return;
			}

			int team_Id = doc["team_id"].GetInt();
			int x = doc["pos_x"].GetInt();
			int y = doc["pos_y"].GetInt();
			bool disableFlg = doc["disable"].GetBool();
			if (x < 0 || x > _mapLayer->getLayerSize().width || y < 0 || y > _mapLayer->getLayerSize().height) {
				log("Invalid title position");
				return;
			}
			Vec2 titleCoor = Vec2(x, y);
			auto title = _mapLayer->getTileAt(titleCoor);
			if (title == nullptr) return;
			if (team_Id == TEAM_FLG_BLUE)
			{
				if (disableFlg) {
					//in case of disable title
					title->setColor(Color3B::BLUE);
					auto mTittle = _miniLayer->getTileAt(titleCoor);
					mTittle->setColor(Color3B::BLUE);
					title->setName("disable");
					log("disable title :  %d, %d", x, y);
					return;
				}
				if (title->getColor() != Color3B::GREEN)
				{
					title->setColor(Color3B::GREEN);
					auto mTittle = _miniLayer->getTileAt(titleCoor);
					mTittle->setColor(Color3B::GREEN);
					title->setName("green");
	
				}
				return;
			}
			else
			{
				if (disableFlg) {
					//in case of disable title
					title->setColor(Color3B::RED);
					auto mTittle = _miniLayer->getTileAt(titleCoor);
					mTittle->setColor(Color3B::RED);
					mTittle->setName("disable");
					log("disable title :  %d, %d", x, y);
					return;
				}
				if (title->getColor() != Color3B::ORANGE)
				{
					title->setColor(Color3B::ORANGE);
					auto mTittle = _miniLayer->getTileAt(titleCoor);
					mTittle->setColor(Color3B::ORANGE);
					mTittle->setName("orange");
				}
				return;
			}

		});

		/*handler for another unit attack to neutral event*/
		/*
		 *JSON: uuid, team_id, user_id, direc, index
		 */
		sv->on("neutral_tower_attack", [&](SIOClient *client, const string data) {
			log("neutral attack receive: %s", data.c_str());
			if (_onDestructCalled) return;

			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Battle scene: neutral_tower_attack Parse JSOn error");
				return;
			}
			vector<Sprite*>  effectSpriteList;
			vector<UserUnitInfo> effectUnitData;
			if (doc["team_id"].GetInt() == _currentPlayerTeamFlg) {
				effectSpriteList = _allAlliedUnitSprite;
				effectUnitData = _allAlliedUnitData;
			}
			else {
				effectSpriteList = _allEnemyUnitSprite;
				effectUnitData = _allEnemyUnitData;
			}
			for (int i = 0; i < effectUnitData.size(); i++)
			{
				if (strcmp(doc["uuid"].GetString(), effectUnitData[i].uuid.c_str()) == 0)
				{
					Character* object = (Character*)effectSpriteList[i];

					object->attackActionByTargetPosition(Vec2(doc["direc_x"].GetDouble(), doc["direc_y"].GetDouble()), effectUnitData[i].attack_speed, nullptr, nullptr);
					break;
				}
			}
		});

		/*Listener for neutral tower status change event*/
		/*JSON: index, team_id*/
		sv->on("neutral_status_change", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: neutral_status_change Parse JSOn error");
				return;
			}

			Tower* tower = _neutralTowerList[doc["index"].GetInt()];
			tower->changeTowerType(doc["team_id"].GetInt());
			changeTitlesNearObject(tower, doc["team_id"].GetInt(), 8);

		});

		/*Listener for other player attack neutral unit for create attack animation*/
		sv->on("neutral_unit_attack", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;

			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Battle scene: neutral_unit_attack callback parse JSON eror");
				return;
			}

			vector<Sprite*>  effectSpriteList;
			vector<UserUnitInfo> effectUnitData;
			if (doc["team_id"].GetInt() == _currentPlayerTeamFlg) {
				effectSpriteList = _allAlliedUnitSprite;
				effectUnitData = _allAlliedUnitData;
			}
			else {
				effectSpriteList = _allEnemyUnitSprite;
				effectUnitData = _allEnemyUnitData;
			}
			for (int i = 0; i < effectUnitData.size(); i++)
			{
				if (strcmp(doc["uuid"].GetString(), effectUnitData[i].uuid.c_str()) == 0)
				{
					Character* object = (Character*)effectSpriteList[i];

					object->attackActionByTargetPosition(Vec2(doc["direc_x"].GetDouble(), doc["direc_y"].GetDouble()), effectUnitData[i].attack_speed, nullptr, nullptr);
					break;
				}
			}
		});

		/*Listener for neutral unit type change event*/
		sv->on("neutral_unit_status_change", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: neutral_unit_status_change Parse JSOn error");
				return;
			}
			Character* unit = _neutralUnitList[doc["index"].GetInt()];
			neutralUnitStatusChange(unit, doc["team_id"].GetInt(), doc["index"].GetInt());

		});

		sv->on("neutral_move", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: neutral_move Parse JSOn error");
				return;
			}
			int index = doc["index"].GetInt();
			Vec2 oldPos = getPositionForTitleCoord(Vec2(doc["pos_x"].GetDouble(), doc["pos_y"].GetDouble()));
			Vec2 newPos = getPositionForTitleCoord(Vec2(doc["new_x"].GetDouble(), doc["new_y"].GetDouble()));
			Vec2 distanVector = newPos - oldPos;
			int direc = detectDirectionBaseOnTouchAngle(-distanVector.getAngle()*RAD_DEG + 90);
			//_neutralUnitList[index]->setPosition(newPos);
			_neutralUnitList[index]->stopMoveAction();
			_neutralUnitList[index]->actionMoveCharacter(direc);
			_neutralUnitList[index]->runAction(MoveTo::create(1, newPos));

		});
		/*base on another unit attack funtion. Same with tower attack and neutral unit attack*/
		sv->on("attack_cannon", [&](SIOClient *client, const string data) {

			if (_onDestructCalled) return;

			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("Battle scene: Cannon callback parse JSON eror");
				return;
			}

			vector<Sprite*>  effectSpriteList;
			vector<UserUnitInfo> effectUnitData;
			if (doc["team_id"].GetInt() == _currentPlayerTeamFlg) {
				effectSpriteList = _allAlliedUnitSprite;
				effectUnitData = _allAlliedUnitData;
			}
			else {
				effectSpriteList = _allEnemyUnitSprite;
				effectUnitData = _allEnemyUnitData;
			}
			for (int i = 0; i < effectUnitData.size(); i++)
			{
				if (strcmp(doc["uuid"].GetString(), effectUnitData[i].uuid.c_str()) == 0)
				{
					Character* object = (Character*)effectSpriteList[i];

					object->attackActionByTargetPosition(Vec2(doc["direc_x"].GetDouble(), doc["direc_y"].GetDouble()), effectUnitData[i].attack_speed, nullptr, nullptr);
					break;
				}
			}
		});

		/*Handler for cannon change team status event*/
		sv->on("cannon_change", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: Cannon change Parse JSOn error");
				return;
			}
			Cannon* cannon = _cannonList[doc["index"].GetInt()];
			cannonStatusChange(cannon, doc["team_id"].GetInt());
		});

		/*Handler for cannon on/ off event*/
		sv->on("cannon_onoff", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: Cannon ON/OFF Parse JSOn error");
				return;
			}
			Cannon* cannon = _cannonList[doc["index"].GetInt()];
			cannon->setDisableFlg(doc["disable"].GetBool());
		});

		/*Handler for wormhole enable/ disable event*/
		sv->on("wormhole_status_change", [&](SIOClient *client, const string data) {
			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: wormhole_status_change Parse JSOn error");
				return;
			}
			int ingate = doc["index"].GetInt();
			int outgate = doc["outgate"].GetInt();

			if (doc["status"].GetInt() == 0)
			{
				closeWormHole(ingate, outgate);
			}
			else {
				openWormHole(ingate, outgate);
			}
		});

		/*Listener for battle end event*/
		sv->on("battle_public_battle_end", [&](SIOClient *client, const string data) {

			if (_onDestructCalled) return;
			Document doc;
			doc.Parse<0>(data.c_str());
			if (doc.HasParseError()) {
				log("battle scene: public_battle_end Parse JSOn error");
				return;
			}
			int winTeamId = doc["win_team_id"].GetInt();


			Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene(winTeamId)));
		});
	}
	else
	{
		log("SOLO MODE");
		return;
	}
}

void BattleScene::towerAttackLogic(Sprite* towerSprite,UserUnitInfo towerData, vector<Sprite*> targetFindList, vector<UserUnitInfo>* unitDataList, string targetUuid, float randomNum)
{

	if (towerData.isStun) return;

	for (int i = 0; i < unitDataList->size(); i++)
	{
		if (strcmp(unitDataList->at(i).uuid.c_str(), targetUuid.c_str()) ==0)
		{
			auto tw = (Tower*)towerSprite;

			tw->runAttackAnimationWithTarget(targetFindList[i], CC_CALLBACK_1(BattleScene::towerAttackCallback, this, towerData, targetFindList[i], unitDataList, i, randomNum));
			
			break;
		}
	}
}

void BattleScene::towerAttackCallback(Ref *p, UserUnitInfo towerData, Sprite* target, vector<UserUnitInfo>* unitDataList, int targetIndex, float randomNum)
{
	int dame = (towerData.attack - unitDataList->at(targetIndex).defence) * caculDameRate(towerData.element, unitDataList->at(targetIndex).element)*randomNum;
	if (dame <= 1)
	{
		dame = 1;
	}
	unitDataList->at(targetIndex).hp -= dame;	
	//check die
	//check to show dame if target is curren player unit 
	if (target == testObject) {
		showAttackDame(dame, target->getPosition() + Vec2(0, 100), 1);
		//TODO
		//send receive dame event to save value

		if (unitDataList->at(targetIndex).hp <=0)
		{
			runRespawnAction(_allEnemyUnitData.back().uuid.c_str());
		}
	}
	if (unitDataList->at(targetIndex).hp <= 0) {
		unitDieAction(target, unitDataList, targetIndex);
	}
	updateSlider();
}

void BattleScene::displaySkillMpInButton(Button *parent, int mp)
{
	auto lb = Label::createWithSystemFont(DataUtils::numberToString(mp), "", 25);
	lb->setColor(Color3B(0,0,243));
	lb->setPosition(Vec2(parent->getContentSize() / 2) - Vec2(0, 30));
	lb->setTag(TAG_MP_LABEL);
	parent->addChild(lb);
}

void BattleScene::createPhysicBolder()
{
	Size s = _blockLayer->getLayerSize();
	for (int x = 0; x < s.width; x++)
	{
		for (int y = 0; y < s.height; y++)
		{
			auto title = _blockLayer->getTileAt(Vec2(x, y));
			if (title != nullptr) {
				auto body = PhysicsBody::createBox(title->getBoundingBox().size, PhysicsMaterial(1, 1, 0));
				body->setRotationEnable(false);
				body->setDynamic(false);
				body->setContactTestBitmask(0x111);
				body->setCollisionBitmask(0x1111);
				title->setPhysicsBody(body);
				title->setTag(BOUND_BORDER_TAG);

			}
		}
	}
}

Sprite* BattleScene::createCornerBolder(float angle)
{
	auto wallBd = PhysicsBody::createBox(Size(100, 800), PhysicsMaterial(1, 0.95f, 0));
	wallBd->setGravityEnable(false);
	
	wallBd->setContactTestBitmask(0x111);
	wallBd->setCollisionBitmask(0x1111);

	auto node = Sprite::create("mini_icon.png");
	node->setTag(BOUND_BORDER_TAG);
	node->setPhysicsBody(wallBd);
	node->setRotation(angle);
	wallBd->setDynamic(false);
	return node;
}

void BattleScene::onEnter()
{
	LayerBase::onEnter();
// 	srand(time(NULL));
	time(&timer);
	timeinfo = localtime(&timer);

	auto dispath = Director::getInstance()->getEventDispatcher();
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(BattleScene::onPhysicContactBegin, this);
	contactListener->onContactPreSolve = CC_CALLBACK_2(BattleScene::onPhysicContactPreSolve, this);
	contactListener->onContactPostSolve = CC_CALLBACK_2(BattleScene::onPhysicContactPostSlove, this);
	dispath->addEventListenerWithSceneGraphPriority(contactListener, this);
	

}

void BattleScene::update(float delta)
{
	//log("Current Hp: %d", _characterCurentHp);
	_checkTime += delta;
	//TODO: uncomment to send unit status every 0.07 seconds
	if (_checkTime >= 0.07 && _gameMode == MULTI_MODE) {
		float angle = _mainCharacterIconInMiniMap->getRotation();
		int direc = detectDirectionBaseOnTouchAngle(angle);

		sendMoveEvent(direc, angle, testObject->getOnMovingFlg());
		_checkTime = 0;
	}
	updateMiniMap();
	updateTime();
	updateHpAndMpViewLabel();
	checkForAutoAttack();
	auto child = _battleBackround->getChildren();
	for (auto &sp : child)
	{
		if (sp->getTag() == DRAW_UNIT) {
			sp->setPosition(testObject->getPosition());
		}
	}

	//logic for detect unit in area when main character moved
	if (_showSkillTargetFlag) {
		for (auto &a : _allEnemyUnitSprite)
		{
			a->setColor(Color3B::WHITE);
		}
		vector<int> detectedUnit = detectUnitInAoe(testObject,_onSelectSkillData, _allEnemyUnitSprite, false);
		for (auto &c : detectedUnit)
		{
			_allEnemyUnitSprite[c]->setColor(Color3B::RED);
		}
	}
	for (int i = 0; i < _allEnemyUnitSprite.size(); i++)
	{
		if (_allEnemyUnitData[i].isStun)
		{
			_allEnemyUnitSprite[i]->getPhysicsBody()->setVelocity(Vect::ZERO);
			_allEnemyUnitSprite[i]->stopActionByTag(_allEnemyUnitSprite[i]->getTag());
		}
	}

	fakeZOrder();

	testMoveLogic(testObject, _currentPlayerTeamFlg,&_saveOldTitle);
	if (_gameMode == SOLO_MODE)
	{
		for (int i = 0; i < _neutralUnitList.size(); i++)
		{
			if (_neutralUnitList[i]->getTag() != 0) {
				testMoveLogic(_neutralUnitList[i], _neutralUnitList[i]->getTag(), &_saveOldPosOfNeutral[i]);
			}
		}
	}

	checkForWarp();

	checkForUsingCannon();

}

void BattleScene::neutralUnitMoveInSoloMod()
{

	for (auto &neutral : _neutralUnitList)
	{
		float x = random(-1.0f, 1.0f)*1000;
		float y = random(-1.0f, 1.0f)*1000;
		Vec2 vec = Vec2(x, y);
		int direc = detectDirectionBaseOnTouchAngle(-vec.getAngle()*RAD_DEG + 90);
		neutral->stopMoveAction();
		neutral->getPhysicsBody()->setVelocity(Vec2::ZERO);
		neutral->actionMoveCharacter(direc);
		Vec2 force = Vec2(NEUTRAL_MOVE_SPEED * cos(vec.getAngle()), NEUTRAL_MOVE_SPEED * sin(vec.getAngle()));
		neutral->getPhysicsBody()->setVelocity(force);
	}
}

void BattleScene::testMoveLogic(Sprite* object, int teamFlg, vector<bool> *sendingFlg)
{
	if (_onRespwanFlg) return;
	vector<Vec2> savePos;
	auto pos = object->getPosition();
	//return in wrong position title case
	if (!checkPositionInsideMap(pos)) return;
	savePos.push_back(pos);
	auto cha = (Character*)object;
	if (cha->getBirdMode() == true)
	{
		auto birdMode = cha->getBirdModeIndex();
		if (birdMode <= 6) {
			auto nearPos = pos - Vec2(0, _myMap->getTileSize().height);
			if (checkPositionInsideMap(nearPos))
			{
				savePos.push_back(nearPos);
			}
		}

	}

	if (cha->getOnCannonLunchFlg()) {
		int s = savePos.size();
		Size titleSize = _myMap->getTileSize();
		Vec2 first = savePos.front();
		Vec2 last = savePos.back();
		int i = 1;
		while (i <= s)
		{
			Vec2 exPos1 = Vec2(first.x, first.y - i*titleSize.height);
			if (checkPositionInsideMap(exPos1)) {
				savePos.push_back(exPos1);
			}
			Vec2 exPos2 = Vec2(last.x, last.y + i* titleSize.height);
			if (checkPositionInsideMap(exPos2))
				savePos.push_back(exPos2);
			i++;
		}
	}
	


	for (int i = 0; i < savePos.size(); i ++)
	{
		Vec2 titlePos = getTitleCoorForPosition(savePos[i]);
		//return in nearly tower title case
		vector<Sprite*> temp;
		for (auto &tower : _neutralTowerList)
		{
			temp.push_back(tower);
		}

		if (checkTitleNearObject(titlePos, temp, 8)) continue;;
		temp.clear();
		for (auto &cannon : _cannonList)
		{
			temp.push_back(cannon);
		}
		if (checkTitleNearObject(titlePos, temp, 4)) continue;;
		auto title = _mapLayer->getTileAt(titlePos);

		if (title == nullptr) {
			continue;
		}
		if (title->getName() == "disable") continue;

		if (teamFlg == TEAM_FLG_BLUE) {
			if (title->getColor() != Color3B::GREEN)
			{				
				if (_gameMode == MULTI_MODE)
				{
					if (title->getName() == "sending") continue;
					log("send test move event");
					//sendingFlg->push_back(true);
					title->setName("sending");
					BattleAPI::getInstance()->sendTestMoveLogic(titlePos);
					continue;
				}
				else {
					title->setColor(Color3B::GREEN);
					auto mTittle = _miniLayer->getTileAt(titlePos);
					mTittle->setColor(Color3B::GREEN);
					continue;
				}
			}
		}
		else {
			if (title->getColor() != Color3B::ORANGE)
			{
				if (_gameMode == MULTI_MODE) {
					if (title->getName() == "sending") continue;
					log("send test move event");
					title->setName("sending");
					//sendingFlg->push_back(true);
					BattleAPI::getInstance()->sendTestMoveLogic(titlePos);
					continue;
				}
				else {
					title->setColor(Color3B::ORANGE);
					auto mTittle = _miniLayer->getTileAt(titlePos);
					mTittle->setColor(Color3B::ORANGE);
					continue;
				}
			}
		}
	}
	


}

bool BattleScene::checkTitleNearObject(Vec2 titleCoor, vector<Sprite*> ObjectList, int offset)
{
	for (int i = 0; i < ObjectList.size(); i++)
	{
		auto towerCoor = getTitleCoorForPosition(ObjectList[i]->getParent()->getPosition()) + Vec2(0, offset/4);
		if (titleCoor.x >= towerCoor.x - offset/2 && titleCoor.x < towerCoor.x + offset/2 && titleCoor.y >= towerCoor.y - offset/2 && titleCoor.y < towerCoor.y + offset/2)
		{
			return true;
		}

	}


	return false;
}



void BattleScene::changeTitlesNearObject(Sprite* object, int type, int offset)
{
	Color3B color;
	switch (type)
	{
	case  0:
		color = Color3B::WHITE;
		break;
	case 1:
		color = Color3B::GREEN;
		break;
	case 2:
		color = Color3B::ORANGE;
		break;
	}
	Vec2 titleSize = _myMap->getTileSize();
	Vec2 posCoor = getTitleCoorForPosition(object->getParent()->getPosition() - Vec2(0, titleSize.y*offset/4));
	for (int i = posCoor.x - offset/2; i < posCoor.x + offset/2; i++)
	{
		for (int j = posCoor.y - offset/2; j < posCoor.y + offset/2; j ++)
		{
			_mapLayer->getTileAt(Vec2(i, j))->setColor(color);
			_miniLayer->getTileAt(Vec2(i, j))->setColor(color);
		}
	}
}

void BattleScene::checkForUsingCannon()
{
	for (int i = 0; i < _cannonList.size(); i ++)
	{
		if (_cannonList[i]->getDisableFlg() || _cannonList[i]->getCurrentTeamId() != _currentPlayerTeamFlg) continue;
		Vec2 distan = _cannonList[i]->getPosition() - testObject->getPosition();
		if (distan.length() < Vec2(_myMap->getTileSize()).length())
		{
			testObject->stopMoveAction();
			readyForLunch(_cannonList[i], i);
			break;
		}
	}
}

void BattleScene::checkForWarp()
{
	if (_onWarping) return;
	for (int i = 0; i < _wormHoleList.size(); i++)
	{
		Vec2 objectPos = testObject->getPosition();
		Vec2 holePos = _wormHoleList[i]->getParent()->getPosition();
		if ((holePos - objectPos).length() < 80 && strcmp(_wormHoleList[i]->getName().c_str(),"OPEN") ==0) {
			_onWarping = true;
			int outGate = -1;
			switch (i)
			{
			case 0:
				outGate = 2;
				break;
			case 1:
				outGate = 3;
				break;
			case 2:
				outGate = 0;
				break;
			case 3:
				outGate = 1;
				break;
			}
			//send warp event
			if (_gameMode == MULTI_MODE)
			{
				BattleAPI::getInstance()->sendWarpEvent(i, outGate, [&, outGate](SIOClient *client, const string data) {
					_onWarping = false;
					testObject->setPosition(_wormHoleList[outGate]->getParent()->getPosition());
				});
			}
			else
			{
				_onWarping = false;
				testObject->setPosition(_wormHoleList[outGate]->getParent()->getPosition());
				closeWormHole(i, outGate);
				_wormHoleList[i]->runAction(Sequence::create(DelayTime::create(5.0f), CallFuncN::create(CC_CALLBACK_0(BattleScene::openWormHole, this, i, outGate)), nullptr));
			}
			return;
		}

	}
}

void BattleScene::closeWormHole(int ingate, int outgate)
{
	log("close");
	_wormHoleList[ingate]->stopActionByTag(WORMHOLDROTATE);
	_wormHoleList[ingate]->setName("CLOSED");
	_wormHoleList[outgate]->stopActionByTag(WORMHOLDROTATE);
	_wormHoleList[outgate]->setName("CLOSED");
}

void BattleScene::openWormHole(int ingate, int outgate)
{
	log("open");
	auto action = RepeatForever::create(RotateBy::create(1, 135));
	action->setTag(WORMHOLDROTATE);
	auto action2 = action->clone();
	action2->setTag(WORMHOLDROTATE);
	_wormHoleList[ingate]->runAction(action);
	_wormHoleList[ingate]->setName("OPEN");
	_wormHoleList[outgate]->runAction(action2);
	_wormHoleList[outgate]->setName("OPEN");
}

void BattleScene::attackLogicAndAnimation(Vec2 posDistan, int direc, int i, int dame)
{
	_mainCharacterIconInMiniMap->setRotation(-(posDistan.getAngle() * RAD_DEG) + 90);
	testObject->attackActionByTargetPosition(posDistan, _allAlliedUnitData[0].attack_speed, CC_CALLBACK_0(BattleScene::oneSecondAttackCallback, this), CC_CALLBACK_0(BattleScene::characterAttackCallback, this, i, dame));

	if (_moveMode == MOVE_CIRCLE)
	{
		_miniUnit->attackActionByTargetPosition(posDistan, _allAlliedUnitData[0].attack_speed);
	}

	_indexOfBeAttackEnemy = i;
}

void BattleScene::checkForAutoAttack()
{

	/************************************************************************/
	/* CHECK AUTO ATTACK OF MAIN UNIT                                       */
	/************************************************************************/
	/*Dead or stunning unit cannot attack*/
	if (_onRespwanFlg || _allAlliedUnitData[0].isStun || getCannonFlg()) return;
	//float area = IMAGE_SCALE*_autoAttackArea->getContentSize().width / 2 + 25;
	//Check for main character attack
	for (int i = 0; i < _allEnemyUnitSprite.size(); i++)
	{
		///Check for main character run attack animation

		auto posDistan = _allEnemyUnitSprite[i]->getPosition() - testObject->getPosition();
		if (i == _allAlliedUnitSprite.size() - 1) {
			posDistan = _allEnemyUnitSprite[i]->getParent()->getPosition() - testObject->getPosition();
		}
		int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
		if (posDistan.length() - _allEnemyUnitSprite[i]->getBoundingBox().size.width / 4 < ATTACK_AOE*_allAlliedUnitData[0].attack_range / 100.0f && _allEnemyUnitSprite[i]->isVisible())
		{

			if (/*testObject->getActionByTag(testObject->getCurrentAttackActionTag()) == nullptr && */_onDelayAttackFlg == false)
			{
				//auto call1 = CallFuncN::create(CC_CALLBACK_0(BattleScene::characterAttackCallback, this));
				_onDelayAttackFlg = true;
				testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
				testObject->stopMoveAction();
				if (_moveMode == MOVE_CIRCLE) {
					_miniUnit->stopMoveAction();
				}
				if (_gameMode == MULTI_MODE)
				{
					BattleAPI::getInstance()->sendAttackEvent(posDistan, _allAlliedUnitData[0], _allEnemyUnitData[i], [&, i, direc, posDistan](SIOClient *client, const string data)
					{
						if (_onDestructCalled) return;
						log("Battle attack callback with data: %s", data.c_str());
						Document doc;
						doc.Parse<0>(data.c_str());
						if (doc.HasParseError()) {
							log("ATTACK: Parse JSOn error");
							return;
						}
						attackLogicAndAnimation(posDistan, direc, i, doc["dame"].GetInt());

					});
				}
				else {
					int dame = caculDameRate(_allAlliedUnitData[0].element, _allEnemyUnitData[i].element)*random(0.85f, 1.0f) *(_allAlliedUnitData[0].attack - _allEnemyUnitData[i].defence);
					attackLogicAndAnimation(posDistan, direc, i, dame);
				}
			}
			break;
		}

	}
	/*this effect not active in tower
	*/
	for (int i = 0; i < _allAlliedUnitData.size()-1; i ++)
	{
		if ((_allAlliedUnitSprite[i]->getPosition() - Vec2((_currentPlayerTeamFlg - 1) * _myMap->getContentSize().width, _myMap->getContentSize().height/2)).length() < 150)
		{
			fountainRestoreEffect(_allAlliedUnitSprite[i], &_allAlliedUnitData, i);
		}
		else
		{
			_allAlliedUnitSprite[i]->stopActionByTag(FOUNTAIN_ACTION);
		}
	}

	for (int j = 0; j <_allEnemyUnitData.size() - 1; j++)
	{
		if ((_allEnemyUnitSprite[j]->getPosition() - Vec2((_currentEnemyTeamFlg - 1) * _myMap->getContentSize().width, _myMap->getContentSize().height)).length() < 150)
		{
			fountainRestoreEffect(_allEnemyUnitSprite[j], &_allEnemyUnitData, j);
		}
		else
		{
			_allEnemyUnitSprite[j]->stopActionByTag(FOUNTAIN_ACTION);
		}
	}

	/*Check for attack and get neutral tower*/
	for (int i = 0; i < _neutralTowerList.size(); i++)
	{
		//log("I: %d", i);
		int towerType = _neutralTowerList[i]->getTowerType();
		Vec2 towerPos = _neutralTowerList[i]->getParent()->getPosition() - Vec2(0, _neutralTowerList[i]->getBoundingBox().size.height /4);
		Vec2 distan = towerPos - testObject->getPosition();
		if ( towerType != _currentPlayerTeamFlg && !_onDelayAttackFlg && distan.length() - _neutralTowerList[i]->getBoundingBox().size.width / 4 < _allAlliedUnitData[0].attack_range) {
			_onDelayAttackFlg = true;
			testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
			testObject->stopMoveAction();
			if (_moveMode == MOVE_CIRCLE) {
				_miniUnit->stopMoveAction();
			}
			//log("SEnd attack neutral tower");
			int direc = detectDirectionBaseOnTouchAngle(-distan.getAngle()*RAD_DEG + 90);
			if (_gameMode == MULTI_MODE)
			{
				BattleAPI::getInstance()->sendNeutralTowerAttackEvent(_currentPlayerTeamFlg, i, distan, [&, i, direc, distan](SIOClient* client, const string repData) {
					//log("neutral tower callback");
					characterAttackAnimationLogic(distan, direc);
				});
			}
			else
			{
				characterAttackAnimationLogic(distan, direc, CC_CALLBACK_0(BattleScene::chatacterAttackNeutralCallback,this, i, 1));
			}
			break;
		}
		
	}

	/*Check for attack and get neutral units*/
	for (int i = 0; i < _neutralUnitList.size(); i++)
	{
		int unitType = _neutralUnitList[i]->getTag();
		Vec2 unitPos = _neutralUnitList[i]->getPosition();
		Vec2 distan = unitPos - testObject->getPosition();
		if (unitType != _currentPlayerTeamFlg && !_onDelayAttackFlg && distan.length() - _neutralUnitList[i]->getBoundingBox().size.width / 4 < _allAlliedUnitData[0].attack_range)
		{
			_onDelayAttackFlg = true;
			testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
			testObject->stopMoveAction();
			if (_moveMode == MOVE_CIRCLE) {
				_miniUnit->stopMoveAction();
			}
			int direc = detectDirectionBaseOnTouchAngle(-distan.getAngle() *RAD_DEG + 90);
			if (_gameMode == MULTI_MODE) {
				BattleAPI::getInstance()->sendNeutralUnitAttackEvent(_currentPlayerTeamFlg, i, distan, [&, i, direc, distan](SIOClient *client, const string data) {
					characterAttackAnimationLogic(distan, direc);
				});
			}
			else {
				characterAttackAnimationLogic(distan, direc,CC_CALLBACK_0(BattleScene::chatacterAttackNeutralCallback,this,i,2));
			}
			break;
		}

	}

	/*check for auto attack cannons*/
	for (int i = 0; i < _cannonList.size(); i ++)
	{
		int cannonTeam = _cannonList[i]->getCurrentTeamId();
		Vec2 cannonPos = _cannonList[i]->getParent()->getPosition();
		Vec2 distan = cannonPos - testObject->getPosition();
		if (cannonTeam != _currentPlayerTeamFlg && !_onDelayAttackFlg && distan.length() - 30 < _allAlliedUnitData[0].attack_range)
		{
			_onDelayAttackFlg = true;
			testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
			testObject->stopMoveAction();
			if (_moveMode == MOVE_CIRCLE) {
				_miniUnit->stopMoveAction();
			}
			int direc = detectDirectionBaseOnTouchAngle(-distan.getAngle() *RAD_DEG + 90);
			if (_gameMode == MULTI_MODE) {
				BattleAPI::getInstance()->sendCannonAttackEvent(_currentPlayerTeamFlg, i, distan, [&, i, direc, distan](SIOClient *client, const string data) {
					characterAttackAnimationLogic(distan, direc, CC_CALLBACK_0(BattleScene::characterAttackCannonCallback, this, i));
				});
			}
			else {
				characterAttackAnimationLogic(distan, direc, CC_CALLBACK_0(BattleScene::characterAttackCannonCallback, this, i));
			}
			break;
		}

	}
	 

	if (_gameMode == SOLO_MODE) {
		checkAutoAttackOfTower();
	}
}

void BattleScene::characterAttackAnimationLogic(Vec2 distan, int direc, AttackCallback callback)
{
	_mainCharacterIconInMiniMap->setRotation(-(distan.getAngle() * RAD_DEG) + 90);
	testObject->attackActionByTargetPosition(distan, _allAlliedUnitData[0].attack_speed, CC_CALLBACK_0(BattleScene::oneSecondAttackCallback, this), callback);
}

void BattleScene::chatacterAttackNeutralCallback(int neutralIndex, int neutralType)
{
	if (neutralType == 1)
	{
		Tower* tower = _neutralTowerList[neutralIndex];
		int tag = tower->getTag();
		tag += 1;
		if (tag < 5) {
			tower->setTag(tag);
		}
		else 
		{
			
			tower->setTag(0);
			tower->changeTowerType(_currentPlayerTeamFlg);
			changeTitlesNearObject(tower, _currentPlayerTeamFlg, 8);

		}
	}
	else 
	{
		Character* charac = _neutralUnitList[neutralIndex];
		int tag = charac->getSaveAtttackTime();
		tag += 1;
		if (tag < 5) {
			charac->setSaveAtttackTime(tag);
		}
		else
		{
			charac->setSaveAtttackTime(0);
			neutralUnitStatusChange(charac, _currentPlayerTeamFlg, neutralIndex);
		}
	}
}

void BattleScene::characterAttackCannonCallback(int cannonIndex)
{
	//add unit to cannn
	//change touch mode to lunch the unit
	int at = _cannonList[cannonIndex]->getAttackedTime();
	at += 1;
	if (at < 5) {
		_cannonList[cannonIndex]->setAttackedTime(at);
		return;
	}
	else {
		_cannonList[cannonIndex]->setAttackedTime(0);
		cannonStatusChange(_cannonList[cannonIndex], _currentPlayerTeamFlg);
		//ready for lunch
		readyForLunch(_cannonList[cannonIndex], cannonIndex);
	}
}

void BattleScene::readyForLunch(Cannon * cannon, int cannonIndex) {
	setCannonFlg(true);
	testObject->setPosition(cannon->getParent()->getPosition());
	testObject->setColor(Color3B::YELLOW);
	_onLunchCannonIndex = cannonIndex;
	//logic for ready lunch unit
}

void BattleScene::lunchObject(Touch *touch) {
	/*send data to server in multi mode*/
	if (_gameMode == MULTI_MODE)
	{
		BattleAPI::getInstance()->sendCannonLunchEvent(_currentPlayerTeamFlg, _onLunchCannonIndex, [&](SIOClient * client, const string data) {
			log("cannon lunch data");
		});
	}
	Vec2 desPos = _battleBackround->convertToNodeSpace(touch->getLocation());
	/*calculate distan*/
	Vec2 distan = desPos - testObject->getPosition();
	/*apply velocity*/
	testObject->getPhysicsBody()->setVelocity(Vec2(cos(distan.getAngle())* _allAlliedUnitData[0].move_speed * 3, sin(distan.getAngle()) * _allAlliedUnitData[0].move_speed * 3));
	setCannonFlg(false);
	testObject->runLunchingAction();

	if (_gameMode = SOLO_MODE) {
		auto cannon = _cannonList[_onLunchCannonIndex];
		cannon->setRotation(0);
		cannon->setFlippedX(false);
		cannon->disableCannon();
	}
}

void BattleScene::neutralUnitStatusChange(Character* unit, int team, int index) {
	Texture2D* texture;
	if (team == _currentPlayerTeamFlg) {
		texture = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/allied_icon.png");
	}
	else {
		if (team == _currentEnemyTeamFlg)
		{
			texture = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/enemy_icon.png");
		}
		else {
			texture = Director::getInstance()->getTextureCache()->addImage("image/screen/battle/neutral_icon.png");
		}
	}
	int type = 8;
	switch (team)
	{
	case 1:
		type = 7;
		break;
	case 2:
		type = 2;
		break;
	default:
		type = 9;

		break;
	}
	/*stop move animation of neutral unit*/
	unit->stopMoveAction();
	if(unit->getPhysicsBody() != nullptr) unit->getPhysicsBody()->setVelocity(Vec2::ZERO);

	/*change unit type*/
	unit->changeUnitType(type);
	unit->setTag(team);
	_neutralUnitIconInMiniMap[index]->setTexture(texture);
}

void BattleScene::cannonStatusChange(Cannon* cannon, int teamId)
{
	cannon->changeTeamStatus(teamId);
	changeTitlesNearObject(cannon, teamId, 4);
}


void BattleScene::checkAutoAttackOfTower()
{
	//in SOLO mode. enemy team have only one unit. That is tower.
	if (_enemyTowerAttackdelay) return;
	auto pos = _allEnemyUnitSprite[0]->getParent()->getPosition();
	
	auto distan = pos - testObject->getPosition();
	if (distan.length() <= _allEnemyUnitData[0].attack_range) {
		_enemyTowerAttackdelay = true;
		string uu = UserModel::getInstance()->getUuId().c_str();
		towerAttackLogic(_allEnemyUnitSprite.back(), _allEnemyUnitData.back(), _allAlliedUnitSprite, &_allAlliedUnitData, uu.c_str(), random(0.85f, 1.0f));
		runAction(Sequence::create(DelayTime::create(_allEnemyUnitData.back().attack_speed), CallFuncN::create([&](Ref* p){
			_enemyTowerAttackdelay = false;
		}), nullptr));
	}
}

void BattleScene::characterAttackCallback(int  i, int dame)
{
	//if main character die before atack event end.
	if (_onRespwanFlg) return;
	//check for enemy unit still alive
	if (_allEnemyUnitData[i].hp > 0) {
		if (dame <= 0) {
			dame = 1;
		}
		//show hp lost = final attack dame
		showAttackDame(dame, _allEnemyUnitSprite[i]->getPosition() + Vec2(0, 100), 1);

		_allEnemyUnitData[i].hp -= dame;
		//save dame deal info to battle result
		//saveDameInfo(dame, 0, i, _currentPlayerTeamFlg);

		//TODO
		if (_gameMode == MULTI_MODE)
		{
			sendDameDeal(dame, i, nullptr);
		}
		//check if enemy die
		if (_allEnemyUnitData[i].hp <= 0)
		{
			enemyDieAction(i);
			return;
		}
		//update enemy hp status bar
		_allEnemyHpBar[i]->setPercent(ceil((100.0f * _allEnemyUnitData[i].hp / _allEnemuUnitMaxHp[i])));
		

	}
	else {
		//enemyDieAction(_indexOfBeAttackEnemy);
	}
}
void BattleScene::oneSecondAttackCallback()
{
	_onDelayAttackFlg = false;
}

void BattleScene::unitDieAction(Sprite* unitSprite, vector<UserUnitInfo>* processUnitList, int index)
{
	//unitSprite->stopAllActions();
	while (unitSprite->getChildByTag(BUFF_STATUS_TAG) != nullptr)
	{
		unitSprite->removeChildByTag(BUFF_STATUS_TAG);
	}
	processUnitList->at(index).isStun = false;
	unitSprite->setVisible(false);

	auto action = Sequence::create(DelayTime::create(5), CallFuncN::create(CC_CALLBACK_1(BattleScene::unitRespwanAction, this, unitSprite, processUnitList, index)), nullptr);
	action->setTag(ENEMY_RESPAW_ACTION_TAG);
	unitSprite->runAction(action);

}

void BattleScene::unitRespwanAction(Ref* pSender, Sprite* unitSprite, vector<UserUnitInfo>* processUnitList, int index)
{
	auto unit = BattleModel::getInstance()->getUnitInforByUuid(processUnitList->at(index).uuid.c_str());
	unitSprite->setPosition(Vec2(unit.position_x, unit.position_y));
	unitSprite->setVisible(true);
	_allEnemyIconInMinimap[index]->setVisible(true);
	processUnitList->at(index).hp = UserUnitModel::getInstance()->getUnitInfoById(processUnitList->at(index).mst_unit_id).hp;
	updateSlider();
}

void BattleScene::enemyDieAction(int id)
{
	//_allEnemyUnitSprite[id]->stopAllActions();
	while (_allEnemyUnitSprite[id]->getChildByTag(BUFF_STATUS_TAG) != nullptr)
	{
		_allEnemyUnitSprite[id]->removeChildByTag(BUFF_STATUS_TAG);
	}
	_allEnemyUnitData[id].isStun = false;
	_allEnemyAttachDelay[id] = false;
	_allEnemyUnitSprite[id]->setVisible(false);
	_allEnemyIconInMinimap[id]->setVisible(false);
	saveKillDeadInfo(0, id, _currentPlayerTeamFlg);
	/*SEND SKILLDEAD DATA TO SERVER*/
	if (_gameMode == MULTI_MODE)
	{
		string uu = UserModel::getInstance()->getUuId().c_str();
		auto titleCoor = getTitleCoorForPosition(_allEnemyUnitSprite[id]->getPosition());
		sendKillDead(uu.c_str(), _allEnemyUnitData[id].uuid,titleCoor, nullptr);
	}
	else {
		//auto pos = 
		//change title in local map
	}
	_indexOfBeAttackEnemy = -1;
	testObject->stopActionByTag(_currentAttackActionTag);
	/*if (id == _allEnemyUnitData.size() - 1) {
		endBattle();
	}*/
	enemyRespawAction(id);
	/* now it will checked on server. Not client*/
	/*_enemyTeamTotalDead += 1;
	if (_enemyTeamTotalDead == 5)
	{
		endBattle(_currentPlayerTeamFlg);
	}*/
}
void BattleScene::enemyAttackCallback(Ref *pSEnder, int i)
{
	if (_onRespwanFlg) return;
	Sprite *_sprite = (Sprite*)pSEnder;
	_sprite->stopActionByTag(1111);
	//log("enemy");
	if (_allAlliedUnitData[0].hp > 0)
	{
		if (!_onRespwanFlg)
		{
			int dame = _allEnemyUnitData[i].attack - _allAlliedUnitData[0].defence;
			float defaultDameRate = caculDameRate(_allEnemyUnitData[i].element, _allAlliedUnitData[0].element);

			dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);
			if (dame <= 0) {
				dame = 1;
			}
			if (dame <= 0)
			{
				dame = 1;
			}
			showAttackDame(dame, testObject->getPosition() + Vec2(0, 100), 2);
			_allAlliedUnitData[0].hp -= dame;
			saveDameInfo(dame, i, 0, _currentEnemyTeamFlg);
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			if (_allAlliedUnitData[0].hp <= 0) {
				runRespawnAction(_allEnemyUnitData[i].uuid.c_str());
				return;
			}
			//log("Percent: %d", ceil(float(float(_characterCurentHp) / float(_unitData.hp)) * 100));
			_mainCharacterMiniHpBar->setPercent(ceil((100.0f * _allAlliedUnitData[0].hp / _allAlliedUnitMaxHp[0])));
			_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());
			
		}
	}
	else {
		runRespawnAction(_allEnemyUnitData[i].uuid.c_str());
	}
	

}

void BattleScene::showAttackDame(int dameValue, Vec2 pos,int type)
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

void BattleScene::runRespawnAction(string killerUuid)
{
	if (_onRespwanFlg) return;
	_onRespwanFlg = true;
	//FOR sever fix position
	auto pos = getTitleCoorForPosition(testObject->getPosition());
	if (_gameMode == MULTI_MODE)
	{
		
		BattleAPI::getInstance()->sendDeadEvent(_allAlliedUnitData[0], pos, [&](SIOClient* client, const std::string json) {

			log("Dead event callback From server with data: %s", json.c_str());

		});
	}
	else {
		//disable title
		Color3B color = Color3B::WHITE;
		if (_currentPlayerTeamFlg == TEAM_FLG_BLUE) {
			color = Color3B::BLUE;
		}
		else {
			color = Color3B::RED;
		}
		for (int x = pos.x - 1; x <= pos.x; x++)
		{
			for (int y = pos.y; y <= pos.y + 1; y++)
			{
				auto title = _mapLayer->getTileAt(Vec2(x, y));
				title->setColor(color);
				title->setName("disable");
				auto mTitle = _miniLayer->getTileAt(Vec2(x, y));
				mTitle->setColor(color);
			}
		}
	}

		auto timeLb = Label::createWithSystemFont("5", JAPANESE_FONT_1_HEAVY, 150);
		_battleBackround->addChild(timeLb, 1000);
		timeLb->setPosition(testObject->getPosition());
		timeLb->setColor(Color3B::RED);
		/* Move skill area sprite
		*/
		while (_battleBackround->getChildByTag(DRAW_UNIT))
		{
			_battleBackround->removeChildByTag(DRAW_UNIT);
		}
		/*reset enemy unit color*/
		for (auto& e : _allEnemyUnitSprite)
		{
			e->setColor(Color3B::WHITE);
		}
		_mainCharacterMiniHpBar->setPercent(100);
		_mainCharacterHpBar->setPercent(100);
		_mainCharacterMpBar->setPercent(100);
		_allAlliedUnitData[0].hp = _allAlliedUnitMaxHp[0];
		_allAlliedUnitData[0].mp = _mainCharacerMaxMp;

		_battleBackround->stopActionByTag(121);
		testObject->setVisible(false);
		testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
		testObject->setPosition(Vec2(-200, _myMap->getContentSize().height/2));
		auto action = Repeat::create(Sequence::create(DelayTime::create(1), CallFuncN::create([&](Ref *pSender){
			Label* lb = dynamic_cast<Label*>(pSender);
			int t = DataUtils::stringToFloat(lb->getString());
			lb->setString(DataUtils::numberToString(t - 1));
		}), nullptr), 5);
		auto action2 = CallFuncN::create([&](Ref *pSEnder){
			Label* lb = dynamic_cast<Label*>(pSEnder);
			lb->removeFromParent();

			//Zoomout mini Map
			if (_onZoomMiniMap) {
				//_miniTMXMap->setPosition(Vec2(_visibleSize) - Vec2(15, 65));
				_miniTMXMap->setPosition(Vec2(_miniTMXMap->getContentSize().width + 15, _visibleSize.height - 15));
				_miniTMXMap->setScale(1.0f);
				_onZoomMiniMap = false;
			}

			if (_selectRect->isVisible() == false) {
				_selectRect->setVisible(true);
			}
			if (_gameMode == MULTI_MODE)
			{
				auto unitData = BattleModel::getInstance()->getUnitInforByUuid(UserModel::getInstance()->getUuId());
				testObject->setPosition(Vec2(unitData.position_x, unitData.position_y));
				testObject->rotateCharacter(unitData.direction);
			}
			else {
				testObject->setPosition(Vec2(100, _myMap->getBoundingBox().size.height / 2));
				testObject->rotateCharacter(2);
			}
			//testObject->setPosition(Vec2(_visibleSize.width, 100)); //need to change to value base on callback from server
			testObject->setVisible(true);
			auto follow = Follow::create(testObject, Rect(FOLLOW_MARGIN, 0, _myMap->getContentSize().width - FOLLOW_MARGIN, _myMap->getContentSize().height));
			follow->setTag(121);
			_battleBackround->runAction(follow);

			auto action3 = Spawn::create(Blink::create(0.6f, 6), Sequence::create(DelayTime::create(RESPAWN_DELAY), CallFuncN::create(CC_CALLBACK_0(BattleScene::removeRespawnFlg, this)), nullptr), nullptr);
			testObject->runAction(action3);
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			//SEND RESPAWN ACTION

		});
		timeLb->runAction(Sequence::create(action, action2, nullptr));

		if (!_onZoomMiniMap) {
			//zoom in minimap
			_miniTMXMap->setScale((_visibleSize.width - 150) / _miniTMXMap->getContentSize().width);
			_miniTMXMap->setPosition(Vec2(_visibleSize.width - 75, _visibleSize.height - 50));
			_onZoomMiniMap = true;
			_selectRect->setVisible(false);
		}
	
}
void BattleScene::removeRespawnFlg()
{
	testObject->setVisible(true);
	_onRespwanFlg = false;
}

void BattleScene::updateTime()
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
string BattleScene::makeTimeString(int second) {
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


bool BattleScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	auto touchPoint = touch->getLocation();
	if (_miniTMXMap->getBoundingBox().containsPoint(touchPoint)) {
		if (!_onZoomMiniMap) {
			_miniTMXMap->setScale((_visibleSize.width - 150) / _miniTMXMap->getContentSize().width);
			_miniTMXMap->setPosition(Vec2(_visibleSize.width - 75, _visibleSize.height -50));
			_onZoomMiniMap = true;
		}
		else
		{
			//_miniTMXMap->setPosition(Vec2(_visibleSize) - Vec2(15, 65));
			_miniTMXMap->setPosition(Vec2(_miniTMXMap->getContentSize().width + 15, _visibleSize.height - 15));
			_miniTMXMap->setScale(1.0f);
			_onZoomMiniMap = false;
		}

		return false;
	}

	if (getCannonFlg()) {
		_touchMoveBeginSprite->setTexture("image/screen/battle/target.png");
		_touchMoveBeginSprite->setVisible(true);
		_touchMoveBeginSprite->setPosition(touchPoint);
		return true;
	}

	_touchStartPoint = touchPoint;
	auto testPos = _battleBackround->convertToNodeSpace(_touchStartPoint);
	auto cor = getTitleCoorForPosition(testPos);

	if (_moveDisableFlg == false && _allAlliedUnitData[0].isStun == false) {

		_touchMoveBeginSprite->setTexture("image/screen/battle/ui_move.png");
		_touchMoveEndSprite->setTexture("image/screen/battle/ui_move_end.png");
		if (_moveMode == MOVE_MANUAL)
		{
			_touchMoveBeginSprite->setPosition(_touchStartPoint);
			_touchMoveBeginSprite->setVisible(true);
		}

		if (_moveMode == MOVE_CIRCLE)
		{
			if (_miniCircle->getBoundingBox().containsPoint(touch->getLocation())) {
				_touchMoveBeginSprite->setPosition(_miniCircle->getPosition());
				_touchMoveBeginSprite->setVisible(true);

				_touchMoveEndSprite->setPosition(touch->getLocation());
				_touchMoveEndSprite->setVisible(true);
			}
			else {
				return false;
			}

			auto actionLongTap = Sequence::create(DelayTime::create(0.15f), CallFuncN::create([&, touch](Ref* pSender){
				auto distanVector = touch->getLocation() - _miniCircle->getPosition();

				testObject->setMoveMode(4);
				testObject->moveActionByVector(touch->getLocation() - _miniCircle->getPosition());
				_checkOneTapMoveFlg = true;
				_checkLongTapMoveFlg = true;

				_mainCharacterIconInMiniMap->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
				int direc = detectDirectionBaseOnTouchAngle(_mainCharacterIconInMiniMap->getRotation());
				if (direc != 0)
				{
					_miniUnit->actionMoveCharacter(direc);
				}

				// Doi mau cho 2 vong tron di chuyen neu su dung longTap
				_touchMoveBeginSprite->setTexture("image/screen/battle/ui_move_long.png");
				_touchMoveEndSprite->setTexture("image/screen/battle/ui_move_end_long.png");

			}), nullptr);
			actionLongTap->setTag(STOP_LONG_MOVE);
			testObject->runAction(actionLongTap);
		}
	}
	else {
		//_selectTargetSprite->setPosition(testObject->getPosition());
		//_selectTargetSprite->setVisible(true);
	}

	return true;
}

void BattleScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	// Xac dinh vector giua diem touchMove va touchBegin
	//Stunning unit cannot move
	if (_allAlliedUnitData[0].isStun) return;
	if (getCannonFlg()) {
		_touchMoveBeginSprite->setPosition(touch->getLocation());
		auto cannon = _cannonList[_onLunchCannonIndex];
		auto distan = _battleBackround->convertToNodeSpace(touch->getLocation()) - testObject->getPosition();
		float angle = -distan.getAngle() * RAD_DEG;
		log("Angle: %f", angle);
		if (angle > 90 || angle < -90) {
			cannon->setFlippedX(true);
			angle = 180 + angle;
		}
		else {
			cannon->setFlippedX(false);
		}
		cannon->setRotation(angle);

		return;
	}

	Vec2 distanVector;
	if (_moveMode == MOVE_MANUAL)
	{
		distanVector = touch->getLocation() - _touchStartPoint;
	}

	if (_moveMode == MOVE_CIRCLE)
	{
		distanVector = touch->getLocation() - _miniCircle->getPosition();
	}


	if ((touch->getLocation() - _touchStartPoint).length() < 20) {
		_checkOneTapMoveFlg = false;
		return;
	}

	if (_moveMode == MOVE_MANUAL) {
		_touchMoveEndSprite->setVisible(true);
		if (distanVector.length() < 200) {
			_touchMoveEndSprite->setPosition(touch->getLocation());
		}
		else {
			_touchMoveEndSprite->setPosition(_touchStartPoint + Vec2(200 * cos(distanVector.getAngle()), 200 * sin(distanVector.getAngle())));
		}
	}

	if (_moveMode == MOVE_CIRCLE) {
		_touchMoveEndSprite->setVisible(true);

		if (_miniCircle->getBoundingBox().containsPoint(touch->getLocation())) {
			_touchMoveEndSprite->setPosition(touch->getLocation());
		}
		else {
			_touchMoveEndSprite->setPosition(_miniCircle->getPosition() + Vec2(circleX / 2 * cos(distanVector.getAngle()), circleY / 2 * sin(distanVector.getAngle())));
		}
	}

	if (distanVector.length() < _touchMoveBeginSprite->getContentSize().width / 6 && _moveMode != MOVE_CIRCLE) {
		testObject->stopMoveAction();
		testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
		return;
	}

	/////////////////MOVE WITH TESTOBJECT
	testObject->stopAllActionsByTag(STOP_LONG_MOVE);
	if (_moveMode == MOVE_MANUAL || _moveMode == MOVE_CIRCLE) {
		_checkOneTapMoveFlg = true;
		testObject->setMoveMode(_moveMode);
		testObject->moveActionByVector(distanVector);

		_mainCharacterIconInMiniMap->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);
		int direc = detectDirectionBaseOnTouchAngle(_mainCharacterIconInMiniMap->getRotation());
		if (direc != 0)
		{
			if (_moveMode == MOVE_CIRCLE)
			{
				_miniUnit->actionMoveCharacter(direc); // Xoay miniUnit
			}
		}
	}
	else {

	}
}
/*
7--8--9
4-YOU-6
1--2--3
*/

int BattleScene::detectDirectionBaseOnTouchAngle(float angle)
{
	/*if(caculAvgAngle(0,angle)) return 8;
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
	return 8;*/
	//new direction
	if (caculAvgAngle(0, angle)) return 1;
	if (caculAvgAngle(90, angle)) return 4;
	if (caculAvgAngle(180, angle)) return 7;
	if (caculAvgAngle(270, angle) || caculAvgAngle(-90, angle)) return 10;

	//default
	return 7;

}
bool BattleScene::caculAvgAngle(int avg, float angle)
{
	if (angle > avg - 44.9f && angle < avg + 44.9f) return true;
	return false;
}
void BattleScene::updateMiniMap()
{
	if (testObject == nullptr) return;
	auto objectPos = testObject->getPosition();

	float positionXScaleRate = _miniTMXMap->getContentSize().width / (_myMap->getContentSize().width);
	float positionYScaleRate = _miniTMXMap->getContentSize().height / (_myMap->getContentSize().height);
	_selectRect->setPosition(Vec2(objectPos.x*positionXScaleRate,objectPos.y*positionYScaleRate));
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyIconInMinimap[i]->setPosition(Vec2(_allEnemyUnitSprite[i]->getPositionX()*positionXScaleRate, _allEnemyUnitSprite[i]->getPositionY()*positionYScaleRate));
	}
	for (int i = 1; i < _allAlliedUnitData.size(); i++)
	{
		_allAlliedUnitIconInMiniMap[i-1]->setPosition(Vec2(_allAlliedUnitSprite[i]->getPositionX()*positionXScaleRate, _allAlliedUnitSprite[i]->getPositionY()*positionYScaleRate));
	}
	for (int i = 0; i < _neutralUnitList.size(); i++)
	{
		_neutralUnitIconInMiniMap[i]->setPosition(Vec2(_neutralUnitList[i]->getPositionX()*positionXScaleRate, _neutralUnitList[i]->getPositionY()* positionYScaleRate));
	}
	
}


void BattleScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	
	if (_allAlliedUnitData[0].isStun) return;

	_touchMoveBeginSprite->setTexture("image/screen/battle/ui_move.png");
	_touchMoveEndSprite->setTexture("image/screen/battle/ui_move_end.png");
	_touchMoveBeginSprite->setVisible(false);
	_touchMoveEndSprite->setVisible(false);

	/*logic for lunch unit*/
	if (getCannonFlg())
	{
		lunchObject(touch);
		return;
	}

	/////////////////////////////////////////////////////////////////////////
	// LOGIC
	/////////////////////////////////////////////////////////////////////////
	testObject->stopAllActionsByTag(STOP_LONG_MOVE);
	if ((_moveMode == MOVE_MANUAL) || (_moveMode == MOVE_CIRCLE && _checkOneTapMoveFlg == true) || (_moveMode == MOVE_CIRCLE && _checkLongTapMoveFlg == true))
	{
		/* Day la truong hop MOVE_MANUAL || MOVE_CIRCLE_MANUAL || MOVE_CIRCLE_LONGTAP */
		testObject->stopMoveAction();
		testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
		if (_moveMode == MOVE_CIRCLE)
		{
			_miniUnit->stopMoveAction();
			
		}
		sendMoveEndEvent();
		_checkOneTapMoveFlg = false;
		_checkLongTapMoveFlg = false;

		return;
	}
	else
	{
		if (_moveMode == MOVE_AUTO)
		{
			testObject->setMoveMode(1);
			//testObject->stopMoveAction();
			testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
			auto distanVector = _battleBackround->convertToNodeSpace(touch->getLocation()) - testObject->getPosition();
			/*test*/
			//auto vec = AStarPathFindingAlgorithm(testObject->getPosition(), testObject->getPosition() + distanVector);
			float time = distanVector.length() / _allAlliedUnitData[0].move_speed;
			_mainCharacterIconInMiniMap->setRotation(-distanVector.getAngle()*RAD_DEG + 90);
			int direc = detectDirectionBaseOnTouchAngle(-distanVector.getAngle()*RAD_DEG + 90);
			if (testObject->getCurrentMoveActionTag() != direc)
			{
				testObject->actionMoveCharacter(direc);
			}
			/*auto moveAction = Sequence::create(MoveBy::create(time, distanVector), CallFuncN::create([&](Ref* p) {
				testObject->stopMoveAction();
			}), nullptr);
			moveAction->setTag(919);
			testObject->stopActionByTag(919);
			testObject->runAction(moveAction);*/
			testObject->getPhysicsBody()->setVelocity(Vec2::ZERO);
			/*Move by Apply Force*/
			//Vec2 force = Vec2(250 * cos(distanVector.getAngle()), 250 * sin(distanVector.getAngle()));

			//testObject->getPhysicsBody()->applyImpulse(force * 7000);
			/*Move by Veloc vector. Same with force*/
			Vec2 force = Vec2(_allAlliedUnitData[0].move_speed * SPEED_MULTIPLE * cos(distanVector.getAngle()), _allAlliedUnitData[0].move_speed * SPEED_MULTIPLE * sin(distanVector.getAngle()));
			testObject->getPhysicsBody()->setVelocity(force);
			
			return;

		}

		if (_moveMode == MOVE_CIRCLE)
		{
			if (_miniCircle->getBoundingBox().containsPoint(touch->getLocation())) {

				//log("Day la oneTap circle");
				testObject->setMoveMode(5);
				_miniUnit->setMoveMode(5);

				float moveTime;
				if (_circleProperty == MOVE_CIRCLE_TIME)
				{
					moveTime = 1;
				}
				if (_circleProperty == MOVE_CIRCLE_DISTANCE)
				{
					moveTime = (float)(MOVE_DISTANCE / _allAlliedUnitData[0].move_speed);
				}
				testObject->setMoveOneTapTime(moveTime);
				_miniUnit->setMoveOneTapTime(moveTime);
				Vec2 space = (touch->getLocation() - _miniCircle->getPosition());

				_mainCharacterIconInMiniMap->setRotation(-(space.getAngle() * RAD_DEG) + 90);

				testObject->moveActionByVector(space);
				_miniUnit->moveActionByVector(space);

			}
			else {
				return;
			}
		}

	}
	
}

void BattleScene::menuButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		auto dialog = OptionDialog::create(CC_CALLBACK_2(BattleScene::optionDecideCallback, this), CC_CALLBACK_2(BattleScene::optionCancelCallback, this));
		getParent()->addChild(dialog);
		break; 
	}

	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}



void BattleScene::savePhysicWorld(PhysicsWorld *world)
{
	_myWorld = world;
}

/*This function will be change in future with GAF animation file or another unit image
In my opinion, animation image path must be store in database or something have sane struct
*/
void BattleScene::changeAnimationImagePathByUnitId(int unitId)
{

	switch (unitId)
	{
	case 1:
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	case  3:
		_moveImagePath = "image/unit_new/move/green/";
		_attackImagePath = "image/unit_new/attack/green/";
		break;
	case 4:
		_moveImagePath = "image/unit_new/move/blue/";
		_attackImagePath = "image/unit_new/attack/blue/";
		break;
	case 5:
		_moveImagePath = "image/unit_new/move/black/";
		_attackImagePath = "image/unit_new/attack/black/";
		break;
	case 2:
		_moveImagePath = "image/unit_new/move/purple/";
		_attackImagePath = "image/unit_new/attack/purple/";
		break;
	default:
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	}
}

void BattleScene::createRandomRock()
{
	Texture2D *textureStone = Director::getInstance()->getTextureCache()->addImage("stone.png");
	Texture2D *textureTree = Director::getInstance()->getTextureCache()->addImage("tree.png");
	for (int i = 1; i < 6; i++)
	{
		auto sp = Sprite::createWithTexture(textureStone);
		MyBodyParser::getInstance()->parseJsonFile("json/stone.json");
		auto body = MyBodyParser::getInstance()->bodyFormJson(sp, "stone");
		body->setDynamic(false);
		body->getShape(0)->setRestitution(0);
		body->getShape(0)->setFriction(0);
		body->setGravityEnable(false);
		body->setContactTestBitmask(0x1);
		sp->setPhysicsBody(body);
		sp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sp->setTag(2);
		sp->setPosition(Vec2(random(0.1f, 0.9f)*_myMap->getContentSize().width,random(0.1f,0.9f)*_myMap->getContentSize().height));
		_allStone.push_back(sp);
		_battleBackround->addChild(_allStone.back(),ceil(_visibleSize.height * 3 - sp->getPositionY()));
	}
	for (int i = 1; i < 6; i++)
	{
		auto tree = Sprite::createWithTexture(textureTree);
		MyBodyParser::getInstance()->parseJsonFile("json/tree.json");
		auto body = MyBodyParser::getInstance()->bodyFormJson(tree, "tree");
		body->setDynamic(false);
		body->getShape(0)->setRestitution(0);
		body->setGravityEnable(false);
		body->setContactTestBitmask(0x1);
		tree->setPhysicsBody(body);
		tree->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
		tree->setTag(1);
		tree->setPosition(Vec2(random(0.1f, 0.9f)*_myMap->getContentSize().width, random(0.1f, 0.9f)*_myMap->getContentSize().height));
		_allStone.push_back(tree);
		_battleBackround->addChild(_allStone.back(),ceil(_visibleSize.height*3 - tree->getPositionY()));
	}
}

void BattleScene::fakeZOrder()
{
	for (auto &stone : _allStone)
	{
		if (caculAvgAngle(testObject->getPositionY() /*- testObject->getBoundingBox().size.height/2*/,stone->getPositionY()/*- stone->getBoundingBox().size.height/2*/) /*&& stone->getBoundingBox().intersectsRect(testObject->getBoundingBox())*/)
		{
			if (testObject->getPositionY()/* - testObject->getBoundingBox().size.height / 2 */> stone->getPositionY()/* - stone->getBoundingBox().size.height / 2*/)
			{
				testObject->setLocalZOrder(stone->getLocalZOrder() - 1);
			}
			else {
				testObject->setLocalZOrder(stone->getLocalZOrder()+ 1);
			}
			break;
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

bool BattleScene::onPhysicContactBegin(const PhysicsContact &contact)
{
	auto spriteA = contact.getShapeA()->getBody()->getNode();
	auto spriteB = contact.getShapeB()->getBody()->getNode();
	if ((spriteA->getTag() == BOUND_BORDER_TAG && spriteB == testObject) || (spriteA == testObject && spriteB->getTag() == BOUND_BORDER_TAG))
	{
		log("contact begin with wall");
	}


	return true;
}

bool BattleScene::onPhysicContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
// 	log("contact pre solve");
	auto spriteA = contact.getShapeA()->getBody()->getNode();
	auto spriteB = contact.getShapeB()->getBody()->getNode();

	return true;
}

void BattleScene::onPhysicContactPostSlove(PhysicsContact& contact, const PhysicsContactPostSolve& solve)
{
	auto spriteA = contact.getShapeA()->getBody()->getNode();
	auto spriteB = contact.getShapeB()->getBody()->getNode();

	if ((spriteA->getTag() == BOUND_BORDER_TAG && spriteB == testObject) || (spriteA == testObject && spriteB->getTag() == BOUND_BORDER_TAG))
	{
		contactWithWall();
	}

	if ((spriteA->getTag() == TOWER_TAG && spriteB == testObject) || (spriteA == testObject && spriteB->getTag() == TOWER_TAG))
	{
		contactWithTower();
	}
}

void BattleScene::contactWithWall()
{
	Vec2 veloc = testObject->getPhysicsBody()->getVelocity();
	int direc = detectDirectionBaseOnTouchAngle(-veloc.getAngle() * RAD_DEG+90);
	_mainCharacterIconInMiniMap->setRotation(-veloc.getAngle() * RAD_DEG + 90);
	testObject->stopMoveAction();
	if (_moveMode == MOVE_CIRCLE)
	{
		_miniUnit->stopMoveAction();
	}
	if (veloc.length() > 50)
	{
		testObject->actionMoveCharacter(direc);
		if (_moveMode == MOVE_CIRCLE)
		{
			_miniUnit->actionMoveCharacter(direc);
		}
	}
}

void BattleScene::contactWithTower()
{

}

void BattleScene::optionDecideCallback(Ref *pSEnder, Widget::TouchEventType type)
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
		if (_moveMode == MOVE_CIRCLE) {
			_circleType = UserDefault::getInstance()->getIntegerForKey(MOVE_CIRCLE_TYPE);
			_circleProperty = UserDefault::getInstance()->getIntegerForKey(NOVE_CIRCLE_PROPERTY);
			if (_circleType != 0 || _circleProperty != 0) {
				if (_checkMiniCircleFlg == true && _miniCircle != nullptr) {
					_miniCircle->removeFromParentAndCleanup(true);
					_miniUnit->removeFromParentAndCleanup(true);
					_checkMiniCircleFlg = false;
				}

				createMiniControlUnit(_circleType);
			}
		}
		else
		{
			if (_checkMiniCircleFlg == true && _miniCircle != nullptr) {
				_miniCircle->removeFromParentAndCleanup(true);
				_miniUnit->removeFromParentAndCleanup(true);
				_checkMiniCircleFlg = false;
			}
		}

		testObject->setMoveMode(_moveMode);
		_screenMode = UserDefault::getInstance()->getIntegerForKey(SCR_ORIEN_K);
		if (_screenMode == SCREEN_VERTICAL)
		{
			_miniTMXMap->setPosition(Vec2(_miniTMXMap->getContentSize().width + 15, _visibleSize.height - 15));
			_skillButtonParentnode->setPosition(Vec2(_visibleSize.width - 65, _visibleSize.height / 2));
			_skillButtonParentnode->setRotation(-90.0f);
			_topMenuSprite->setRotation(-90.0f);
			_topMenuSprite->setPosition(Vec2(15, 15));

			for (auto &tower : _neutralTowerList)
			{
					tower->getParent()->setRotation(-90.0f);
					tower->setPosition(Vec2(-40, 35));
			}
		}
		else {
			_miniTMXMap->setPosition(Vec2(_visibleSize.width - 15, _visibleSize.height - 15));
			_skillButtonParentnode->setPosition(Vec2(_visibleSize.width / 2, 65));
			_skillButtonParentnode->setRotation(0);
			_topMenuSprite->setRotation(0);
			_topMenuSprite->setPosition(Vec2(15, _visibleSize.height - 15));
			for (auto &tower : _neutralTowerList)
			{
				tower->getParent()->setRotation(0);
				tower->setPosition(Vec2(-15, 5));
			}
		}



		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BattleScene::optionCancelCallback(Ref *pSEnder, Widget::TouchEventType type)
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

void BattleScene::debugPhysicButtonCallback(Ref *pSEnder, Widget::TouchEventType type)
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
			//_autoAttackArea->setVisible(false);
		}
		else {
			_myWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
			//_autoAttackArea->setVisible(true);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BattleScene::checkMapTestButtonClick(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//changeAnimationImagePathByUnitId();
		if (_isCheckMapCallback) {
			_isCheckMapCallback = false;
			BattleAPI::getInstance()->sendCheckMapEvent([&](SIOClient *client, const string data) {
				_isCheckMapCallback = true;
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
void BattleScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (!_touchEndButton)
		{
			_touchEndButton = true;
			if (_gameMode == MULTI_MODE)
			{
				endBattle(_currentPlayerTeamFlg);
			}
			else {
				Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene(TEAM_FLG_BLUE)));
			}
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}

void BattleScene::birdButtonCallback(Ref *p, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		_birdMode = true;
		_testBirdIndex++;
		if (_testBirdIndex > 8) {
			_testBirdIndex = 0;
			testObject->setBirdMode(false);
			testObject->changeUnitType(_allAlliedUnitData[0].mst_unit_id);
			testObject->setScale(IMAGE_SCALE);
		}
		else {
			//testObject->getPhysicsBody()->setRotationEnable(true);
			testObject->birdMode(_testBirdIndex);
			if (_testBirdIndex == 1 || _testBirdIndex == 2) {
				//testObject->setScale(IMAGE_SCALE * 0.7f);
				return;
			}
			if (_testBirdIndex >= 3 && _testBirdIndex <= 6) {
				//testObject->setScale(IMAGE_SCALE * 0.8f);
				return;
			}
			//testObject->setScale(IMAGE_SCALE);
		}
		
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

UserUnitInfo BattleScene::getUnitDataFromDataBase(int unitId)
{

	return UserUnitModel::getInstance()->getUnitInfoById(unitId);

}

vector<UserSkillInfo> BattleScene::getUnitSkillFromDataBase(UserUnitInfo unitData)
{
	vector<UserSkillInfo> result;
	result.push_back(UserSkillModel::getInstance()->getSkillInfoById(unitData.skill1_id));
	result.push_back(UserSkillModel::getInstance()->getSkillInfoById(unitData.skill2_id));
	return result;
}

vector<UserUnitInfo> BattleScene::getUnitsDataListByUnitIdIdList(vector<Room_User_Unit_Model> idList)
{
	vector<UserUnitInfo> returnData;
	for (int i = 0; i < idList.size(); i++)
	{
		auto unitInfo = UserUnitModel::getInstance()->getUnitInfoById(idList[i].mst_unit_id);
		unitInfo.uuid = idList[i].uuid;
		returnData.push_back(unitInfo);
	}
	

	return returnData;
}
void BattleScene::sendMoveBeginEvent(float angle)
{
	//BattleAPI::getInstance()->sendMoveBeginEvent(_allAlliedUnitData[0], detectDirectionBaseOnTouchAngle(angle));
}

void BattleScene::sendMoveEvent(int direction, float angle, bool onMovingFlg)
{
	if (_gameMode == MULTI_MODE) 
	{
		BattleAPI::getInstance()->sendMoveEvent(_allAlliedUnitData[0], direction, angle, testObject->getPosition(), getUnitStatus(), onMovingFlg);
	}
}

void BattleScene::sendMoveEndEvent()
{
	if (_gameMode == MULTI_MODE) 
	{
		BattleAPI::getInstance()->sendMoveEndEvent(_allAlliedUnitData[0]);
	}
}


void BattleScene::autoRestoreHpAndMp()
{ 
	_allAlliedUnitData[0].mp += _allAlliedUnitData[0].mp_heal / RESTORE_MULTI;
	if (_allAlliedUnitData[0].mp > _saveMainStatusData.mp) {
		_allAlliedUnitData[0].mp = _saveMainStatusData.mp;
	}
	for (int j = 0; j < _allAlliedUnitData.size(); j ++)
	{
		_allAlliedUnitData[j].hp += _allAlliedUnitData[j].hp_heal / RESTORE_MULTI;
		if (_allAlliedUnitData[j].hp > _allAlliedUnitMaxHp[j])
		{
			_allAlliedUnitData[j].hp = _allAlliedUnitMaxHp[j];
		}
	}

	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyUnitData[i].hp += _allEnemyUnitData[i].hp_heal/RESTORE_MULTI;
		if (_allEnemyUnitData[i].hp > _allEnemuUnitMaxHp[i]) {
			_allEnemyUnitData[i].hp = _allEnemuUnitMaxHp[i];
		}
	}
	updateSlider();
	//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
}

void BattleScene::updateSlider()
{
	_mainCharacterMiniHpBar->setPercent(ceil((100.0f *_allAlliedUnitData[0].hp / _allAlliedUnitMaxHp[0])));
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());

	_mainCharacterMpBar->setPercent(ceil((100.0f * _allAlliedUnitData[0].mp / _mainCharacerMaxMp)));

	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyHpBar[i]->setPercent(ceil(( 100.0f * _allEnemyUnitData[i].hp / _allEnemuUnitMaxHp[i])));
	}
	for (int i = 0; i < _allAlliedUnitData.size(); i++)
	{
		_allAlliedUnitHpBar[i]->setPercent(ceil((100.0f * _allAlliedUnitData[i].hp / _allAlliedUnitMaxHp[i])));
	}
}
void BattleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	/*On repawn/ stunning unit cannot play skill*/
	if (_onRespwanFlg || _allAlliedUnitData[0].isStun) return;
	Button* bt = dynamic_cast<Button*>(pSender);
	int tag = bt->getTag();
	bt->stopActionByTag(TAG_SKILL_AOE);
 	
	UserSkillInfo skill;
	if (tag == TAG_SKILL_1 || tag == TAG_SKILL_2)
	{
		skill = _mainCharacterSkillData[tag - 1];
	}
	else {
		skill = _playerSkills[tag - 3];
	}
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		if (skill.range_distance > 0 && skill.mp_cost <= _allAlliedUnitData[0].mp)  {
			longPressAction(bt, skill);
			_showSkillTargetFlag = true;
		}
		
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//Progress Timer
		_showSkillTargetFlag = false;
		/*remove skill area and normal enemy unit color*/
		while (_battleBackround->getChildByTag(DRAW_UNIT))
		{
			_battleBackround->removeChildByTag(DRAW_UNIT);
		}
		for (auto &a : _allEnemyUnitSprite)
		{
			a->setColor(Color3B::WHITE);
		}

		if (skill.target_type == TARGET_ONE && skill.skill_type == TYPE_ATTACK && _indexOfBeAttackEnemy < 0) {
			log("Invalid attack target");
			return;
		}
		if (skill.mp_cost <= _allAlliedUnitData[0].mp){
			bt->setColor(Color3B::GRAY);
			bt->setTouchEnabled(false);
			bt->setEnabled(true);
			//BattleAPI::getInstance()->sendSkillEvent(skill, {1,2,3,4,5,6,7,8,9});
			playSkill(skill);
			showCoolDownTime(bt, skill.cooldown_time);
			bt->runAction(Sequence::create(DelayTime::create(skill.cooldown_time), CallFuncN::create([&, tag](Ref *p) {
				auto button = (Button*)p;
				button->setEnabled(true);
				button->setTouchEnabled(true);
				button->setColor(Color3B::WHITE);
			}), nullptr));
			
			
		}
		else {
			for (auto &a : _allEnemyUnitSprite)
			{
				a->setColor(Color3B::WHITE);
			}
			log("Not enough MP");
		}
		
		break; }
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		_showSkillTargetFlag = false;
		/*remove skill area and normal enemy unit color*/
		while (_battleBackround->getChildByTag(DRAW_UNIT))
		{
			_battleBackround->removeChildByTag(DRAW_UNIT);
		}
		for (auto &a : _allEnemyUnitSprite)
		{
			a->setColor(Color3B::WHITE);
		}

		break;
	default:
		break;
	}
}

void BattleScene::showCoolDownTime(Button *parentButton, int time)
{
	auto secondLabel = Label::createWithSystemFont(DataUtils::numberToString(time), "", 40);
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

/************************************************************************/
/* SEND SELECTED SKILL DATA TO SERVER                                  */
/************************************************************************/
void BattleScene::playSkill(UserSkillInfo skillData)
{
	if (_gameMode == MULTI_MODE)
	{
		BattleAPI::getInstance()->sendSkillEvent(skillData, _allAlliedUnitData[0]);
	}
	else {
		string uu = UserModel::getInstance()->getUuId().c_str();
		playSkillLogicAndAnimation(testObject, skillData, _currentPlayerTeamFlg, random(0.85f, 1.0f), uu.c_str(), 0, _allAlliedUnitData[0]);
	}

}

void BattleScene::playSkillLogicAndAnimation(Sprite* playObject, UserSkillInfo skill, int team_id, float randNum, string uuid, int saveIndex, UserUnitInfo unit)
{
	if (playObject == testObject) {
		for (auto& e : _allEnemyUnitSprite)
		{
			e->setColor(Color3B::WHITE);
		}
		_allAlliedUnitData[0].mp -= skill.mp_cost;
		//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
		_mainCharacterMpBar->setPercent(ceil(100.0f * _allAlliedUnitData[0].mp / _mainCharacerMaxMp));
	}

	
	switch (skill.effect_type)
	{
	case TYPE_BUFF:
		skillBuffAction(playObject, skill, team_id, saveIndex);
		break;
	case TYPE_RESTORE:

		skillRestoreAction(playObject, skill, saveIndex, team_id);
		break;
	case TYPE_ATTACK:
		skillAttackAction(playObject, skill, unit, team_id, randNum);
		break;
	case TYPE_POISON:
		skillPoisonAction(playObject, skill, team_id, uuid.c_str());
		break;
	case TYPE_STUN:
		skillStunAction(playObject, skill, team_id);
		break;
	default:
		break;
	}
}



void BattleScene::skillRestoreAction(Sprite* object, UserSkillInfo skillInfo, int targetUnitIndex, int teamId)
{
	switch (skillInfo.multi_effect)
	{
	case TARGET_ALL:
		skillRestoreAll(object,skillInfo, teamId);
		break;
	case TARGET_ONE:

		if (teamId == _currentPlayerTeamFlg) {
			skillRestoreOne(object, &_allAlliedUnitData[targetUnitIndex], skillInfo, teamId);
		}
		else 
		{
			skillRestoreOne(object, &_allEnemyUnitData[targetUnitIndex], skillInfo, teamId);
		}
		break;
	default:
		break;
	}


}

void BattleScene::skillBuffAction(Sprite* object, UserSkillInfo skillInfo, int teamId, int saveIndex)
{
	switch (skillInfo.multi_effect)
	{
	case TARGET_ALL :
		skillHelpAll(object,skillInfo, teamId);
		break;
	case TARGET_ONE:
		skillHelpOne(object,skillInfo, teamId, saveIndex);
		break;
	default:
		break;
	}
}

void BattleScene::skillAttackAction(Sprite* object, UserSkillInfo skillInfo,UserUnitInfo attacker,  int teamId, float randNum)
{
	switch (skillInfo.multi_effect)
	{
	case TARGET_ALL:
	{
		int flg = 0;
		vector<UserUnitInfo> effectUnitList;
		vector<Sprite*> effectUnitSpriteList;
		if (teamId == _currentEnemyTeamFlg) {
			skillAttackAll(object, skillInfo, attacker, ALLIED_FLAG, randNum,_allAlliedUnitSprite,&_allAlliedUnitData );
		}
		else {
			skillAttackAll(object, skillInfo, attacker, ENEMY_FLAG, randNum, _allEnemyUnitSprite, &_allEnemyUnitData);
		}
	}
		
		break;
	case TARGET_ONE:
		skillAttackOne(object,skillInfo, attacker, teamId, randNum);
	default:
		break;
	}
}

void BattleScene::skillRestoreAll(Sprite* object,UserSkillInfo skillInfo, int teamId)
{
	if (object == testObject) {
		int value = 0;
		switch (skillInfo.corrett_value)
		{
		case DAME_TYPE_PERCENT:
			value = ceil(1.0f*_saveMainStatusData.hp *skillInfo.corrett_value / 100.0f);
			break;
		case DAME_TYPE_PURE:
			value = ceil(skillInfo.corrett_value);
			break;
		}
		//TODO
		if (skillInfo.range_distance > 0)
		{

			vector<int> allUnitIndex = detectUnitInAoe(object, skillInfo, _allAlliedUnitSprite, false);

			for (int &index : allUnitIndex)
			{
				//TODO change to unique unitID

				_allAlliedUnitData[index].hp += value;
				if (_allAlliedUnitData[index].hp > _allAlliedUnitMaxHp[index]) {
					_allAlliedUnitData[index].hp = _allAlliedUnitMaxHp[index];
				}
				_allAlliedUnitHpBar[index]->setPercent(100.0f *_allAlliedUnitData[index].hp / _allEnemuUnitMaxHp[index]);

			}
		}
		else {

			log("Restore All");
			for (int i = 0; i < _allAlliedUnitData.size(); i++)
			{
				//TODO change to unique unit ID
				_allAlliedUnitData[i].hp += value;
				if (_allAlliedUnitData[i].hp > _allAlliedUnitMaxHp[i]) {
					_allAlliedUnitData[i].hp = _allEnemuUnitMaxHp[i];
				}
				//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
				_allAlliedUnitHpBar[i]->setPercent(100.0f * _allAlliedUnitData[i].hp / _allEnemuUnitMaxHp[i]);

			}
		}
	}
	else {
		if (teamId == _currentPlayerTeamFlg)
		{
			restoreAllActionLogic(object, skillInfo, _allAlliedUnitData, _allAlliedUnitSprite);
		}
		else
		{
			restoreAllActionLogic(object, skillInfo, _allEnemyUnitData, _allEnemyUnitSprite);
		}
	}


	createSorceryEffect(object, SORCERY_GREEN);
}

void BattleScene::restoreAllActionLogic(Sprite* object, UserSkillInfo skillInfo, vector<UserUnitInfo>& targetUnitDataList, vector<Sprite*> targetUnitList)
{
	if (skillInfo.range_distance > 0)
	{
		vector<int> a = detectUnitInAoe(object, skillInfo, targetUnitList, false);
		//TODO
	}
}

void BattleScene::skillRestoreOne(Sprite* object,UserUnitInfo* unitData,UserSkillInfo skillInfo, int teamId)
{
	//if (object == testObject) {
		int value = 0;
		int maxHp = UserUnitModel::getInstance()->getUnitInfoById(unitData->mst_unit_id).hp;
		switch (skillInfo.correct_type)
		{
		case DAME_TYPE_PERCENT:
			value = ceil(1.0f*maxHp *skillInfo.corrett_value / 100.0f);
			break;
		case DAME_TYPE_PURE:
			value = ceil(skillInfo.corrett_value);
			break;
		}
		unitData->hp += value;
		if (unitData->hp > maxHp) {
			unitData->hp = maxHp;
		}
		updateSlider();

		if (object == testObject) {
			updateHpAndMpViewLabel();
		}
	//}

	healEffectWithObject(object);
}

void BattleScene::healEffectWithObject(Sprite* obj) {
	/////////////////////////////////////////
	///////////RUN EFFECT HEAL ONE
	createSorceryEffect(obj, SORCERY_GREEN);

	Effect* effectHeal = Effect::createWithParticle(PARTICLE_HEAL, effectHeal->EC_Other, effectHeal->Restore);
	effectHeal->setTag(10);
	effectHeal->setScale(1 / IMAGE_SCALE);
	obj->addChild(effectHeal);
	effectHeal->setPosition(Vec2(obj->getContentSize().width / 2, obj->getContentSize().height / 2));

	auto healSequence = Sequence::create(
		DelayTime::create(DELAY_RESTORE)
		, RemoveSelf::create(true)
		, nullptr);
	effectHeal->runAction(healSequence);


}

void BattleScene::skillHelpAll(Sprite* object, UserSkillInfo skillInfo, int teamId)
{
	int value = 0;
}

void BattleScene::skillHelpOne(Sprite* object, UserSkillInfo skillInfo, int teamId, int saveIndex)
{
	//TODO
	float value = 1.0f;
	int pureValue = 0;
	/*Only run logic if object is current player unit*/
	if (object == testObject) {
		switch (skillInfo.correct_type)
		{
		case DAME_TYPE_PERCENT:
			value = ceil(skillInfo.corrett_value / 100.0f);
			break;
		case DAME_TYPE_PURE:
			pureValue = skillInfo.corrett_value;
			break;
		default:
			break;
		}
	}
	int saveValue = 0;

	Effect* effect = new Effect();

	switch (skillInfo.buff_effect_type)
	{
	case SKILL_HELP_TYPE::HP:
	{
		log("help HP");
		if (object == testObject) {
			saveValue = 1.0f*_saveMainStatusData.hp*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].hp += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].hp -= saveValue;
			}), nullptr));
		}
		/////////////////////////////////////////
		///////////RUN EFFECT HP RESTORE
		createSorceryEffect(object, SORCERY_GREEN);

		Effect* hp = Effect::createWithParticle(PARTICLE_HEAL, effect->EC_Other, effect->Restore);
		object->addChild(hp);
		hp->setPosition(Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2));
		hp->setScale(1 / IMAGE_SCALE);
		auto hpSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, RemoveSelf::create(true)
			, nullptr);
		hp->runAction(hpSequence);

		break;
	}
	case SKILL_HELP_TYPE::HP_RESTORE:
	{
		if (object == testObject) {
			saveValue = 1.0f*_saveMainStatusData.hp_heal*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].hp_heal += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].hp_heal -= saveValue;
			}), nullptr));
		}
		/////////////////////////////////////////
		///////////RUN EFFECT HP RESTORE
		createSorceryEffect(object, SORCERY_GREEN);

		Effect* hpRestore = Effect::createWithParticle(PARTICLE_HEAL, effect->EC_Other, effect->Restore);
		object->addChild(hpRestore);
		hpRestore->setScale(1 / IMAGE_SCALE);
		hpRestore->setPosition(Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2));

		auto hpRestoreSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, RemoveSelf::create(true)
			, nullptr);
		hpRestore->runAction(hpRestoreSequence);

		break;
	}
	case SKILL_HELP_TYPE::MP:
		if (object == testObject) {
			saveValue = 1.0f*_saveMainStatusData.mp*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].mp += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].mp -= saveValue;
			}), nullptr));
		}
		break;
	case SKILL_HELP_TYPE::MP_RESTORE:
		if (object == testObject)
		{
			saveValue = 1.0f*_saveMainStatusData.mp_heal*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].mp_heal += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].mp_heal -= saveValue;
			}), nullptr));
		}
		break;
	case SKILL_HELP_TYPE::ATTACK_DAME:
	{
		if (object == testObject)
		{
			saveValue = 1.0f*_saveMainStatusData.attack*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].attack += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			log("increase attack by %d", saveValue);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].attack -= saveValue;
				log("remove attack buff %d", saveValue);
			}), nullptr));
		}
		if (teamId == _currentPlayerTeamFlg) {
			displayUnitStatus(object, BUFF_ATTACK, skillInfo, saveIndex, &_alliedStatusImagePath);
		}
		else {
			displayUnitStatus(object, BUFF_ATTACK, skillInfo, saveIndex, &_enemyStatusImagePath);
		}
		break;
	}
	case SKILL_HELP_TYPE::DEFENCE:
	{
		if (object == testObject) {
			saveValue = 1.0f*_saveMainStatusData.defence*(value - 1.0f) + pureValue;
			log("increase defence by %d", saveValue);
			_allAlliedUnitData[0].defence += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				if (saveValue > 0) {
					_allAlliedUnitData[0].defence -= saveValue;
				}

				log("remove defence buff %d", saveValue);
			}), nullptr));
		}
		//displayUnitStatus(object, BUFF_DEFENCE, skillInfo);

		if (teamId == _currentPlayerTeamFlg) {
			displayUnitStatus(object, BUFF_DEFENCE, skillInfo, saveIndex, &_alliedStatusImagePath);
		}
		else {
			displayUnitStatus(object, BUFF_DEFENCE, skillInfo, saveIndex, &_enemyStatusImagePath);
		}
		/////////////////////////////////////////
		///////////RUN EFFECT HELP DEFENCE
		createSorceryEffect(object, SORCERY_BLUE);

		Effect* effectHelpDefence = Effect::createWithParticle(PARTICLE_DEFENCE_H, effect->EC_Other, effect->Help_Defence);
		object->addChild(effectHelpDefence);
		effectHelpDefence->setScale(1 / IMAGE_SCALE);
		effectHelpDefence->setPosition(Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2));

		auto helpDefenceSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, RemoveSelf::create(true)
			, nullptr);
		effectHelpDefence->runAction(helpDefenceSequence);


		break;
	}
	case SKILL_HELP_TYPE::ATTACK_RANGE:
	{
		if (object = testObject)
		{
			saveValue = 1.0f*_saveMainStatusData.attack_range*(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].attack_range += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].attack_range -= saveValue;
			}), nullptr));
		}
		break;
	}
	case SKILL_HELP_TYPE::MOVESPEED:
	{
		if (object = testObject)
		{
			saveValue = 1.0f* _saveMainStatusData.move_speed *(value - 1.0f) + pureValue;
			_allAlliedUnitData[0].move_speed += saveValue;
			//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
			runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
				_allAlliedUnitData[0].move_speed -= saveValue;
			}), nullptr));
		}
		/////////////////////////////////////////
		///////////RUN EFFECT HELP MOVE SPEED
		createSorceryEffect(object, SORCERY_BLUE);

		Effect* effectHelpMoveSpeed = Effect::createWithParticle(PARTICLE_MOVESPEED, effect->EC_Other, effect->Help);
		object->addChild(effectHelpMoveSpeed);
		effectHelpMoveSpeed->setScale(1/IMAGE_SCALE);
		effectHelpMoveSpeed->setPosition(Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2));

		auto helpmMoveSpeedSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, RemoveSelf::create(true)
			, nullptr);
		effectHelpMoveSpeed->runAction(helpmMoveSpeedSequence);

		break;
	}
	default:
		break;
	}

}

void BattleScene::skillAttackAll(Sprite* object, UserSkillInfo skillInfo, UserUnitInfo attacker, int flg, float randNum, vector<Sprite*> &effectUnitSpriteList, vector<UserUnitInfo>* effectUnitList)
{
	int value = 0;
	switch (skillInfo.correct_type)
	{
	case DAME_TYPE_PERCENT:
		value = attacker.attack*100.0f / skillInfo.corrett_value;
		break;
	case DAME_TYPE_PURE:
		value = skillInfo.corrett_value;
	default:
		break;
	}
	
	vector<int> unitIndex;
	if (skillInfo.range_distance > 0) {
		
		unitIndex = detectUnitInAoe(object, skillInfo, effectUnitSpriteList, false);
	}
	else 
	{
		for (int i = 0; i < effectUnitList->size(); i++)

		{
			unitIndex.push_back(i);
		}
	}
	auto aas = &effectUnitList;
	for (int &index : unitIndex)
	{
		int dame = 0;
		dame = (value - effectUnitList->at(index).defence);
		float defaultDameRate = caculDameRate(attacker.element, effectUnitList->at(index).element);
		dame = ceil(randNum*dame*defaultDameRate);

		if (dame <= 0) dame = 1;
		
		if (effectUnitSpriteList[index]->isVisible())
		{
			auto action = /*Spawn::create(*/Sequence::create(DelayTime::create(2), CallFuncN::create(CC_CALLBACK_1(BattleScene::showDameAndSkillLogic, this,index, dame, object, effectUnitSpriteList[index], effectUnitList)), nullptr);/* , CallFuncN::create([&, this, index](Ref *p) {*/
			/*Not need Spawn with CallFuncC. We can use code directly*/
			effectUnitSpriteList[index]->setTag(index);
			effectUnitSpriteList[index]->runAction(action);

			///////////////////////////////////////////////////
			//RUN EFFECT ATTACK ONE UNIT
			///////////////////////////////////////////////////
			// Create attack effect
			Effect* attackFire = Effect::createWithParticle(PARTICLE_FIRE, attackFire->EC_Other, attackFire->Help);
			attackFire->setTargetAttack(effectUnitSpriteList[index]);
			object->getParent()->addChild(attackFire);
			attackFire->setPosition(object->getPosition());

			auto attackFireSequence = Sequence::create(
				DelayTime::create(DELAY_HELP)
				, RemoveSelf::create(true)
				, nullptr);
			attackFire->runAction(attackFireSequence);

			/*}), nullptr);*/

			
		}
	}
	createSorceryEffect(object, SORCERY_RED);
}
void BattleScene::showDameAndSkillLogic(Ref *p, int index, int dame, Sprite* object, Sprite* target, vector<UserUnitInfo>* effectUnitlist) {
	showAttackDame(dame, target->getPosition() + Vec2(0, 100), 1);
	effectUnitlist->at(index).hp -= dame;
	if (object == testObject) {
		saveDameInfo(dame, 0, index, _currentPlayerTeamFlg);
		sendDameDeal(dame, index, [&](SIOClient *c, const string data) {
			log("send dame event in play skill completed");
		});
		if (effectUnitlist->at(index).hp <= 0) {
			enemyDieAction(index);
		}
	}
	updateSlider();
}

void BattleScene::skillAttackOne(Sprite* object, UserSkillInfo skillInfo,UserUnitInfo attacker, int teamId ,float randNum)
{
	log("Attack One");
	/*Need more param for skill attack one in RTS mode: target id*/
	if (object == testObject) {
		int value = 0;
		switch (skillInfo.correct_type)
		{
		case DAME_TYPE_PERCENT:
			value = _allAlliedUnitData[0].attack*100.0f / skillInfo.corrett_value;
			break;
		case DAME_TYPE_PURE:
			value = skillInfo.corrett_value;
		default:
			break;
		}
		/*Need more parameter*/
		if (_indexOfBeAttackEnemy < 0) return;

		int dame = (value - _allEnemyUnitData[_indexOfBeAttackEnemy].defence);
		float defaultDameRate = caculDameRate(_allAlliedUnitData[0].element, _allEnemyUnitData[_indexOfBeAttackEnemy].element);

		dame = ceil(randNum*dame*defaultDameRate);
		if (dame <= 0) dame = 1;

		int indexValue = _indexOfBeAttackEnemy;


		auto action = /*Spawn::create(*/
			Sequence::create(
			//////Start Sequence
			DelayTime::create(5),  //DELAY_ATTACK_MOVE + DELAY_ATTACK_FIRE_LIFE
			CallFuncN::create([&, this, dame, indexValue](Ref *p) {
			showAttackDame(dame, _allEnemyUnitSprite[indexValue]->getPosition() + Vec2(0, 100), 1);
			_allEnemyHpBar[indexValue]->setPercent(100.0f *_allEnemyUnitData[indexValue].hp / _allEnemuUnitMaxHp[indexValue]);
			_allEnemyUnitData[indexValue].hp -= dame;
			saveDameInfo(dame, 0, indexValue, _currentPlayerTeamFlg);
			if (_allEnemyUnitData[indexValue].hp <= 0)
			{
				enemyDieAction(indexValue);
				return;
			}

		}), nullptr);/* ,

			////////End Sequence
			CallFuncN::create([&, this, indexValue](Ref *p) {*/
		///////////////////////////////////////////////////
		//RUN EFFECT ATTACK ONE UNIT
		///////////////////////////////////////////////////
		_allEnemyUnitSprite[_indexOfBeAttackEnemy]->runAction(action);
		createSorceryEffect(testObject, SORCERY_ORANGE);
		// Create attack effect
		Effect* attackFire = Effect::createWithParticle(PARTICLE_FIRE, attackFire->EC_Other, attackFire->Help);
		attackFire->setTargetAttack(_allEnemyUnitSprite[indexValue]);
		testObject->getParent()->addChild(attackFire);
		attackFire->setPosition(testObject->getPosition());

		// Effect life in 1s , remove Effect
		auto attackFireSequence = Sequence::create(
			DelayTime::create(DELAY_ATTACK_FIRE_LIFE)
			, RemoveSelf::create(true)
			, nullptr);
		attackFire->runAction(attackFireSequence);

		/*}), nullptr); */

	}

}


vector<int> BattleScene::detectUnitInAoe(Sprite* mainObj, UserSkillInfo skill, vector<Sprite*> targetList, bool drawFlg /*= true*/)

{
	vector<int> resultUnitId;
	auto pos = mainObj->getPosition();
	vector<Vec2> vec;
	DrawNode *draw = DrawNode::create();
	switch (skill.range_type)
	{

	case SKILL_RANGE_TYPE::RECTANGLE:
		draw->drawRect(Vec2(-skill.range_distance/2,-skill.range_distance/2), Vec2(skill.range_distance/2, skill.range_distance/2), Color4F::RED);
		draw->setPosition(pos/* - Vec2(skill.aoe / 2, skill.aoe / 2)*/);
		draw->setTag(DRAW_UNIT);
		break;
	case SKILL_RANGE_TYPE::TRIANGLE:
		vec.push_back(Vec2(-skill.range_distance / 2, -skill.range_distance * 1 / 3));
		vec.push_back(Vec2(skill.range_distance / 2, -skill.range_distance / 3));
		vec.push_back(Vec2(0, skill.range_distance * 2 / 3));
		draw->drawPoly(&vec[0], 3, true, Color4F::RED);
		draw->setPosition(pos);
		draw->setTag(DRAW_UNIT);
		break;
	case SKILL_RANGE_TYPE::STAR:
		//draw->drawPolygon()
		vec.push_back(Vec2(-skill.range_distance / 2, 0));
		vec.push_back(Vec2(-skill.range_distance/6, -skill.range_distance/6));
		vec.push_back(Vec2(0, -skill.range_distance / 2));
		vec.push_back(Vec2(skill.range_distance / 6, -skill.range_distance / 6));
		vec.push_back(Vec2(skill.range_distance / 2, 0));
		vec.push_back(Vec2(skill.range_distance / 6, skill.range_distance / 6));
		vec.push_back(Vec2(0, skill.range_distance / 2));
		vec.push_back(Vec2(-skill.range_distance / 6, skill.range_distance / 6));
		draw->drawPoly(&vec[0], vec.size(),true, Color4F::MAGENTA);
		draw->setPosition(pos);
		draw->setTag(DRAW_UNIT);
		break;
	default:
		draw->drawCircle(Vec2::ZERO, skill.range_distance, 360.0f, 50, false, Color4F(200,0,0,50));
		draw->setPosition(pos);
		draw->setTag(DRAW_UNIT);
		break;
	}
// 	bool drawFlg = true;
	if (skill.multi_effect != 1)
	{
		drawFlg = false;
	}
	if (drawFlg) {
		_battleBackround->addChild(draw);
	}

	for (int i = 0; i < targetList.size(); i++)
	{
		//rectangle
		Rect a;
		Vec2 distan = targetList[i]->getPosition() - pos;
		switch (skill.range_type)
		{
		case SKILL_RANGE_TYPE::RECTANGLE:
			a = Rect(pos.x - skill.range_distance / 2, pos.y - skill.range_distance / 2, skill.range_distance, skill.range_distance);
			if (a.containsPoint(targetList[i]->getPosition())) {
				resultUnitId.push_back(i);
			}
			break;
		case SKILL_RANGE_TYPE::TRIANGLE:
			if (detectPointInTriangle(targetList[i]->getPosition(), vec,mainObj)) {
				resultUnitId.push_back(i);
			}
			break;
		case SKILL_RANGE_TYPE::STAR:
			if (detectPointInTriangle(targetList[i]->getPosition(), {vec[0],vec[1],vec[7]},mainObj)) {
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(targetList[i]->getPosition(), {vec[1],vec[2],vec[3]}, mainObj))
			{
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(targetList[i]->getPosition(), { vec[3], vec[4], vec[5] },mainObj)) {
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(targetList[i]->getPosition(), { vec[7], vec[5], vec[6] }, mainObj)) {
				resultUnitId.push_back(i);
				break;
			}
			if (Rect(pos.x - skill.range_distance / 6, pos.y - skill.range_distance / 6, skill.range_distance / 3, skill.range_distance / 3).containsPoint(targetList[i]->getPosition())) {
				resultUnitId.push_back(i);
				break;
			}

			break;
		default:
			//round
			
			// 		distan.y = distan.y *SKILL_AOE_Y_SCALE;
			if (distan.length() < skill.range_distance) {
				resultUnitId.push_back(i);
			}
			break;
		}
		
	}
	return resultUnitId;
}

void BattleScene::updateHpAndMpViewLabel()
{
	string hp = DataUtils::numberToString(_allAlliedUnitData[0].hp).append("/").append(DataUtils::numberToString(_allAlliedUnitMaxHp[0]));
	_hpViewLabel->setString(hp.c_str());
	string mp = DataUtils::numberToString(_allAlliedUnitData[0].mp).append("/").append(DataUtils::numberToString(_mainCharacerMaxMp));
	_mpViewlabel->setString(mp.c_str());
}

void BattleScene::endBattle(int winTeamId)
{
	if (_gameMode == MULTI_MODE)
	{
		BattleAPI::getInstance()->sendBattleEndEvent(winTeamId);
	}
	
}

float BattleScene::caculDameRate(int mainC, int enemy)
{
	if (enemy == 4 || mainC == enemy)
	{
		return 1.0f;
	}
	if (mainC - enemy == -1 || mainC - enemy == 2) {
		return 1.5f;
	}
	return 0.5f;
}

void BattleScene::longPressAction(Button *pSender,UserSkillInfo skill)
{
	auto a = detectUnitInAoe(testObject,skill, _allEnemyUnitSprite);
	_onSelectSkillData = skill;
	for (auto& enemy : a)
	{
		_allEnemyUnitSprite[enemy]->setColor(Color3B::RED);
	}
}


bool BattleScene::detectPointInTriangle(Vec2 point, vector<Vec2> points, Sprite* object)
{
	Vec2 v0 = makePoint(points[1]+object->getPosition(), points[0]+object->getPosition());
	Vec2 v1 = makePoint(points[2]+object->getPosition(), points[0]+object->getPosition());
	Vec2 v2 = makePoint(point, points[0]+object->getPosition());

	float dot00 = makeDot(v0, v0);
	float dot01 = makeDot(v0, v1);
	float dot02 = makeDot(v0, v2);
	float dot11 = makeDot(v1, v1);
	float dot12 = makeDot(v1, v2);

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u > 0) && (v > 0) && (u + v < 1);

}

float BattleScene::makeDot(Vec2 v1, Vec2 v2)
{
	return v1.x*v2.x + v1.y * v2.y;
}

Vec2 BattleScene::makePoint(Vec2 v1, Vec2 v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

BattleScene::BattleScene() :
	_oldSecond(0),
	_indexOfBeAttackEnemy(-1),
	_allAlliedUnitData({}),
	_allAlliedUnitSprite({}),
	_allAlliedUnitHpBar({})
{

}

BattleScene::~BattleScene()
{
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(BattleScene::update), this);
	_allAlliedUnitData = {};
	_allAlliedUnitSprite = {};
	_allAlliedUnitHpBar = {};

	_allEnemyUnitData = {};
	_allEnemyUnitSprite = {};
	_allAlliedUnitData = {};
	_onDestructCalled = true;
}

void BattleScene::saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg)
{
	/*if (teamFlg == _currentPlayerTeamFlg) {
		if (attackUnitId < _allAlliedUnitData.size() - 1) 
		{
			_saveBattleInfoAlliedTeam[attackUnitId].totalDealDame += dame;
		}
		if (beAttackUnitId < _allEnemyUnitData.size() - 1)
		{

			_saveBattleInfoEnemyTeam[beAttackUnitId].totalReceivedDame += dame;
		}
		return;
	}
	else 
	{
		if (attackUnitId < _allEnemyUnitData.size() - 1)
		{
			_saveBattleInfoEnemyTeam[attackUnitId].totalDealDame += dame;
		}

		if (beAttackUnitId < _allAlliedUnitData.size() - 1)
		{
			_saveBattleInfoAlliedTeam[beAttackUnitId].totalReceivedDame += dame;
		}
		
	}*/
}

void BattleScene::saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg)
{
	/*if (teamFlg == _currentPlayerTeamFlg) {
		if (killerId < _allAlliedUnitData.size() - 1)
		{
			_saveBattleInfoAlliedTeam[killerId].killNum++;
		}
		if (deadUnitId < _allEnemyUnitData.size() - 1) {
			_saveBattleInfoEnemyTeam[deadUnitId].deadNum++;
		}

	}
	else 
	{
		if (killerId < _allEnemyUnitData.size() - 1)
		{
			_saveBattleInfoEnemyTeam[killerId].killNum++;
		}
		if (deadUnitId < _allAlliedUnitData.size() - 1)
		{
			_saveBattleInfoAlliedTeam[deadUnitId].deadNum++;
		}
	}*/
}

void BattleScene::sendDameDeal(int dame, int targetId, SocketIOCallback callback)
{
	if (_gameMode == MULTI_MODE)
	{
		BattleAPI::getInstance()->sendDameDealEvent(dame, _allEnemyUnitData[targetId].uuid.c_str(), callback);
	}
}

void BattleScene::sendKillDead(string killerUuid, string deadUnitUuid, Vec2 deadTitleCoor, SocketIOCallback callback)
{
	if (_gameMode == MULTI_MODE) {
		BattleAPI::getInstance()->sendKillEvent(killerUuid.c_str(), deadUnitUuid.c_str(), deadTitleCoor, callback);
	}
}


/*status type: POISON, STUN, BUFF_ATTACK, BUFF_DEFENCE, DEBUFF_ATTACK*/
/*Need to modify for RTS process*/
//TODO
void BattleScene::displayUnitStatus(Sprite *object, int statusType, UserSkillInfo skillInfo, int spIndex /*= 0*/, vector<vector<string>>* targetImageStatus)
{
	string imagePath;
	switch (statusType)
	{
	case POISON:
		imagePath = "balloon_badstatus_poison_";
		break;
	case STUN:
		imagePath = "balloon_badstatus_stun_";
		break;
	case BUFF_ATTACK:
		imagePath = "balloon_buff_attack_";
		break;
	case BUFF_DEFENCE:
		imagePath = "balloon_buff_defence_";
		break;
	case DEBUFF_ATTACK:
		imagePath = "baloon_debuff_attack_";
		break;
	default:
		break;
	}

	pushStatusImagePath(imagePath, targetImageStatus->at(spIndex));

	object->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create(CC_CALLBACK_1(BattleScene::removeStatus, this, imagePath, targetImageStatus,spIndex)), nullptr));
	displayStatusInTime(object, targetImageStatus->at(spIndex));

	/*if (object != testObject)
	{
		pushStatusImagePath(imagePath, _enemyStatusImagePath[spIndex]);
		object->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, imagePath,spIndex](Ref* p){
			removeStatusImagePath(imagePath, _enemyStatusImagePath[spIndex]);
			displayStatusInTime((Sprite*)p, _enemyStatusImagePath[spIndex]);
		}), nullptr));
		displayStatusInTime(object, _enemyStatusImagePath[spIndex]);
	}
	else
	{
		pushStatusImagePath(imagePath, _skillStatusImageList);
		object->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, imagePath](Ref* p){
			removeStatusImagePath(imagePath, _skillStatusImageList);
			displayStatusInTime((Sprite*)p, _skillStatusImageList);
		}), nullptr));
		displayStatusInTime(object, _skillStatusImageList);
	}*/


}

void BattleScene::removeStatus(Ref *p, string imagepath, vector<vector<string>>* targetStatus , int index) 
{
	removeStatusImagePath(imagepath, targetStatus->at(index));
	displayStatusInTime((Sprite*)p, targetStatus->at(index));
}

Animation* BattleScene::createStatusAnimation(string imagePath)
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "%s%d.png", imagePath.c_str(), i);
		string p = "image/status/";
		p.append(szName);
		//log("%s", p.c_str());
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(STATUS_DELAY_TIME);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	animation->autorelease();
	return animation;
}

void BattleScene::skillPoisonAction(Sprite* object, UserSkillInfo skillInfo, int teamId, const string casterUuid)
{
	vector<Sprite*> effectedUnitSprite;
	if (teamId == _currentPlayerTeamFlg) {
		poisonEffectAction(object,skillInfo, &_allEnemyUnitData, _allEnemyUnitSprite, teamId, casterUuid);
	}
	else {
		poisonEffectAction(object, skillInfo, &_allAlliedUnitData, _allAlliedUnitSprite, teamId, casterUuid);
	}
	/*vector<int> units = detectUnitInAoe(object,skillInfo, effectedUnitSprite, false);
	for (auto & index :units)
	{
		
		if (teamId == _currentPlayerTeamFlg)
		{
			displayUnitStatus(effectedUnitSprite[index], POISON, skillInfo, index, &_enemyStatusImagePath);
			poisonEffectAction(skillInfo, index, &_allEnemyUnitData, effectedUnitSprite[index]);
		}
		else
		{
			displayUnitStatus(effectedUnitSprite[index], POISON, skillInfo, index, &_alliedStatusImagePath);
			poisonEffectAction(skillInfo, index, &_allAlliedUnitData, effectedUnitSprite[index]);
		}
	}*/
}

void BattleScene::poisonEffectAction(Sprite* object, UserSkillInfo skill, vector<UserUnitInfo>* unitList, vector<Sprite*> targetSprite, int teamId, const string casterUuid)
{
	vector<int> unitIds = detectUnitInAoe(object, skill, targetSprite, false);

	for (auto &index : unitIds)
	{
		int dame = ceil(1.0f*UserUnitModel::getInstance()->getUnitInfoById(unitList->at(index).mst_unit_id).hp * 0.05f);


		auto psAction = Sequence::create(CallFuncN::create([&,object, index, dame, targetSprite, unitList, casterUuid](Ref *p) {
			showAttackDame(dame, targetSprite[index]->getPosition() + Vec2(0, 100), 1);
			if (object == testObject) {
				sendDameDeal(dame, index, nullptr);
			}
			unitList->at(index).hp -= dame;
			updateSlider();
			if (unitList->at(index).hp <= 0) {
				auto unit = BattleModel::getInstance()->getUnitInforByUuid(casterUuid);
				if (targetSprite[index] == testObject) {
					runRespawnAction(casterUuid.c_str());
				}
				else
				{
					unitDieAction(targetSprite[index], unitList, index);
				}
				
			}
		}), DelayTime::create(POISON_STEP_TIME), nullptr);
		targetSprite[index]->runAction(Repeat::create(psAction, ceil(skill.duration / POISON_STEP_TIME)));
		if (teamId == _currentPlayerTeamFlg) {
			displayUnitStatus(targetSprite[index], POISON, skill, index, &_enemyStatusImagePath);
		}
		else
		{
			displayUnitStatus(targetSprite[index], POISON, skill, index, &_alliedStatusImagePath);
		}
	
	}
}

void BattleScene::skillStunAction(Sprite* object, UserSkillInfo skillInfo, int teamId)
{
	int teamFlg = 0;
	vector<Sprite*> effectedUnitSprite;
	if (teamId == _currentPlayerTeamFlg) {
		effectedUnitSprite = _allEnemyUnitSprite;
	}
	else {
		effectedUnitSprite = _allAlliedUnitSprite;
	}
	vector<int> units = detectUnitInAoe(object,skillInfo,effectedUnitSprite, false);

	for (auto& i : units)
	{
		if (teamId == _currentPlayerTeamFlg) {
			displayUnitStatus(effectedUnitSprite[i], STUN, skillInfo, i, &_enemyStatusImagePath);
			stunEffecAction(effectedUnitSprite[i], skillInfo, i, &_allEnemyUnitData);
		}
		else
		{
			displayUnitStatus(effectedUnitSprite[i], STUN, skillInfo, i, &_alliedStatusImagePath);
			stunEffecAction(effectedUnitSprite[i], skillInfo, i, &_allAlliedUnitData);
		}
	}
}

void BattleScene::stunEffecAction(Sprite* object, UserSkillInfo skill, int index, vector<UserUnitInfo>* effectUnitDataList) {
	effectUnitDataList->at(index).isStun = true;
	object->getPhysicsBody()->setVelocity(Vec2::ZERO);
	object->runAction(Sequence::create(DelayTime::create(skill.duration), CallFuncN::create([&, index, effectUnitDataList](Ref*p){
		effectUnitDataList->at(index).isStun = false;
	}), nullptr));
}

void BattleScene::fountainRestoreEffect(Sprite *object, vector<UserUnitInfo>* unitList, int index)
{
	if (object->getActionByTag(FOUNTAIN_ACTION) != nullptr)
	{
		return;
	}
	auto action = RepeatForever::create(Sequence::create(CallFuncN::create([&, unitList, index](Ref *p){
		/*_allAlliedUnitData[0].mp += ceil(_saveMainStatusData.mp *0.05f);
		if (_allAlliedUnitData[0].mp > _mainCharacerMaxMp)
		{
			_allAlliedUnitData[0].mp = _mainCharacerMaxMp;
		}
		_mainCharacterMpBar->setPercent(ceil(100.0f*_allAlliedUnitData[0].mp / _saveMainStatusData.mp));
		_allAlliedUnitData[0].hp += ceil(_saveMainStatusData.hp * 0.05f);
		if (_allAlliedUnitData[0].hp > _allAlliedUnitMaxHp[0]) {
			_allAlliedUnitData[0].hp = _allAlliedUnitMaxHp[0];
		}
		//BattleAPI::getInstance()->battleSyncEvent(_allAlliedUnitData[0]);
		_allAlliedUnitHpBar[0]->setPercent(ceil(100.0f*_allAlliedUnitData[0].hp / _saveMainStatusData.hp));
		_mainCharacterHpBar->setPercent(ceil(100.0f*_allAlliedUnitData[0].hp / _saveMainStatusData.hp));
		*/
		auto unit = UserUnitModel::getInstance()->getUnitInfoById(unitList->at(index).mst_unit_id);
		unitList->at(index).mp += ceil(unit.mp*0.05f);
		if (unitList->at(index).mp > unit.mp)
		{
			unitList->at(index).mp = unit.mp;
		}
		unitList->at(index).hp += ceil(unit.hp * 0.05f);
		if (unitList->at(index).hp > unit.hp)
		{
			unitList->at(index).hp = unit.hp;
		}

		updateSlider();
	}), DelayTime::create(1), nullptr));
	action->setTag(FOUNTAIN_ACTION);
	object->runAction(action);
}

void BattleScene::enemyRespawAction(int index)
{
	auto action = (Sequence::create(DelayTime::create(5), CallFuncN::create([&, index](Ref *pSEnder){
		if (_currentPlayerTeamFlg == TEAM_FLG_BLUE) {
			_allEnemyUnitSprite[index]->setPosition(Vec2(_myMap->getContentSize().width - 70 , _myMap->getContentSize().height /2 +(index - 1)*50));
		}
		else
		{
			_allEnemyUnitSprite[index]->setPosition(Vec2(70, _myMap->getContentSize().height / 2 + (index - 1) * 50));
		}
		_allEnemyUnitSprite[index]->setVisible(true);
		_allEnemyIconInMinimap[index]->setVisible(true);
		_allEnemyUnitData[index].hp = _allEnemuUnitMaxHp[index];
		updateSlider();
	}), nullptr));
	action->setTag(ENEMY_RESPAW_ACTION_TAG);
	_allEnemyUnitSprite[index]->runAction(action);
}

//////////////////////////////////////////////////////////////////////////////////////
// SORCERY EFFECT AND REMOVE EFFECT OBJECT AFTER RUN
// Created by Vien
//////////////////////////////////////////////////////////////////////////////////////
void BattleScene::createSorceryEffect(Sprite* spriteUnit, std::string eclipseFilePath)
{
	auto nodeFather = Sprite::create();

	spriteUnit->addChild(nodeFather, -10);
	nodeFather->setPosition(Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2 - spriteUnit->getContentSize().height / 4
		));

	// Create Eclipse
	auto sorcery = Sprite::create(eclipseFilePath);
	sorcery->setScale(1.5f *(1/IMAGE_SCALE));
	sorcery->setPosition(Vec2(0, 0));

	nodeFather->addChild(sorcery, -10);

	auto actionRotate = RotateBy::create(1.0f, Vec3(0, 0, 150));
	auto repeat = RepeatForever::create(actionRotate);
	sorcery->runAction(repeat);

	nodeFather->setScaleY(0.5f);


	//removeSorceryEclipse
	auto sequence = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE),
		CallFuncN::create(CC_CALLBACK_1(BattleScene::removeSorceryEclipse, this)),
		NULL
		);

	nodeFather->runAction(sequence);
}

void BattleScene::removeSorceryEclipse(Ref* pSender)
{
	Sprite* sorcery = dynamic_cast<Sprite*>(pSender);

	if (sorcery != nullptr)
	{
		sorcery->removeFromParentAndCleanup(true);
		//log("Remove eclipse");
	}
	else
	{
		//log("Remove eclipse");
		return;
	}
}


void BattleScene::pushStatusImagePath(string imagepath, vector<string> &allImages)
{
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "%s%d.png", imagepath.c_str(), i);
		string p = "image/status/";
		p.append(szName);
		//log("%s", p.c_str());
		allImages.push_back(p.c_str());
	}

}

void BattleScene::removeStatusImagePath(string imagepath, vector<string> &allImages)
{
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "%s%d.png", imagepath.c_str(), i);
		string p = "image/status/";
		p.append(szName);
		//log("%s", p.c_str());
		int index = findIndexOfString(allImages, p.c_str());
		allImages.erase(allImages.begin()+index); //TODO
	}

}

void BattleScene::displayStatusInTime(Sprite* parent, vector<string> allImages)
{
	
	if (parent->getChildByTag(BUFF_STATUS_TAG)) {
		parent->removeChildByTag(BUFF_STATUS_TAG);
	}
	if (allImages.size() <= 0) return;
	auto animation = Animation::create();
	for (auto &str : allImages)
	{
		animation->addSpriteFrameWithFile(str.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(STATUS_DELAY_TIME);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);

	auto animate = Animate::create(animation);
	auto sprite = Sprite::create("test.png");
	sprite->setTag(BUFF_STATUS_TAG);
	sprite->setPosition(Vec2(parent->getContentSize().width / 2, 30));
	sprite->setScale(1 / IMAGE_SCALE);
	sprite->runAction(RepeatForever::create(animate));
	parent->addChild(sprite, -1);
}

int BattleScene::findIndexOfString(vector<string> v, string element)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == element) {
			return i;
		}
	}
	return 0;
}

cocos2d::Vec2 BattleScene::getTitleCoorForPosition(Vec2 location)
{
	int x = location.x / _myMap->getTileSize().width;
	int y = ((_myMap->getMapSize().height*_myMap->getTileSize().height - location.y) / _myMap->getTileSize().height);
	return Vec2(x, y);
}
cocos2d::Vec2 BattleScene::getPositionForTitleCoord(Vec2 tileCoord)
{
	/*int x = (tileCoord.x * _myMap->getTileSize().width) + _myMap->getTileSize().width / 2;
	int y = (_myMap->getMapSize().height * _myMap->getTileSize().height) -
		(tileCoord.y * _myMap->getTileSize().height) - _myMap->getTileSize().height / 2;
	return Vec2(x, y);*/
	return _mapLayer->getTileAt(tileCoord)->getPosition();
}

vector<Vec2> BattleScene::AStarPathFindingAlgorithm(Vec2 curentPos, Vec2 destinationPos)
{
	vector<Vec2> result = {};
	
	Vec2 fromTitleCoord = getTitleCoorForPosition(curentPos);
	Vec2 desTitleCoord = getTitleCoorForPosition(destinationPos);
	if (desTitleCoord.x > _myMap->getMapSize().width) {
		desTitleCoord.x = _myMap->getMapSize().width;
	}
	if (desTitleCoord.y > _myMap->getMapSize().height) {
		desTitleCoord.y = _myMap->getMapSize().height;
	}
	if (desTitleCoord.x < 0) {
		desTitleCoord.x = 0;
	}
	if (desTitleCoord.y < 0) {
		desTitleCoord.y = 0;
	}
	if (fromTitleCoord == destinationPos)
	{
		log("You are already in the destination");
		return result;
	}
	//Rock, tree check
	log("From: %f, %f:", fromTitleCoord.x, fromTitleCoord.y);
	log("To: %f, %f", desTitleCoord.x, desTitleCoord.y);
	if (!isValidTileCoord(desTitleCoord)|| isWallAtTileCoord(desTitleCoord))
	{
		log("you cannot move there");
		return result;
	}
	_spClosedSteps.clear();
	_spOpenSteps.clear();
	_shortestPath.clear();
	schedule(schedule_selector(BattleScene::countTime));
	insertInOpenSteps(ShortestPathStep::createWithPosition(fromTitleCoord));

	do 
	{
		ShortestPathStep *curStep = _spOpenSteps.at(0);

		_spClosedSteps.pushBack(curStep);

		_spOpenSteps.erase(0);

		if (curStep->getPosition() == desTitleCoord) {
			unschedule(schedule_selector(BattleScene::countTime));
			log("Cost Time: %f", calCulTime);
			calCulTime = 0.0f;
			log("Here the path");
			//
			constructPathAndStartAnimationFromStep(curStep);
			_spClosedSteps.clear();
			_spOpenSteps.clear();
			break;
		}

		PointArray *checkStep = allWalkableTitlesCoordForTitleCoord(curStep->getPosition());

		for (size_t i = 0; i < checkStep->count(); i++)
		{
			ShortestPathStep *step = ShortestPathStep::createWithPosition(checkStep->getControlPointAtIndex(i));

			if (getStepIndex(_spClosedSteps, step) != -1) {
				continue;
			}
			int moveCost = costToMoveFromStepToAdjacentStep(curStep, step);

			ssize_t index = getStepIndex(_spOpenSteps, step);
			if (index == -1) {
				step->setParent(curStep);
				step->setGScore(curStep->getGScore() + moveCost);
				step->setHScore(computeHScoreFromCoordToCoord(step->getPosition(),desTitleCoord));

				insertInOpenSteps(step);
			}
			else {
				step = _spOpenSteps.at(index);
				if (curStep->getGScore() + moveCost < step->getGScore()) {
					step->setGScore(curStep->getGScore() + moveCost);

					step->retain();
					_spOpenSteps.erase(index);
					insertInOpenSteps(step);
					step->release();
				}
			}
		}
	} while (_spOpenSteps.size() > 0);

	if (_shortestPath.empty()) {
		log("cannot move to there");
	}
	return result;
}

void BattleScene::insertInOpenSteps(ShortestPathStep *step)
{
	int stepFScore = step->getFScore();
	ssize_t count = _spOpenSteps.size();
	ssize_t i = 0;
	for (; i < count; ++i)
	{
		if (stepFScore <= _spOpenSteps.at(i)->getFScore())
		{
			break;
		}
	}
	_spOpenSteps.insert(i, step);
}

int BattleScene::computeHScoreFromCoordToCoord(const Vec2 &fromCoord, const Vec2 &toCoord)
{
	return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}

int BattleScene::costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep)
{
	return ((fromStep->getPosition().x != toStep->getPosition().x)&& (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}

ssize_t BattleScene::getStepIndex(const Vector<ShortestPathStep*> &steps, const ShortestPathStep *step)
{
	for (ssize_t i = 0; i < steps.size(); ++i)
	{
		if (steps.at(i)->isEqual(step))
		{
			return i;
		}
	}
	return -1;
}

bool BattleScene::isValidTileCoord(Vec2 &tileCoord)
{
	if (tileCoord.x < 0 || tileCoord.y < 0 ||
		tileCoord.x >= _myMap->getMapSize().width ||
		tileCoord.y >= _myMap->getMapSize().height)
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool BattleScene::isWallAtTileCoord(Vec2 &titleCoord)
{
	//if this title can not walk thought return true. Else return false
	auto point = getPositionForTitleCoord(titleCoord);
	for (auto &sp :_allStone)
 	{
		Rect checkBox = Rect(sp->getBoundingBox().getMinX() + 20, sp->getBoundingBox().getMinY(), sp->getBoundingBox().size.width - 40, sp->getBoundingBox().size.height / 2 * sp->getTag());
		if (checkBox.containsPoint(point)) return true;
 	}
	//check for tower
	if ((_allAlliedUnitSprite.back()->getPosition() - point).length() <_allAlliedUnitSprite.back()->getBoundingBox().size.width/2)
	{
		return true;
	}
	if ((_allEnemyUnitSprite.back()->getPosition() - point).length() < _allEnemyUnitSprite.back()->getBoundingBox().size.width / 2)
	{
		return true;
	}
	//check EnemyRestoreArea
	if (_currentPlayerTeamFlg == TEAM_FLG_BLUE){
		if (Rect(_visibleSize.width - 100, _visibleSize.height * 2 - 150, 200, 150).containsPoint(point)) {
			return true;
		}
	}
	else {
		if (Rect(_visibleSize.width - 100, 0, 200, 150).containsPoint(point)) {
			return true;
		}
	}
	//
	return false;
}

PointArray * BattleScene::allWalkableTitlesCoordForTitleCoord(Vec2 tileCoord)
{
	PointArray *tmp = PointArray::create(8);

	bool t = false;
	bool l = false;
	bool b = false;
	bool r = false;

	Vec2 p(tileCoord.x, tileCoord.y - 1);
	if (isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
		t = true;
	}

	p.setPoint(tileCoord.x - 1, tileCoord.y);
	if (isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
		l = true;
	}

	p.setPoint(tileCoord.x, tileCoord.y + 1);
	if (isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
		b = true;
	}

	p.setPoint(tileCoord.x + 1, tileCoord.y);
	if (isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
		r = true;
	}

	p.setPoint(tileCoord.x - 1, tileCoord.y - 1);
	if (t && l && isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}

	p.setPoint(tileCoord.x - 1, tileCoord.y + 1);
	if (b && l && isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}

	p.setPoint(tileCoord.x + 1, tileCoord.y - 1);
	if (t && r && isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}

	p.setPoint(tileCoord.x + 1, tileCoord.y + 1);
	if (b && r && isValidTileCoord(p) && !isWallAtTileCoord(p))
	{
		tmp->addControlPoint(p);
	}
	return tmp;
}

void BattleScene::constructPathAndStartAnimationFromStep(ShortestPathStep *step)
{
	_shortestPath.clear();
	do
	{
		if (step->getParent())
		{
			_shortestPath.insert(0, step);
		}
		step = step->getParent(); 
	} while (step);                
	vector<Vec2> result = {};
	for (const ShortestPathStep *s : _shortestPath)
	{
		log("%s", s->getDescription().c_str());
		result.push_back(Vec2(s->getPosition().x* _myMap->getTileSize().width, s->getPosition().y * _myMap->getTileSize().height));
	}
	//animation
	testObject->stopAllActionsByTag(AUTO_MOVE_ACTION_TAG);
	moveStepAction();
}

void BattleScene::moveStepAction()
{
	//fakeZOrder();
	Vec2 curPos = getTitleCoorForPosition(testObject->getPosition());
	if (_onRespwanFlg) 
	{
		_shortestPath.clear();
	}
	if (_shortestPath.size() == 0)
	{
		testObject->stopAllActionsByTag(_currentMoveActionTag);
		return;
	}

	ShortestPathStep *s = _shortestPath.at(0);

	Vec2 desPos = getPositionForTitleCoord(s->getPosition());
	Vec2 distanVector = desPos - testObject->getPosition();
	float time = distanVector.length() / (_allAlliedUnitData[0].move_speed);
	MoveTo *moveAction = MoveTo::create(time, desPos );
	_mainCharacterIconInMiniMap->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);

	//log("%f", _mini_Icon->getRotation());
	int direc = detectDirectionBaseOnTouchAngle(_mainCharacterIconInMiniMap->getRotation());
	if (direc != 0)
	{
		testObject->actionMoveCharacter(direc);
		if (_moveMode == MOVE_CIRCLE)
		{
			_miniUnit->actionMoveCharacter(direc);
		}

	}

	CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(BattleScene::moveStepAction, this));

	_shortestPath.erase(0);

	Sequence *moveSequence = Sequence::create(moveAction, moveCallback, nullptr);
	moveSequence->setTag(AUTO_MOVE_ACTION_TAG);
	testObject->runAction(moveSequence);
}

void BattleScene::countTime(float dt)
{
	log("%f", dt);
	calCulTime += dt;
}


BattleScene::ShortestPathStep::ShortestPathStep():
_position(Vec2::ZERO),
_gScore(0),
_hScore(0),
_parent(nullptr)
{

}

BattleScene::ShortestPathStep::~ShortestPathStep()
{

}

BattleScene::ShortestPathStep *BattleScene::ShortestPathStep::createWithPosition(const Vec2 pos)
{
	ShortestPathStep *pRet = new ShortestPathStep();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool BattleScene::ShortestPathStep::initWithPosition(const Vec2 pos)
{
	setPosition(pos);
	return true;
}

int BattleScene::ShortestPathStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool BattleScene::ShortestPathStep::isEqual(const ShortestPathStep *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string BattleScene::ShortestPathStep::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}

/////////////////////////////////////////////////////////////////////////////////////
// CREATE MOVE IN CIRCLE
/////////////////////////////////////////////////////////////////////////////////////

Sprite* BattleScene::createMiniMoveCircle() {
	auto miniCircle = Sprite::create("image/screen/battle/mini_circle.png");
	// Ti le la 1/3 chieu cao cua man hinh
	float diameter = Director::getInstance()->getOpenGLView()->getFrameSize().height * MINU_CIRCLE_SCALE;
	float x = float(diameter / miniCircle->getContentSize().height);

	// Do co kich thuoc cua thiet bi voi man hinh hien thi
	float scaleRateX = Director::getInstance()->getOpenGLView()->getFrameSize().width / _visibleSize.width;
	float scaleRateY = Director::getInstance()->getOpenGLView()->getFrameSize().height / _visibleSize.height;

	miniCircle->setScale(x * (1 / scaleRateY));
	miniCircle->setScaleX(x * (1 / scaleRateX));

	return miniCircle;
}

void BattleScene::createMiniControlUnit(int circleType) {
	_miniCircle = createMiniMoveCircle();
	_checkMiniCircleFlg = true;
	Size visibleSize = Director::getInstance()->getVisibleSize();
	float diameter = Director::getInstance()->getOpenGLView()->getFrameSize().height * MINU_CIRCLE_SCALE;
	float x = float(diameter / _miniCircle->getContentSize().height);
	float scaleRateX = Director::getInstance()->getOpenGLView()->getFrameSize().width / _visibleSize.width;
	float scaleRateY = Director::getInstance()->getOpenGLView()->getFrameSize().height / _visibleSize.height;

	circleX = _miniCircle->getContentSize().width * x * (1 / scaleRateX),
		circleY = _miniCircle->getContentSize().height * x * (1 / scaleRateY);

	switch (circleType)
	{
	case MOVE_CIRCLE_LEFT:
		_miniCircle->setPosition(Vec2(
			visibleSize.width / 48 + circleX / 2, 
			visibleSize.height / 32 + circleY / 2));

		addChild(_miniCircle, 100);
		break;
	case MOVE_CIRCLE_RIGHT:
		_miniCircle->setPosition(
			Vec2(
			visibleSize.width - visibleSize.width / 48 - circleX / 2,
			visibleSize.height / 32 + circleY / 2));

		addChild(_miniCircle, 100);
		break;
	default:
		break;
	}

	// scale mini unit = 1/3 miniCircle
	float scaleUnitX = circleX * 1 / 3;
	float scaleUnitY = circleY * 1 / 3;

	_miniUnit = Character::createCharacter(_selectedUnitId);
	_miniUnit->setBirdMode(false);
	_miniUnit->setScale(float(0.25));
	addChild(_miniUnit, 100);
	_miniUnit->setPosition(_miniCircle->getPosition());
}

void BattleScene::rt_attackAnimationandLogic(Document& doc, vector<Sprite*> processUnitSprite, vector<UserUnitInfo>* processUnitData, vector<Sprite*> targetSpriteList, vector<UserUnitInfo>* targetDataList)
{
	log(">>>>RT_ATTACK =================================================");
	for (int i = 0; i < processUnitData->size(); i++)
	{
		/*Battle can have several atacker at the same time
		 * But this function was called for only one player
		 */
		if (strcmp(processUnitData->at(i).uuid.c_str(), doc["uuid"].GetString()) == 0) {
			log("**** ATTACKER FOUND ****");
			Character* cha = (Character*)processUnitSprite[i];
			//bellow code for detect be attacked unit and sync data:
			Sprite* target;
			int saveTargetIndex = -1;
			for (int j = 0; j < targetDataList->size(); j++)
			{
				if (strcmp(doc["target"]["uuid"].GetString(), targetDataList->at(j).uuid.c_str()) == 0) {
					log("**** TARGET FOUND ****");
					target = targetSpriteList[j];
					targetDataList->at(j).hp = doc["target"]["hp"].GetInt(); //sync unit HP
					saveTargetIndex = j;

					int dame = doc["dame"].GetInt();
					if (dame <= 0) {
						dame = 1;
					}
					log("attack dame: %d", dame);

					cha->attackActionByTargetPosition(Vec2(doc["direction_x"].GetDouble(), doc["direction_y"].GetDouble()), doc["user_unit"]["attack_speed"].GetInt(), nullptr, [&, i, target, dame, saveTargetIndex, targetDataList]() {
						if (target == testObject)
						{
							showAttackDame(dame, target->getPosition() + Vec2(0, 100), 1);
							_allAlliedUnitData[0].hp -= dame;
							updateHpAndMpViewLabel();
							saveDameInfo(dame, i, 0, _currentEnemyTeamFlg);
							if (_allAlliedUnitData[0].hp <= 0) {
								runRespawnAction(_allEnemyUnitData[i].uuid.c_str());
							}
						}
						else {
							//processUnitData->at(saveTargetIndex).hp -= dame;
							if (targetDataList->at(saveTargetIndex).hp <= 0) {
								unitDieAction(target, targetDataList, saveTargetIndex);
							}
							//Unit dead process
						}
						updateSlider();
						log(">>>>RT ATTACK END===========================================");
					});
					break;
				}
			}
			log("Attack end");
			return;
		}
	}
	log("ERROR: RT ATTACKER NOT FOUND");
}


