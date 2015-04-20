#include "BatleScene.h"


Scene* BatleScene::createScene(int selectedUnitId, vector<UserSkillInfo> playerSkills)
{
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	scene->getPhysicsWorld()->setGravity(Vect::ZERO);
	auto lay = BatleScene::create(selectedUnitId, playerSkills);
	lay->savePhysicWorld(scene->getPhysicsWorld());
	scene->addChild(lay);
	return scene;
}

BatleScene* BatleScene::create(int unitId,vector<UserSkillInfo> skills)
{
	BatleScene *layer = new BatleScene();
	if (layer && layer->init(unitId,skills)) {
		layer->autorelease();
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}

bool BatleScene::init(int unitId,vector<UserSkillInfo> skills)
{
	if (!LayerBase::init()) {
		return false;
	}

	NodeServer::getInstance()->sendMessageWithName("hello", "Message from battle");

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

	_mainCharacterSkillData = getUnitSkillFromDataBase(_mainCharacterData);




	_allAlliedUnitData.push_back(_mainCharacterData);
	_saveMainStatusData = _mainCharacterData;
	_mainCharacerMaxMp = _mainCharacterData.mp;

	//Bellow vector is store  list of enemy unit id.
	vector<int> a;
	_allEnemyUnitData = getEnemyUnitsData(a);

	//For test
	_redTeamTowerData = UserUnit::getInstance()->getUnitInfoById(1);
	_redTeamTowerData.hp = 5000;
	_redTeamTowerData.attack_range = 200;
	_redTeamTowerData.attack_speed = 5;
	_blueTeamTowerData = UserUnit::getInstance()->getUnitInfoById(2);

	_blueTeamTowerData.hp = 5000;
	_blueTeamTowerData.attack_range = 200;
	_blueTeamTowerData.attack_speed = 5;

	//////////
	//_moveImagePath = "image/unit_new/move/red/";
	//_attackImagePath = "image/unit_new/attack/red/";
	changeAnimationImagePathByUnitId(unitId);

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

	if (_currentPlayerTeamFlg == TEAM_FLG_BLUE) {
		UserBattleInfo info;
		info.name = _mainCharacterData.name;
		info.unitId = _mainCharacterData.mst_unit_id;
		info.imagePath = UserUnit::getInstance()->getUnitImageById(_mainCharacterData.mst_unit_id);
		_blueTeamInfo.push_back(info);
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			UserBattleInfo enemy;
			enemy.name = _allEnemyUnitData[i].name;
			enemy.unitId = _allEnemyUnitData[i].mst_unit_id;
			_redTeamInfo.push_back(enemy);
		}
		
	}
	else {
		UserBattleInfo info;
		info.name = _mainCharacterData.name;
		info.unitId = _mainCharacterData.mst_unit_id;
		_redTeamInfo.push_back(info);
		for (int i = 0; i < ENEMY_NUM; i++)
		{
			UserBattleInfo enemy;
			enemy.name = _allEnemyUnitData[i].name;
			enemy.unitId = _allEnemyUnitData[i].mst_unit_id;
			_blueTeamInfo.push_back(enemy);
		}
	}

	return true;
}
void BatleScene::createContent()
{
	_battleBackround = Node::create();
	_battleBackround->setPosition(Vec2::ZERO);
	_battleBackround->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	addChild(_battleBackround);

	string pathFIle = FileUtils::getInstance()->fullPathForFilename("map/map3.tmx");
	_myMap = TMXTiledMap::create(pathFIle.c_str());

	if (_myMap != nullptr)
	{
		_battleBackround->addChild(_myMap);
		Size CC_UNUSED s = _myMap->getContentSize();
		log("ContentSize: %f, %f", s.width, s.height);
		Vec2 ts = _myMap->getTileSize();
		log("Title Size: %f, %f", ts.x, ts.y);
		Vec2 ts2 = _myMap->getMapSize();
		log("map Size: %f, %f", ts2.x, ts2.y);
		auto& children = _myMap->getChildren();
		SpriteBatchNode* child = nullptr;

		for (const auto &obj : children) {
			child = static_cast<SpriteBatchNode*>(obj);
			child->getTexture()->setAntiAliasTexParameters();
		}

	}
	else {
		log("null");
	}


	createPhysicBolder();
	/*create blue and red line*/
	auto line = DrawNode::create();
	line->drawLine(Vec2(0, 150), Vec2(_visibleSize.width * 2, 150), Color4F::BLUE);
	line->drawLine(Vec2(0, _visibleSize.height * 2 - 150), Vec2(_visibleSize.width * 2, _visibleSize.height * 2 - 150), Color4F::RED);
	line->drawRect(Vec2(_visibleSize.width - 100, 0), Vec2(_visibleSize.width + 100, 150), Color4F::BLUE);
	line->drawRect(Vec2(_visibleSize.width - 100, _visibleSize.height * 2 - 150), Vec2(_visibleSize.width + 100, _visibleSize.height * 2),Color4F::RED);
	_battleBackround->addChild(line);

	auto blueTeamAreaNode = Node::create();
	blueTeamAreaNode->setPhysicsBody(PhysicsBody::createBox(Size(200, 150), PhysicsMaterial(1, 0, 0)));
	blueTeamAreaNode->setPosition(Vec2(_visibleSize.width, 75));
	blueTeamAreaNode->getPhysicsBody()->setCategoryBitmask(ALLIED_CONTACT_CATEGORY_BITMAP);
	blueTeamAreaNode->getPhysicsBody()->setCollisionBitmask(ALLIED_CONTACT_COLLISION_BITMAP);
	blueTeamAreaNode->getPhysicsBody()->setGravityEnable(false);
	blueTeamAreaNode->getPhysicsBody()->setDynamic(false);
	_battleBackround->addChild(blueTeamAreaNode);

	auto redTeamAreaNode = Node::create();
	redTeamAreaNode->setPhysicsBody(PhysicsBody::createBox(Size(200, 150), PhysicsMaterial(1, 0, 0)));
	redTeamAreaNode->getPhysicsBody()->setCategoryBitmask(ENEMY_CONTACT_CATEGORY_BITMAP);
	redTeamAreaNode->getPhysicsBody()->setCollisionBitmask(ENEMY_CONTACT_COLLISION_BITMAP);
	redTeamAreaNode->getPhysicsBody()->setGravityEnable(false);
	redTeamAreaNode->getPhysicsBody()->setDynamic(false);
	redTeamAreaNode->setPosition(Vec2(_visibleSize.width, 2 * _visibleSize.height - 75));
	_battleBackround->addChild(redTeamAreaNode);


	auto redTower = Sprite::create("tower_red.png");
	redTower->setPosition(Vec2(_visibleSize.width, _visibleSize.height * 2 - 150));
	//redTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(redTower);
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

	auto blueTower = Sprite::create("tower_blue.png");
	blueTower->setPosition(Vec2(_visibleSize.width, 250));
	//blueTower->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(blueTower);
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

	string path = _moveImagePath;
	path.append("unit_00_08_1.png");
	testObject = Sprite::create(path.c_str());
	testObject->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	_battleBackround->addChild(testObject, MID);
	testObject->setPosition(Vec2(_visibleSize.width, 100));
	testObject->setScale(IMAGE_SCALE);
	testObject->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 1)/*,Vec2(0,-50)*/));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->getPhysicsBody()->setContactTestBitmask(0x1);
	testObject->getPhysicsBody()->setCategoryBitmask(ALLIED_CONTACT_CATEGORY_BITMAP);
	testObject->getPhysicsBody()->setCollisionBitmask(ALLIED_CONTACT_COLLISION_BITMAP);


	_statusContainer = Node::create();
	testObject->addChild(_statusContainer);
	_statusContainer->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height));


//  	ArmatureDataManager::getInstance()->addArmatureFileInfo("animation/walk/walk.ExportJson");
//  	testArmature = Armature::create("walk");
//  	testArmature->setPosition(Vec2(0, 0));
//  	testObject->addChild(testArmature);
//  	testArmature->setScale(1.0f);

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

	_hpViewLabel = Label::createWithSystemFont(DataUtils::numberToString(_mainCharacterData.hp), "", 20);
	_hpViewLabel->setColor(Color3B::GREEN);
	_hpViewLabel->setContentSize(Size(150, 50));
	_hpViewLabel->setPosition(Vec2(_mainCharacterHpBar->getContentSize()) - Vec2(0, 50));
	_mainCharacterHpBar->addChild(_hpViewLabel);


	Vec2 slotPos = Vec2(45, topMenu->getContentSize().height / 2);

	_characterImageViewNode = ClippingNode::create();
	_characterImageViewNode->setAlphaThreshold(0);
	_characterImageViewNode->setPosition(Vec2::ZERO);

	auto mask = Sprite::create("image/screen/battle/icon_slot.png");
	mask->setPosition(slotPos);
	_characterImageViewNode->setStencil(mask);

	auto icon = Sprite::create(UserUnit::getInstance()->getUnitImageById(_mainCharacterData.mst_unit_id));
	icon->setPosition(slotPos + Vec2(0,0));
	icon->setScale(1.4f);
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

	_mainCharacterIconInMiniMap = Sprite::create("mini_icon.png");
	_mainCharacterIconInMiniMap->setPosition(Vec2(_selectRect->getContentSize().width / 2, _selectRect->getContentSize().height / 2));
	_selectRect->addChild(_mainCharacterIconInMiniMap);




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
	_skill1Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	Size baseSize = _skill1Button->getContentSize();
	_skill1Button->setTag(TAG_SKILL_1);
	_skill1Button->setSwallowTouches(true);
	_skill1Button->setPosition(Vec2(_visibleSize.width / 2 - 1.5 * baseSize.width - 20, baseSize.height / 2 + baseMargin));
	addChild(_skill1Button);
	displaySkillMpInButton(_skill1Button, _mainCharacterSkillData[0].mp_cost);

	_skill2Button = Button::create();
	_skill2Button->loadTextureNormal(skill2ImagePath.c_str());
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill2Button->setTag(TAG_SKILL_2);
	_skill2Button->setSwallowTouches(true);
	_skill2Button->setPosition(Vec2(_visibleSize.width / 2 - 0.5 *baseSize.width - 10, baseSize.height / 2 + baseMargin));
	addChild(_skill2Button);
	displaySkillMpInButton(_skill2Button, _mainCharacterSkillData[1].mp_cost);

	_skill3Button = Button::create();
	_skill3Button->loadTextureNormal(skill3ImagePath.c_str());
	_skill3Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill3Button->setTag(TAG_SKILL_3);
	_skill3Button->setSwallowTouches(true);
	_skill3Button->setPosition(Vec2(_visibleSize.width / 2 + 0.5*baseSize.width + 10, baseSize.height / 2 + baseMargin));
	addChild(_skill3Button);
	displaySkillMpInButton(_skill3Button, _playerSkills[0].mp_cost);

	_skill4Button = Button::create();
	_skill4Button->loadTextureNormal(skill4ImagePath.c_str());
	_skill4Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill1ButtonCallback, this));
	_skill4Button->setTag(TAG_SKILL_4);
	_skill4Button->setSwallowTouches(true);
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

	_mpViewlabel = Label::createWithSystemFont(DataUtils::numberToString(_mainCharacterData.mp), "", 20);
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
		sp->setPosition(Vec2(_visibleSize.width +(i-1) * 70, 2 * _visibleSize.height-100));
		sp->setScale(IMAGE_SCALE);
		sp->setTag(i);
		sp->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 0)/*, Vec2(0, -50)*/));
		sp->getPhysicsBody()->setRotationEnable(false);
		sp->getPhysicsBody()->setGravityEnable(false);
		sp->getPhysicsBody()->setContactTestBitmask(0x1);
		sp->getPhysicsBody()->setCategoryBitmask(ENEMY_CONTACT_CATEGORY_BITMAP);
		sp->getPhysicsBody()->setCollisionBitmask(ENEMY_CONTACT_COLLISION_BITMAP);
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

		vector<string> tmp = {};
		_enemyStatusImagePath.push_back(tmp);
	}

	_miniMap->addChild(node, 1);

	/*_testAttackTarget = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
	_testAttackTarget->setPosition(_visibleSize);
	_testAttackTarget->setScale(IMAGE_SCALE);
	_battleBackround->addChild(_testAttackTarget);*/

	auto enemyIcon1 = Sprite::create("image/screen/battle/enemyicon.png");
	enemyIcon1->setOpacity(255);
	vector<string> tmp1 = {};
	switch (_currentPlayerTeamFlg)
	{
	case TEAM_FLG_RED:
		_allAlliedUnitSprite.push_back(redTower);
		_allAlliedUnitHpBar.push_back(redTHpBar);
		_allAlliedUnitData.push_back(_redTeamTowerData);

		_allEnemyAttachDelay.push_back(false);
		_allEnemyHpBar.push_back(blueTHpBar);
		_allEnemyUnitData.push_back(_blueTeamTowerData);
		_allEnemyUnitSprite.push_back(blueTower);
		
		_enemyStatusImagePath.push_back(tmp1);
		
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

		_allEnemyAttachDelay.push_back(false);
		_allEnemyHpBar.push_back(redTHpBar);
		_allEnemyUnitData.push_back(_redTeamTowerData);
		_allEnemyUnitSprite.push_back(redTower);
		_enemyStatusImagePath.push_back(tmp1);

		enemyIcon1->setPosition(Vec2(redTower->getPositionX()*positionXScaleRate, redTower->getPositionY()*positionYScaleRate));
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
}

void BatleScene::displaySkillMpInButton(Button *parent, int mp)
{
	auto lb = Label::createWithSystemFont(DataUtils::numberToString(mp), "", 25);
	lb->setColor(Color3B(0,0,243));
	lb->setPosition(Vec2(parent->getContentSize() / 2) - Vec2(0, 30));
	lb->setTag(TAG_MP_LABEL);
	parent->addChild(lb);
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
// 	srand(time(NULL));
	time(&timer);
	timeinfo = localtime(&timer);

	auto dispath = Director::getInstance()->getEventDispatcher();
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(BatleScene::onPhysicContactBegin, this);
	contactListener->onContactPreSolve = CC_CALLBACK_2(BatleScene::onPhysicContactPreSolve, this);
	dispath->addEventListenerWithSceneGraphPriority(contactListener, this);
	scheduleUpdate();

}

void BatleScene::update(float delta)
{
	//log("Current Hp: %d", _characterCurentHp);
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
		vector<int> detectedUnit = detectUnitInAoe(_onSelectSkillData, ENEMY_FLAG, false);
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
	if (random(1,50) < 2)
	{
		enemyUnitAutoMoveTest();
	}

	fakeZOrder();

	UnitData_temp ud;
	ud.user_id = UserModel::getInstance()->getUserInfo()._id;
	ud.room_id = UserModel::getInstance()->getRoomId();
	ud.mst_unit_id = UserModel::getInstance()->getSelectedUnitId();
	ud.hp = _mainCharacterData.hp;
	ud.mp = _mainCharacterData.mp;
	ud.position_x = testObject->getPositionX();
	ud.position_y = testObject->getPositionY();

	BattleAPI::getInstance()->sendMoveEvent(ud);

}
void BatleScene::checkForAutoAttack()
{
	if (_onRespwanFlg) return;
	//float area = IMAGE_SCALE*_autoAttackArea->getContentSize().width / 2 + 25;
	//Check for main character attack
	for (int i = 0; i < _allEnemyUnitSprite.size(); i++)
	{
		//calculate distance vector to decide the direction of attack animation
		auto posDistan = _allEnemyUnitSprite[i]->getPosition() - testObject->getPosition();
		//calculate distance offset by distance vector angle
		int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
		if (posDistan.length() - _allEnemyUnitSprite[i]->getBoundingBox().size.width/4 < ATTACK_AOE*_mainCharacterData.attack_range/100.0f && _allEnemyUnitSprite[i]->isVisible()) {
			if (testObject->getActionByTag(_currentAttackActionTag) == nullptr && _onDelayAttackFlg == false) {
				rotateCharacter(testObject,direc);
				auto ani = createAttackAnimationWithDefine(direc, _attackImagePath);
				//auto action = RepeatForever::create(Animate::create(ani)); //user repeat for change in future
				auto call1 = CallFuncN::create(CC_CALLBACK_0(BatleScene::characterAttackCallback, this));
				auto action = Sequence::create(Animate::create(ani),call1,nullptr);
				action->setTag(direc * 10);
				_currentAttackActionTag = direc * 10;
				testObject->runAction(action);
				_indexOfBeAttackEnemy = i;

				_onDelayAttackFlg = true;
				this->runAction(Sequence::create(DelayTime::create(_mainCharacterData.attack_speed), CallFuncN::create([&](Ref *p) {
					_onDelayAttackFlg = false;
					testObject->stopActionByTag(_currentAttackActionTag);
					//_alltargetUnit[_indexOfBeAttackEnemy]->stopActionByTag(10 - (_currentAttackActionTag / 10));
				}), nullptr));
			}
		}

		//check for enemy unit auto attack
		if (posDistan.length() < ATTACK_AOE*_allEnemyUnitData[i].attack_range/100.0f && _allEnemyUnitSprite[i]->isVisible() && !_allEnemyUnitData[i].isStun) {

			if (/*_allEnemyUnitSprite[i]->getNumberOfRunningActions() < 1 &&*/ _allEnemyAttachDelay[i] == false) {
				_allEnemyUnitSprite[i]->stopAllActionsByTag(_allEnemyUnitSprite[i]->getTag());
				string path = "image/unit_new/attack/red/";
				auto target_ani = createAttackAnimationWithDefine(10 - direc, path);
				auto call2 = CallFuncN::create(CC_CALLBACK_1(BatleScene::enemyAttackCallback, this,i));
				auto action2 = Sequence::create(Animate::create(target_ani), call2, nullptr);
				_allEnemyAttachDelay[i] = true;
				auto forCallback = Sequence::create(DelayTime::create(_allEnemyUnitData[i].attack_speed), CallFuncN::create([&, i](Ref *p) {
					if (i < _allEnemyAttachDelay.size()) {
						_allEnemyAttachDelay[i] = false;
					}
				}), nullptr);
				action2->setTag(1111);
				//rotateCharacter(_alltargetUnit[i], 10 - direc);
				_allEnemyUnitSprite[i]->runAction(Spawn::create(action2, forCallback, nullptr));
				//_indexOfRunningActionTarget = i;
			}
		}
	}

	//check for run fountain restore action
	if( (testObject->getPosition()-Vec2(_visibleSize.width,0)).length() < 150)
	{
		fountainRestoreEffect();
	}
	else {
		testObject->stopActionByTag(FOUNTAIN_ACTION);
	}
}
void BatleScene::characterAttackCallback()
{
	//if main character die before atack event end.
	if (_onRespwanFlg) return;
	//log("charater");
	//check for enemy unit still alive
	if (_allEnemyUnitData[_indexOfBeAttackEnemy].hp > 0) {
		//default dame = attack -  defence
		int dame = (_mainCharacterData.attack - _allEnemyUnitData[_indexOfBeAttackEnemy].defence);
		//calculate dame by unit attribute ( fire, water, wind, sand.... or none)
		float defaultDameRate = caculDameRate(_mainCharacterData.element, _allEnemyUnitData[_indexOfBeAttackEnemy].element);

		//random dame by rate from 85% to 100%
		dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);
		//check if dame < 0 (in case of attack < defence
		if (dame <= 0) {
			dame = 1;
		}
		//show hp lost = final attack dame
		showAttackDame(dame, _allEnemyUnitSprite[_indexOfBeAttackEnemy]->getPosition() + Vec2(0, 100), 1);

		_allEnemyUnitData[_indexOfBeAttackEnemy].hp -= dame;
		//save dame deal info to battle result
		saveDameInfo(dame, 0, _indexOfBeAttackEnemy, _currentPlayerTeamFlg);
		//check if enemy die
		if (_allEnemyUnitData[_indexOfBeAttackEnemy].hp <= 0)
		{
			enemyDieAction(_indexOfBeAttackEnemy);
			return;
		}
		//update enemy hp status bar
		_allEnemyHpBar[_indexOfBeAttackEnemy]->setPercent(ceil((100.0f * _allEnemyUnitData[_indexOfBeAttackEnemy].hp / _allEnemuUnitMaxHp[_indexOfBeAttackEnemy])));
		

	}
	else {
		//enemyDieAction(_indexOfBeAttackEnemy);
	}
}

void BatleScene::enemyDieAction(int id)
{
	
	_allEnemyUnitSprite[id]->stopAllActions();
	while (_allEnemyUnitSprite[id]->getChildByTag(BUFF_STATUS_TAG) != nullptr)
	{
		_allEnemyUnitSprite[id]->removeChildByTag(BUFF_STATUS_TAG);
	}
	_allEnemyUnitData[id].isStun = false;
	_allEnemyAttachDelay[id] = false;
	_allEnemyUnitSprite[id]->setVisible(false);
	_allEnemyIconInMinimap[id]->setVisible(false);
	saveKillDeadInfo(0, id, _currentPlayerTeamFlg);
	_indexOfBeAttackEnemy = -1;
	testObject->stopActionByTag(_currentAttackActionTag);
	if (id == _allEnemyUnitData.size() - 1) {
		endBattle();
	}
	enemyRespawAction(id);
	_enemyTeamTotalDead += 1;
	if (_enemyTeamTotalDead == 5)
	{
		endBattle();
	}
}
void BatleScene::enemyAttackCallback(Ref *pSEnder, int i)
{
	if (_onRespwanFlg) return;
	Sprite *_sprite = (Sprite*)pSEnder;
	_sprite->stopActionByTag(1111);
	//log("enemy");
	if (_mainCharacterData.hp > 0)
	{
		if (!_onRespwanFlg)
		{
			int dame = _allEnemyUnitData[i].attack - _mainCharacterData.defence;
			float defaultDameRate = caculDameRate(_allEnemyUnitData[i].element, _mainCharacterData.element);

			dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);
			if (dame <= 0) {
				dame = 1;
			}
			if (dame <= 0)
			{
				dame = 1;
			}
			showAttackDame(dame, testObject->getPosition() + Vec2(0, 100), 2);
			_mainCharacterData.hp -= dame;
			saveDameInfo(dame, i, 0, _currentEnemyTeamFlg);
			//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
			if (_mainCharacterData.hp <= 0) {
				runRespawnAction(i);
				return;
			}
			//log("Percent: %d", ceil(float(float(_characterCurentHp) / float(_unitData.hp)) * 100));
			_mainCharacterMiniHpBar->setPercent(ceil((100.0f * _mainCharacterData.hp / _allAlliedUnitMaxHp[0])));
			_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());
			
		}
	}
	else {
		runRespawnAction(i);
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

void BatleScene::runRespawnAction(int killerId)
{
	if (_onRespwanFlg) return;
	_onRespwanFlg = true;
	saveKillDeadInfo(killerId, 0, _currentEnemyTeamFlg);
	_alliedTeamTotalDead += 1;
	if (_alliedTeamTotalDead == 5)
	{
		endBattle();
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
	_mainCharacterData.hp = _allAlliedUnitMaxHp[0];
	_mainCharacterData.mp = _mainCharacerMaxMp;
	testObject->setVisible(false);
	auto action = Repeat::create(Sequence::create(DelayTime::create(1), CallFuncN::create([&](Ref *pSender){
		Label* lb = dynamic_cast<Label*>(pSender);
		int t = DataUtils::stringToFloat(lb->getString());
		lb->setString(DataUtils::numberToString(t - 1));
	}), nullptr), 5);
	auto action2 = CallFuncN::create([&](Ref *pSEnder){
		Label* lb = dynamic_cast<Label*>(pSEnder);
		lb->removeFromParent();
		testObject->setPosition(Vec2(_visibleSize.width, 100));
		testObject->setVisible(true);
		auto action3 = Spawn::create(Blink::create(0.6f, 6), Sequence::create(DelayTime::create(RESPAWN_DELAY), CallFuncN::create(CC_CALLBACK_0(BatleScene::removeRespawnFlg,this)), nullptr), nullptr);
		testObject->runAction(action3);
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		//SEND RESPAWN ACTIOn
	});
	timeLb->runAction(Sequence::create(action, action2, nullptr));
}
void BatleScene::removeRespawnFlg()
{
	testObject->setVisible(true);
	_onRespwanFlg = false;
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
	if (_onRespwanFlg) return false;
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
	//fakeZOrder();
	auto distanVector = touch->getLocation() - _touchStartPoint;
	if (distanVector.length() < 200) {
		_touchMoveEndSprite->setPosition(touch->getLocation());
	}
	else {
		_touchMoveEndSprite->setPosition(_touchStartPoint + Vec2(200 * cos(distanVector.getAngle()), 200 * sin(distanVector.getAngle())));
	}
	_touchMoveEndSprite->setVisible(true);


	if (distanVector.length() < _touchMoveBeginSprite->getContentSize().width / 6) {
		testObject->getPhysicsBody()->setVelocity(Vect(0, 0));
		testObject->stopActionByTag(_currentMoveActionTag);
		return;
	}

	testObject->stopActionByTag(_currentAttackActionTag);
	if (_moveMode == MOVE_MANUAL) {
		testObject->getPhysicsBody()->setVelocity(Vect(_mainCharacterData.move_speed*cos(distanVector.getAngle()), _mainCharacterData.move_speed*sin(distanVector.getAngle())));
		_mainCharacterIconInMiniMap->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);

		//log("%f", _mini_Icon->getRotation());
		int direc = detectDirectionBaseOnTouchAngle(_mainCharacterIconInMiniMap->getRotation());
		if (direc != 0) actionCharacter(direc, testObject);
	}
	else {
		
	}

	
	
	
	//_touchStartPoint = touch->getLocation();
	
	
}
/*
7--8--9
4-YOU-6
1--2--3
*/

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
void BatleScene::actionCharacter(int directionId, Sprite *characterSprite)
{
	if (characterSprite->getNumberOfRunningActions() > 0) {
		if (characterSprite->getActionByTag(directionId) != nullptr) {
			//log("!!!");
			return;
		}
	}
	characterSprite->stopAllActionsByTag(_currentMoveActionTag);
	auto action = Animate::create(createMoveAnimationWithDefine(directionId, _moveImagePath));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	_currentMoveActionTag = directionId;
	characterSprite->runAction(repeat);

//  	char type[30] = { 0 };
//  	sprintf(type, "walk_0%d", directionId);
//  	testArmature->getAnimation()->stop();
//  	testArmature->getAnimation()->play(type);

}
void BatleScene::updateMiniMap()
{
	if (testObject == nullptr) return;
	auto objectPos = testObject->getPosition();

	float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
	float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
	_selectRect->setPosition(Vec2(objectPos.x*positionXScaleRate,objectPos.y*positionYScaleRate));
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		_allEnemyIconInMinimap[i]->setPosition(Vec2(_allEnemyUnitSprite[i]->getPositionX()*positionXScaleRate, _allEnemyUnitSprite[i]->getPositionY()*positionYScaleRate));
	}
}


void BatleScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	//log("touch END");
	/*if (_selectTargetSprite->isVisible()) {
		_selectTargetSprite->setVisible(false);
		runAttackAnimation();
		//_moveDisableFlg = false;
	}
	else {*/
	testObject->stopActionByTag(_currentAttackActionTag);
	if (_moveMode == MOVE_MANUAL) {
			testObject->getPhysicsBody()->setVelocity(Vect::ZERO);
			testObject->stopActionByTag(_currentMoveActionTag);
			_touchMoveBeginSprite->setVisible(false);
			_touchMoveEndSprite->setVisible(false);
	}
	else {
		//fakeZOrder();
		_touchMoveBeginSprite->setVisible(false);
		_touchMoveEndSprite->setVisible(false);
		auto distanVector = touch->getLocation() - Vec2(_visibleSize / 2);
		auto vec = AStarPathFindingAlgorithm(testObject->getPosition(), testObject->getPosition() + distanVector);
		/*auto move = MoveBy::create(distanVector.length() / _mainCharacterData.move_speed, distanVector);
		auto moveAction = Sequence::create(move, CallFuncN::create([&](Ref *p){
			_touchMoveBeginSprite->setVisible(false);
			_touchMoveEndSprite->setVisible(false);
			testObject->stopActionByTag(_currentMoveActionTag);
			testObject->stopActionByTag(88);
		}), nullptr);
		moveAction->setTag(88);
		testObject->stopActionByTag(88);
		testObject->runAction(moveAction);
		_mainCharacterAvata->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);

		//log("%f", _mini_Icon->getRotation());
		int direc = detectDirectionBaseOnTouchAngle(_mainCharacterAvata->getRotation());
		if (direc != 0) actionCharacter(direc, testObject);*/
	}
	/*}*/
	
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

/*This function will be change in future with GAF animation file or another unit image
In my opinion, animation image path must be store in database or something have sane struct
*/
void BatleScene::changeAnimationImagePathByUnitId(int unitId)
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

void BatleScene::createRandomRock()
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
		body->setGravityEnable(false);
		body->setContactTestBitmask(0x1);
		sp->setPhysicsBody(body);
		sp->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		sp->setTag(2);
		sp->setPosition(Vec2(random(0.1f, 0.9f)*_visibleSize.width * 2,random(0.1f,0.9f)*_visibleSize.height*2));
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
		tree->setPosition(Vec2(random(0.1f, 0.9f)*_visibleSize.width * 2, random(0.1f, 0.9f)*_visibleSize.height * 2));
		_allStone.push_back(tree);
		_battleBackround->addChild(_allStone.back(),ceil(_visibleSize.height*3 - tree->getPositionY()));
	}
}

void BatleScene::fakeZOrder()
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

bool BatleScene::onPhysicContactBegin(const PhysicsContact &contact)
{
	return true;
}

bool BatleScene::onPhysicContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
// 	log("contact pre solve");
	auto spriteA = contact.getShapeA()->getBody()->getNode();
	auto spriteB = contact.getShapeB()->getBody()->getNode();

	return true;
}

void BatleScene::rotateCharacter(Sprite *target, int direc)
{
	//log("Rotate direc %d", direc);
	//auto animation = Animation::create();
	char szName[100] = { 0 };
	sprintf(szName, "unit_00_0%d_%d.png", direc, 1);
	string p = _moveImagePath;
	p.append(szName);
	//log("%s", p.c_str());
	//animation->addSpriteFrameWithFile(p.c_str());
	//animation->setDelayPerUnit(ANIMETE_DELAY);
	//animation->setRestoreOriginalFrame(true);
	//target->runAction(Animate::create(animation));
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(p.c_str());
	target->setTexture(texture);
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
		//changeAnimationImagePathByUnitId();
		if (Director::getInstance()->isPaused())
		{
			Director::getInstance()->resume();
		}
		else {
			Director::getInstance()->pause();
			//TODO FIX PARAM DATA
			BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		}
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
UserUnitInfo BatleScene::getUnitDataFromDataBase(int unitId)
{

	return UserUnit::getInstance()->getUnitInfoById(unitId);

}

vector<UserSkillInfo> BatleScene::getUnitSkillFromDataBase(UserUnitInfo unitData)
{
	vector<UserSkillInfo> result;
	result.push_back(UserSkill::getInstance()->getSkillInfoById(unitData.skill1_id));
	result.push_back(UserSkill::getInstance()->getSkillInfoById(unitData.skill2_id));
	return result;
}

vector<UserUnitInfo> BatleScene::getEnemyUnitsData(vector<int> enemyIdList)
{
	vector<UserUnitInfo> returnData;
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		returnData.push_back(UserUnit::getInstance()->getUnitInfoById(random(1,6)));
	}
	

	return returnData;
}

void BatleScene::autoRestoreHpAndMp()
{ 
	_mainCharacterData.hp += _mainCharacterData.hp_heal / RESTORE_MULTI;
	if (_mainCharacterData.hp > _allAlliedUnitMaxHp[0]) {
		_mainCharacterData.hp = _allAlliedUnitMaxHp[0];
	}
	_mainCharacterData.mp += _mainCharacterData.mp_heal/RESTORE_MULTI;
	if (_mainCharacterData.mp > _mainCharacerMaxMp)
	{
		_mainCharacterData.mp = _mainCharacerMaxMp;
	}

	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyUnitData[i].hp += _allEnemyUnitData[i].hp_heal/RESTORE_MULTI;
		if (_allEnemyUnitData[i].hp > _allEnemuUnitMaxHp[i]) {
			_allEnemyUnitData[i].hp = _allEnemuUnitMaxHp[i];
		}
	}
	updateSlider();
	BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
}

void BatleScene::updateSlider()
{
	_mainCharacterMiniHpBar->setPercent(ceil((100.0f *_mainCharacterData.hp / _allAlliedUnitMaxHp[0])));
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());

	_mainCharacterMpBar->setPercent(ceil((100.0f * _mainCharacterData.mp / _mainCharacerMaxMp)));

	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyHpBar[i]->setPercent(ceil(( 100.0f * _allEnemyUnitData[i].hp / _allEnemuUnitMaxHp[i])));
	}
}
void BatleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onRespwanFlg) return;
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
		if(skill.range_distance > 0 && skill.mp_cost <= _mainCharacterData.mp ) longPressAction(bt,skill);
		_showSkillTargetFlag = true;
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		//Progress Timer
		_showSkillTargetFlag = false;
		if (skill.target_type == TARGET_ONE && skill.skill_type == TYPE_ATTACK && _indexOfBeAttackEnemy < 0) {
			log("Invalid attack target");
			return;
		}
		if (skill.mp_cost <= _mainCharacterData.mp){
			bt->setColor(Color3B::GRAY);
			bt->setTouchEnabled(false);
			bt->setEnabled(true);
			playSkill(skill);
			showCoolDownTime(bt, skill.cooldown_time);
			bt->runAction(Sequence::create(DelayTime::create(skill.cooldown_time), CallFuncN::create([&, tag](Ref *p) {
				auto button = (Button*)p;
				button->setEnabled(true);
				button->setTouchEnabled(true);
				button->setColor(Color3B::WHITE);
			}), nullptr));
			while (_battleBackround->getChildByTag(DRAW_UNIT))
			{
				_battleBackround->removeChildByTag(DRAW_UNIT);
			}
			
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

void BatleScene::showCoolDownTime(Button *parentButton, int time)
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

void BatleScene::playSkill(UserSkillInfo skillData)
{
	for (auto& e :_allEnemyUnitSprite)
	{
		e->setColor(Color3B::WHITE);
	}
	UserSkillInfo skill = skillData;
	_mainCharacterData.mp -= skill.mp_cost;
	//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
	_mainCharacterMpBar->setPercent(ceil(100.0f * _mainCharacterData.mp / _mainCharacerMaxMp));
	switch (skill.effect_type)
	{
	case TYPE_BUFF:
		skillBuffAction(skill);
		break;
	case TYPE_RESTORE:
		skillRestoreAction(skill);
		break;
	case TYPE_ATTACK:
		skillAttackAction(skill);
		break;
	case TYPE_POISON:
		skillPoisonAction(skill);
		break;
	case TYPE_STUN:
		skillStunAction(skill);
		break;
	default:
		break;
	}

}


void BatleScene::skillRestoreAction(UserSkillInfo skillInfo)
{
	switch (skillInfo.multi_effect)
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

void BatleScene::skillBuffAction(UserSkillInfo skillInfo)
{
	switch (skillInfo.multi_effect)
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

void BatleScene::skillAttackAction(UserSkillInfo skillInfo)
{
	switch (skillInfo.multi_effect)
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

void BatleScene::skillRestoreAll(UserSkillInfo skillInfo)
{
	int value = 0;
	switch (skillInfo.corrett_value)
	{
	case DAME_TYPE_PERCENT:
		value = ceil(1.0f*_saveMainStatusData.hp *skillInfo.corrett_value/100.0f);
		break;
	case DAME_TYPE_PURE:
		value = ceil(skillInfo.corrett_value);
		break;
	}

	createSorceryEffect(testObject, SORCERY_GREEN);

	if (skillInfo.range_distance > 0) 
	{

		vector<int> allUnitIndex = detectUnitInAoe(skillInfo, ALLIED_FLAG);

		for (int &index : allUnitIndex)
		{
			_allAlliedUnitData[index].hp += value;
			if (_allAlliedUnitData[index].hp > _allAlliedUnitMaxHp[index]) {
				_allAlliedUnitData[index].hp = _allAlliedUnitMaxHp[index];
			}
			_allAlliedUnitHpBar[index]->setPercent( 100.0f *_allAlliedUnitData[index].hp / _allEnemuUnitMaxHp[index]);
		
		}
	}
	else {

		log("Restore All");
		for (int i = 0; i < _allAlliedUnitData.size(); i++)
		{
			_allAlliedUnitData[i].hp += value;
			if (_allAlliedUnitData[i].hp > _allAlliedUnitMaxHp[i]) {
				_allAlliedUnitData[i].hp = _allEnemuUnitMaxHp[i];
			}
			//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
			_allAlliedUnitHpBar[i]->setPercent(100.0f * _allAlliedUnitData[i].hp/ _allEnemuUnitMaxHp[i]);

		}
	}

}

void BatleScene::skillRestoreOne(UserSkillInfo skillInfo)
{
	log("Restore One");

	int value = 0;
	switch (skillInfo.correct_type)
	{
	case DAME_TYPE_PERCENT:
		value = ceil(1.0f*_saveMainStatusData.hp *skillInfo.corrett_value/100.0f);
		break;
	case DAME_TYPE_PURE:
		log("Restore One: DAME_TYPE_PURE");
		value = ceil(skillInfo.corrett_value);
		break;
	}
	_mainCharacterData.hp += value;
	if (_mainCharacterData.hp > _allEnemuUnitMaxHp[0]) {
		_mainCharacterData.hp = _allEnemuUnitMaxHp[0];
	}
	//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
	_mainCharacterMiniHpBar->setPercent(100.0f * _mainCharacterData.hp / _allEnemuUnitMaxHp[0]);
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());
	
	/////////////////////////////////////////
	///////////RUN EFFECT HEAL ONE
	createSorceryEffect(testObject , SORCERY_GREEN);

	Effect* effectHeal = Effect::createWithParticle(PARTICLE_HEAL, effectHeal->EC_Other, effectHeal->Restore);
	effectHeal->setTag(10);
	testObject->addChild(effectHeal);
	effectHeal->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2));

	auto healSequence = Sequence::create(
		DelayTime::create(DELAY_RESTORE)
		, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
		, nullptr);
	effectHeal->runAction(healSequence);

}

void BatleScene::skillHelpAll(UserSkillInfo skillInfo)
{
	int value = 0;
}

void BatleScene::skillHelpOne(UserSkillInfo skillInfo)
{
	float value = 1.0f;
	int pureValue = 0;
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
	int saveValue = 0;

	Effect* effect = new Effect();

	switch (skillInfo.buff_effect_type)
	{
	case SKILL_HELP_TYPE::HP:
	{
		log("help HP");
		saveValue = 1.0f*_saveMainStatusData.hp*(value - 1.0f) + pureValue;
		_mainCharacterData.hp += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.hp -= saveValue;
		}), nullptr));
		

		/////////////////////////////////////////
		///////////RUN EFFECT HP RESTORE
		createSorceryEffect(testObject, SORCERY_GREEN);

		Effect* hp = Effect::createWithParticle(PARTICLE_HEAL, effect->EC_Other, effect->Restore);
		testObject->addChild(hp);
		hp->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2));

		auto hpSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
			, nullptr);
		hp->runAction(hpSequence);

		break;
	}
	case SKILL_HELP_TYPE::HP_RESTORE:
	{
		saveValue = 1.0f*_saveMainStatusData.hp_heal*(value -1.0f) + pureValue;
		_mainCharacterData.hp_heal += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.hp_heal -= saveValue;
		}), nullptr));

		/////////////////////////////////////////
		///////////RUN EFFECT HP RESTORE
		createSorceryEffect(testObject, SORCERY_GREEN);

		Effect* hpRestore = Effect::createWithParticle(PARTICLE_HEAL, effect->EC_Other, effect->Restore);
		testObject->addChild(hpRestore);
		hpRestore->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2));

		auto hpRestoreSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
			, nullptr);
		hpRestore->runAction(hpRestoreSequence);

		break;
	}
	case SKILL_HELP_TYPE::MP:
		saveValue = 1.0f*_saveMainStatusData.mp*(value-1.0f) + pureValue;
		_mainCharacterData.mp += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.mp -= saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::MP_RESTORE:
		saveValue = 1.0f*_saveMainStatusData.mp_heal*(value-1.0f) + pureValue;
		_mainCharacterData.mp_heal += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.mp_heal -= saveValue;
		}), nullptr));
		break;
	case SKILL_HELP_TYPE::ATTACK_DAME:
	{
		saveValue = 1.0f*_saveMainStatusData.attack*(value - 1.0f) + pureValue;
		_mainCharacterData.attack += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		log("increase attack by %d", saveValue);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.attack -= saveValue;
			log("remove attack buff %d", saveValue);
		}), nullptr));

		displayUnitStatus(testObject, BUFF_ATTACK, skillInfo);
		break;
	}
	case SKILL_HELP_TYPE::DEFENCE:
	{
		saveValue = 1.0f*_saveMainStatusData.defence*(value-1.0f) + pureValue;
		log("increase defence by %d", saveValue);
		_mainCharacterData.defence += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		displayUnitStatus(testObject, BUFF_DEFENCE, skillInfo);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			if (saveValue > 0) {
				_mainCharacterData.defence  -= saveValue;
			}
			
			log("remove defence buff %d",saveValue);
		}), nullptr));
		
		/////////////////////////////////////////
		///////////RUN EFFECT HELP DEFENCE
		createSorceryEffect(testObject, SORCERY_BLUE);

		Effect* effectHelpDefence = Effect::createWithParticle(PARTICLE_DEFENCE_H, effect->EC_Other, effect->Help_Defence);
		testObject->addChild(effectHelpDefence);
		effectHelpDefence->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2));

		auto helpDefenceSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
			, nullptr);
		effectHelpDefence->runAction(helpDefenceSequence);


		break;
	}
	case SKILL_HELP_TYPE::ATTACK_RANGE:
	{
		saveValue = 1.0f*_saveMainStatusData.attack_range*(value-1.0f) + pureValue;
		_mainCharacterData.attack_range += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.attack_range -= saveValue;
		}), nullptr));
		break;
	}
	case SKILL_HELP_TYPE::MOVESPEED:
	{
		saveValue = 1.0f* _saveMainStatusData.move_speed *(value - 1.0f) + pureValue;
		_mainCharacterData.move_speed += saveValue;
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, saveValue](Ref *pSEnder){
			_mainCharacterData.move_speed -= saveValue;
		}), nullptr));

		/////////////////////////////////////////
		///////////RUN EFFECT HELP MOVE SPEED
		createSorceryEffect(testObject, SORCERY_BLUE);

		Effect* effectHelpMoveSpeed = Effect::createWithParticle(PARTICLE_MOVESPEED, effect->EC_Other, effect->Help);
		testObject->addChild(effectHelpMoveSpeed);
		effectHelpMoveSpeed->setPosition(Vec2(testObject->getContentSize().width / 2, testObject->getContentSize().height / 2));

		auto helpmMoveSpeedSequence = Sequence::create(
			DelayTime::create(DELAY_HELP)
			, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
			, nullptr);
		effectHelpMoveSpeed->runAction(helpmMoveSpeedSequence);

		break;
	}
	default:
		break;
	}


}

void BatleScene::skillAttackAll(UserSkillInfo skillInfo)
{
	int value = 0;
	switch (skillInfo.correct_type)
	{
	case DAME_TYPE_PERCENT:
		value = _mainCharacterData.attack*100.0f / skillInfo.corrett_value;
		break;
	case DAME_TYPE_PURE:
		value = skillInfo.corrett_value;
	default:
		break;
	}

	vector<int> unitIndex;
	if (skillInfo.range_distance > 0) {

		unitIndex = detectUnitInAoe(skillInfo, ENEMY_FLAG);
	}
	else 
	{
		for (int i = 0; i < _allEnemyUnitData.size(); i++)

		{
			unitIndex.push_back(i);
		}
	}
	for (int &index : unitIndex)
	{
		int dame = (value - _allEnemyUnitData[index].defence);
		float defaultDameRate = caculDameRate(_mainCharacterData.element, _allEnemyUnitData[index].element);
		dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);

		if (dame <= 0) dame = 1;

		if (_allEnemyUnitSprite[index]->isVisible())
		{
			auto action = Spawn::create(Sequence::create(DelayTime::create(2), CallFuncN::create([&, this, index, dame](Ref *p) {
				showAttackDame(dame, _allEnemyUnitSprite[index]->getPosition() + Vec2(0, 100), 1);
				_allEnemyHpBar[index]->setPercent( 100.0f *_allEnemyUnitData[index].hp / _allEnemuUnitMaxHp[index]);
				_allEnemyUnitData[index].hp -= dame;
				saveDameInfo(dame, 0, index, TEAM_FLG_BLUE);
				if (_allEnemyUnitData[index].hp <= 0) {
					enemyDieAction(index);
				}
			}), nullptr), CallFuncN::create([&, this, index](Ref *p) {
				
				///////////////////////////////////////////////////
				//RUN EFFECT ATTACK ONE UNIT
				///////////////////////////////////////////////////

				createSorceryEffect(testObject, SORCERY_RED);
				// Create attack effect
				Effect* attackFire = Effect::createWithParticle(PARTICLE_FIRE, attackFire->EC_Other, attackFire->Help);
				attackFire->setTargetAttack(_allEnemyUnitSprite[index]);
				testObject->getParent()->addChild(attackFire);
				attackFire->setPosition(testObject->getPosition());

				auto attackFireSequence = Sequence::create(
					DelayTime::create(DELAY_HELP)
					, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
					, nullptr);
				attackFire->runAction(attackFireSequence);

			}), nullptr);
			_allEnemyUnitSprite[index]->setTag(index);
			_allEnemyUnitSprite[index]->runAction(action);
		}
	}
}

void BatleScene::skillAttackOne(UserSkillInfo skillInfo)
{
	log("Attack One");

	int value = 0;
	switch (skillInfo.correct_type)
	{
	case DAME_TYPE_PERCENT:
		value = _mainCharacterData.attack*100.0f / skillInfo.corrett_value;
		break;
	case DAME_TYPE_PURE:
		value = skillInfo.corrett_value;
	default:
		break;
	}


	int dame = (value - _allEnemyUnitData[_indexOfBeAttackEnemy].defence);
	float defaultDameRate = caculDameRate(_mainCharacterData.element, _allEnemyUnitData[_indexOfBeAttackEnemy].element);

	dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);
	if (dame <= 0) dame = 1;

	int indexValue = _indexOfBeAttackEnemy;


	auto action = Spawn::create(
		Sequence::create(
		//////Start Sequence
		DelayTime::create(5),  //DELAY_ATTACK_MOVE + DELAY_ATTACK_FIRE_LIFE
		CallFuncN::create([&, this, dame, indexValue](Ref *p) {
		showAttackDame(dame, _allEnemyUnitSprite[indexValue]->getPosition() + Vec2(0, 100), 1);
		_allEnemyHpBar[indexValue]->setPercent(100.0f *_allEnemyUnitData[indexValue].hp / _allEnemuUnitMaxHp[indexValue]);
		_allEnemyUnitData[indexValue].hp -= dame;
		saveDameInfo(dame, 0, indexValue, TEAM_FLG_BLUE);
		if (_allEnemyUnitData[indexValue].hp <= 0)
		{
			enemyDieAction(indexValue);
			return;
		}
		
		}), nullptr), 
		
		////////End Sequence
		CallFuncN::create([&, this, indexValue](Ref *p) {
	///////////////////////////////////////////////////
	//RUN EFFECT ATTACK ONE UNIT
	///////////////////////////////////////////////////

	createSorceryEffect(testObject, SORCERY_ORANGE);
	// Create attack effect
	Effect* attackFire = Effect::createWithParticle(PARTICLE_FIRE, attackFire->EC_Other, attackFire->Help);
	attackFire->setTargetAttack(_allEnemyUnitSprite[indexValue]);
	testObject->getParent()->addChild(attackFire);
	attackFire->setPosition(testObject->getPosition());

	// Effect life in 1s , remove Effect
	auto attackFireSequence = Sequence::create(
		DelayTime::create(DELAY_ATTACK_FIRE_LIFE)
		, CallFuncN::create(CC_CALLBACK_1(BatleScene::removeEffect, this))
		, nullptr);
	attackFire->runAction(attackFireSequence);

	}), nullptr); 

	_allEnemyUnitSprite[_indexOfBeAttackEnemy]->runAction(action);

}


vector<int> BatleScene::detectUnitInAoe(UserSkillInfo skill, int unitFlg, bool drawFlg /*= true*/)

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
	auto pos = testObject->getPosition();
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
		vec.push_back(Vec2(-100, -100));
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
	if (drawFlg) {
		_battleBackround->addChild(draw);
	}

	for (int i = 0; i < allUnit.size(); i++)
	{
		//rectangle
		Rect a;
		Vec2 distan = allUnit[i]->getPosition() - pos;
		switch (skill.range_type)
		{
		case SKILL_RANGE_TYPE::RECTANGLE:
			a = Rect(pos.x - skill.range_distance / 2, pos.y - skill.range_distance / 2, skill.range_distance, skill.range_distance);
			if (a.containsPoint(allUnit[i]->getPosition())) {
				resultUnitId.push_back(i);
			}
			break;
		case SKILL_RANGE_TYPE::TRIANGLE:
			if (detectPointInTriangle(allUnit[i]->getPosition(), vec)) {
				resultUnitId.push_back(i);
			}
			break;
		case SKILL_RANGE_TYPE::STAR:
			if (detectPointInTriangle(allUnit[i]->getPosition(), {vec[0],vec[1],vec[7]})) {
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(allUnit[i]->getPosition(), {vec[1],vec[2],vec[3]}))
			{
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(allUnit[i]->getPosition(), { vec[3], vec[4], vec[5] })) {
				resultUnitId.push_back(i);
				break;
			}
			if (detectPointInTriangle(allUnit[i]->getPosition(), { vec[7], vec[5], vec[6] })) {
				resultUnitId.push_back(i);
				break;
			}
			if (distan.length() < (skill.range_distance / 6 * sqrt(2))) {
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

void BatleScene::updateHpAndMpViewLabel()
{
	string hp = DataUtils::numberToString(_mainCharacterData.hp).append("/").append(DataUtils::numberToString(_allAlliedUnitMaxHp[0]));
	_hpViewLabel->setString(hp.c_str());
	string mp = DataUtils::numberToString(_mainCharacterData.mp).append("/").append(DataUtils::numberToString(_mainCharacerMaxMp));
	_mpViewlabel->setString(mp.c_str());
}

void BatleScene::endBattle()
{
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene(_blueTeamInfo,_redTeamInfo)));
}

float BatleScene::caculDameRate(int mainC, int enemy)
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

void BatleScene::longPressAction(Button *pSender,UserSkillInfo skill)
{
	auto a = detectUnitInAoe(skill, ENEMY_FLAG);
	_onSelectSkillData = skill;
	for (auto& enemy : a)
	{
		_allEnemyUnitSprite[enemy]->setColor(Color3B::RED);
	}
}


bool BatleScene::detectPointInTriangle(Vec2 point, vector<Vec2> points)
{
	Vec2 v0 = makePoint(points[1]+testObject->getPosition(), points[0]+testObject->getPosition());
	Vec2 v1 = makePoint(points[2]+testObject->getPosition(), points[0]+testObject->getPosition());
	Vec2 v2 = makePoint(point, points[0]+testObject->getPosition());

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

float BatleScene::makeDot(Vec2 v1, Vec2 v2)
{
	return v1.x*v2.x + v1.y * v2.y;
}

Vec2 BatleScene::makePoint(Vec2 v1, Vec2 v2)
{
	return Vec2(v1.x - v2.x, v1.y - v2.y);
}

BatleScene::BatleScene() :
	_oldSecond(0),
	_indexOfBeAttackEnemy(-1),
	_allAlliedUnitData({}),
	_allAlliedUnitSprite({}),
	_allAlliedUnitHpBar({})
{

}

BatleScene::~BatleScene()
{
	Director::getInstance()->getScheduler()->unschedule(schedule_selector(BatleScene::update), this);
}

void BatleScene::saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg)
{
	switch (teamFlg)
	{
	case TEAM_FLG_BLUE:
		_blueTeamInfo[attackUnitId].totalDealDame += dame;
		if (beAttackUnitId < ENEMY_NUM)
		{
			_redTeamInfo[beAttackUnitId].totalReceivedDame += dame;
		}	
		break;
	case TEAM_FLG_RED:
		_blueTeamInfo[beAttackUnitId].totalReceivedDame += dame;
		if (attackUnitId < ENEMY_NUM)
		{
			_redTeamInfo[attackUnitId].totalDealDame += dame;
		}
		
		break;
	default:
		break;
	}
}

void BatleScene::saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg)
{
	if (teamFlg == _currentPlayerTeamFlg) {
		switch (_currentPlayerTeamFlg)
		{
		case TEAM_FLG_BLUE:
			_blueTeamInfo[killerId].killNum++;
			if(deadUnitId < ENEMY_NUM) _redTeamInfo[deadUnitId].deadNum++;
			break;
		case TEAM_FLG_RED:
			_redTeamInfo[killerId].killNum++;
			if(deadUnitId < ENEMY_NUM)_blueTeamInfo[deadUnitId].deadNum++;
			break;
		default:
			break;
		}
	}
	else {
		if (teamFlg == _currentEnemyTeamFlg)
		{
			switch (_currentPlayerTeamFlg)
			{
			case TEAM_FLG_BLUE:
				_blueTeamInfo[deadUnitId].deadNum++;
				if(killerId < ENEMY_NUM) _redTeamInfo[killerId].killNum++;

				break;
			case TEAM_FLG_RED:
				_redTeamInfo[deadUnitId].deadNum++;
				if(killerId < ENEMY_NUM)_blueTeamInfo[killerId].killNum++;
				break;
			default:
				break;
			}
		}
	}
}
/*status type: POISON, STUN, BUFF_ATTACK, BUFF_DEFENCE, DEBUFF_ATTACK*/
void BatleScene::displayUnitStatus(Sprite *parent, int statusType, UserSkillInfo skillInfo, int spIndex /*= 0*/)
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
	if (parent != testObject)
	{
// 		auto animate = Animate::create(createStatusAnimation(imagePath));
// 		auto sprite = Sprite::create("test.png");
// 		sprite->setTag(BUFF_STATUS_TAG);
// 		sprite->setPosition(Vec2(parent->getContentSize().width / 2, parent->getContentSize().height));
// 		sprite->setScale(1 / IMAGE_SCALE);
// 		sprite->runAction(Sequence::create(Repeat::create(animate, ceil(skillInfo.duration / animate->getDuration())), RemoveSelf::create(true), nullptr));
// 		parent->addChild(sprite, -1);
		pushStatusImagePath(imagePath, _enemyStatusImagePath[spIndex]);
		parent->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, imagePath,spIndex](Ref* p){
			removeStatusImagePath(imagePath, _enemyStatusImagePath[spIndex]);
			displayStatusInTime((Sprite*)p, _enemyStatusImagePath[spIndex]);
		}), nullptr));
		displayStatusInTime(parent, _enemyStatusImagePath[spIndex]);
	}
	else
	{
		pushStatusImagePath(imagePath, _skillStatusImageList);
		parent->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&, imagePath](Ref* p){
			removeStatusImagePath(imagePath, _skillStatusImageList);
			displayStatusInTime((Sprite*)p, _skillStatusImageList);
		}), nullptr));
		displayStatusInTime(parent, _skillStatusImageList);
	}


}

Animation* BatleScene::createStatusAnimation(string imagePath)
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

void BatleScene::skillPoisonAction(UserSkillInfo skillInfo)
{
	vector<int> units = detectUnitInAoe(skillInfo, ENEMY_FLAG);
	for (auto & index :units)
	{
		displayUnitStatus(_allEnemyUnitSprite[index], POISON, skillInfo,index);
		poisonEffectAction(skillInfo, index);
	}
}

void BatleScene::skillStunAction(UserSkillInfo skillInfo)
{
	vector<int> units = detectUnitInAoe(skillInfo,ENEMY_FLAG);
	for (auto& i : units)
	{
		displayUnitStatus(_allEnemyUnitSprite[i], STUN, skillInfo,i);
		_allEnemyUnitData[i].isStun = true;
		_allEnemyUnitSprite[i]->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&,i](Ref *p){
			_allEnemyUnitData[i].isStun = false;
		}), nullptr));
	}
}

void BatleScene::poisonEffectAction(UserSkillInfo skill, int index)
{
	int dame =ceil(1.0f*_allEnemyUnitData[index].hp * 0.05f);
	auto action = Sequence::create( CallFuncN::create([&, index, dame](Ref *p){
		showAttackDame(dame, _allEnemyUnitSprite[index]->getPosition(), 1);
		_allEnemyUnitData[index].hp -= dame;
		_allEnemyHpBar[index]->setPercent(ceil(100.0f*_allEnemyUnitData[index].hp / _allEnemuUnitMaxHp[index]));
		if (_allEnemyUnitData[index].hp < 0)
		{
			enemyDieAction(index);
		}
	}), DelayTime::create(POISON_STEP_TIME), nullptr);

	_allEnemyUnitSprite[index]->runAction(Repeat::create(action, ceil(skill.duration / POISON_STEP_TIME)));
}

void BatleScene::enemyUnitAutoMoveTest()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (!_allEnemyUnitSprite[i]->isVisible())
		{
			continue;
		}
		auto body = _allEnemyUnitSprite[i]->getPhysicsBody();
		if (_allEnemyUnitData[i].isStun)
		{
			body->setVelocity(Vect::ZERO);
			_allEnemyUnitSprite[i]->stopActionByTag(_allEnemyUnitSprite[i]->getTag());
			continue;
		}
		float angle = random(-90.0f,270.0f);
		body->setVelocity(Vect(_allEnemyUnitData[i].move_speed*cos(angle), _allEnemyUnitData[i].move_speed*sin(angle)));
		int direc = detectDirectionBaseOnTouchAngle(angle);
		if (direc != 0) actionCharacterCopy(direc,_allEnemyUnitSprite[i]);
	}
}

void BatleScene::actionCharacterCopy(int directionId, Sprite *sprite)
{
	if (sprite->getNumberOfRunningActions() > 0) {
		if (sprite->getActionByTag(directionId) != nullptr) {
			//log("!!!");
			return;
		}
	}
	sprite->stopActionByTag(sprite->getTag());
	auto action = Animate::create(createMoveAnimationWithDefine(directionId, "image/unit_new/move/red/"));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	sprite->setTag(directionId);
	sprite->runAction(repeat);
}

void BatleScene::fountainRestoreEffect()
{
	if (testObject->getActionByTag(FOUNTAIN_ACTION) != nullptr)
	{
		return;
	}
	auto action = RepeatForever::create(Sequence::create(CallFuncN::create([&](Ref *p){
		_mainCharacterData.mp += ceil(_saveMainStatusData.mp *0.05f);
		if (_mainCharacterData.mp > _mainCharacerMaxMp)
		{
			_mainCharacterData.mp = _mainCharacerMaxMp;
		}
		_mainCharacterMpBar->setPercent(ceil(100.0f*_mainCharacterData.mp / _saveMainStatusData.mp));
		_mainCharacterData.hp += ceil(_saveMainStatusData.hp * 0.05f);
		if (_mainCharacterData.hp > _allAlliedUnitMaxHp[0]) {
			_mainCharacterData.hp = _allAlliedUnitMaxHp[0];
		}
		//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
		_allAlliedUnitHpBar[0]->setPercent(ceil(100.0f*_mainCharacterData.hp / _saveMainStatusData.hp));
		_mainCharacterHpBar->setPercent(ceil(100.0f*_mainCharacterData.hp / _saveMainStatusData.hp));
		updateSlider();
	}), DelayTime::create(1), nullptr));
	action->setTag(FOUNTAIN_ACTION);
	testObject->runAction(action);
}

void BatleScene::enemyRespawAction(int index)
{
	auto action = (Sequence::create(DelayTime::create(5), CallFuncN::create([&, index](Ref *pSEnder){
		_allEnemyUnitSprite[index]->setPosition(Vec2(_visibleSize.width + (index - 1) * 70, _visibleSize.height * 2 - 100));
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
void BatleScene::createSorceryEffect(Sprite* spriteUnit, std::string eclipseFilePath)
{
	auto nodeFather = Sprite::create();

	spriteUnit->addChild(nodeFather, -10);
	nodeFather->setPosition(Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2 - spriteUnit->getContentSize().height / 4
		));

	// Create Eclipse
	auto sorcery = Sprite::create(eclipseFilePath);
	sorcery->setScale(1.5f);
	sorcery->setPosition(Vec2(0, 0));

	nodeFather->addChild(sorcery, -10);

	auto actionRotate = RotateBy::create(1.0f, Vec3(0, 0, 150));
	auto repeat = RepeatForever::create(actionRotate);
	sorcery->runAction(repeat);

	nodeFather->setScaleY(0.5f);


	//removeSorceryEclipse
	auto sequence = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE),
		CallFuncN::create(CC_CALLBACK_1(BatleScene::removeSorceryEclipse, this)),
		NULL
		);

	nodeFather->runAction(sequence);
}

void BatleScene::removeSorceryEclipse(Ref* pSender)
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


void BatleScene::removeEffect(Ref* pSender)
{

	auto effect = dynamic_cast<Sprite*>(pSender);

	if (effect != nullptr)
	{
		effect->removeFromParentAndCleanup(true);
		//log("Remove Effect");
	}
	else
	{
		//log("Remove Effect");
		return;
	}
}

void BatleScene::pushStatusImagePath(string imagepath, vector<string> &allImages)
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

void BatleScene::removeStatusImagePath(string imagepath, vector<string> &allImages)
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

void BatleScene::displayStatusInTime(Sprite* parent, vector<string> allImages)
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
	sprite->setPosition(Vec2(parent->getContentSize().width / 2, parent->getContentSize().height));
	sprite->setScale(1 / IMAGE_SCALE);
	sprite->runAction(RepeatForever::create(animate));
	parent->addChild(sprite, -1);
}

int BatleScene::findIndexOfString(vector<string> v, string element)
{
	for (int i = 0; i < v.size(); i++)
	{
		if (v[i] == element) {
			return i;
		}
	}
	return 0;
}

cocos2d::Vec2 BatleScene::getTitlePosForPosition(Vec2 location)
{
	int x = location.x / _myMap->getTileSize().width;
	int y = ((_myMap->getMapSize().height*_myMap->getTileSize().height - location.y) / _myMap->getTileSize().height);
	return Vec2(x, y);
}
cocos2d::Vec2 BatleScene::getPositionForTitleCoord(Vec2 tileCoord)
{
	int x = (tileCoord.x * _myMap->getTileSize().width) + _myMap->getTileSize().width / 2;
	int y = (_myMap->getMapSize().height * _myMap->getTileSize().height) -
		(tileCoord.y * _myMap->getTileSize().height) - _myMap->getTileSize().height / 2;
	return Vec2(x, y);
}

vector<Vec2> BatleScene::AStarPathFindingAlgorithm(Vec2 curentPos, Vec2 destinationPos)
{
	vector<Vec2> result = {};
	
	Vec2 fromTitleCoord = getTitlePosForPosition(curentPos);
	Vec2 desTitleCoord = getTitlePosForPosition(destinationPos);
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
	schedule(schedule_selector(BatleScene::countTime));
	insertInOpenSteps(ShortestPathStep::createWithPosition(fromTitleCoord));

	do 
	{
		ShortestPathStep *curStep = _spOpenSteps.at(0);

		_spClosedSteps.pushBack(curStep);

		_spOpenSteps.erase(0);

		if (curStep->getPosition() == desTitleCoord) {
			unschedule(schedule_selector(BatleScene::countTime));
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

void BatleScene::insertInOpenSteps(ShortestPathStep *step)
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

int BatleScene::computeHScoreFromCoordToCoord(const Vec2 &fromCoord, const Vec2 &toCoord)
{
	return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}

int BatleScene::costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep)
{
	return ((fromStep->getPosition().x != toStep->getPosition().x)&& (fromStep->getPosition().y != toStep->getPosition().y)) ? 14 : 10;
}

ssize_t BatleScene::getStepIndex(const Vector<ShortestPathStep*> &steps, const ShortestPathStep *step)
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

bool BatleScene::isValidTileCoord(Vec2 &tileCoord)
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
bool BatleScene::isWallAtTileCoord(Vec2 &titleCoord)
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

PointArray * BatleScene::allWalkableTitlesCoordForTitleCoord(Vec2 tileCoord)
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

void BatleScene::constructPathAndStartAnimationFromStep(ShortestPathStep *step)
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

void BatleScene::moveStepAction()
{
	//fakeZOrder();
	Vec2 curPos = getTitlePosForPosition(testObject->getPosition());
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
	float time = distanVector.length() / _mainCharacterData.move_speed;
	MoveTo *moveAction = MoveTo::create(time, desPos );
	_mainCharacterIconInMiniMap->setRotation(-(distanVector.getAngle() * RAD_DEG) + 90);

	//log("%f", _mini_Icon->getRotation());
	int direc = detectDirectionBaseOnTouchAngle(_mainCharacterIconInMiniMap->getRotation());
	if (direc != 0) actionCharacter(direc, testObject);

	CallFunc *moveCallback = CallFunc::create(CC_CALLBACK_0(BatleScene::moveStepAction, this));

	_shortestPath.erase(0);

	Sequence *moveSequence = Sequence::create(moveAction, moveCallback, nullptr);
	moveSequence->setTag(AUTO_MOVE_ACTION_TAG);
	testObject->runAction(moveSequence);
}

void BatleScene::countTime(float dt)
{
	log("%f", dt);
	calCulTime += dt;
}


BatleScene::ShortestPathStep::ShortestPathStep():
_position(Vec2::ZERO),
_gScore(0),
_hScore(0),
_parent(nullptr)
{

}

BatleScene::ShortestPathStep::~ShortestPathStep()
{

}

BatleScene::ShortestPathStep *BatleScene::ShortestPathStep::createWithPosition(const Vec2 pos)
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

bool BatleScene::ShortestPathStep::initWithPosition(const Vec2 pos)
{
	setPosition(pos);
	return true;
}

int BatleScene::ShortestPathStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool BatleScene::ShortestPathStep::isEqual(const ShortestPathStep *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string BatleScene::ShortestPathStep::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}
