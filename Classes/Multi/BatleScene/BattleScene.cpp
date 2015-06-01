#include "BattleScene.h"


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

	_menu->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_selectedUnitId = UserModel::getInstance()->getSelectedUnitId();
	setUnitStatus(1);
	for (auto &sk : BattleModel::getInstance()->getPlayerSkills())
	{
		_playerSkills.push_back(UserSkillModel::getInstance()->getSkillInfoById(sk));
	}
	///INIT DATA FOR ALL UNIT IN BATTLE
	_mainCharacterData = getUnitDataFromDataBase(_selectedUnitId);
	_mainCharacterData.uuid = UserModel::getInstance()->getUuId();

	_mainCharacterSkillData = getUnitSkillFromDataBase(_mainCharacterData);

	_saveMainStatusData = _mainCharacterData;
	_mainCharacerMaxMp = _mainCharacterData.mp;

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

	log("Enemy Unit vector Size: %d", a.size());

	_allEnemyUnitData = getUnitsDataListByUnitIdIdList(a);

	//first element of allied unit list is the main character info
	_allAlliedUnitData.push_back(_mainCharacterData);

	for (auto &alliedUnit : getUnitsDataListByUnitIdIdList(b))
	{
		_allAlliedUnitData.push_back(alliedUnit);
	}

	//For test
	_redTeamTowerData = UserUnitModel::getInstance()->getUnitInfoById(1);
	_redTeamTowerData.hp = 5000;
	_redTeamTowerData.attack_range = 200;
	_redTeamTowerData.attack_speed = 5;
	_redTeamTowerData.uuid = "redTower";

	_blueTeamTowerData = UserUnitModel::getInstance()->getUnitInfoById(2);

	_blueTeamTowerData.hp = 5000;
	_blueTeamTowerData.attack_range = 200;
	_blueTeamTowerData.attack_speed = 5;
	_blueTeamTowerData.uuid = "blueTower";

	//////////
	//_moveImagePath = "image/unit_new/move/red/";
	//_attackImagePath = "image/unit_new/attack/red/";
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

	auto changeImage = Button::create();
	changeImage->loadTextureNormal("CloseNormal.png");
	changeImage->setPosition(Vec2(_visibleSize.width - 150, 70));
	changeImage->setTouchEnabled(true);
	changeImage->addTouchEventListener(CC_CALLBACK_2(BattleScene::changeImageButtonCallback, this));
	addChild(changeImage, 10);

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
			log("type: %d , property: %d", _circleType, _circleProperty);

			log("haha");
			if (_circleType != 0 && _circleProperty != 0) {
				// Check tu lan thu 2 tro di
				// Lan dau tien vao thi ham check se la false, nen no se cho khoi tao 1 controlunit
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

	if (_currentPlayerTeamFlg == TEAM_FLG_BLUE) {
		UserBattleInfo info;
		info.name = _mainCharacterData.name;
		info.unitId = _mainCharacterData.mst_unit_id;
		info.imagePath = UserUnitModel::getInstance()->getUnitImageById(_mainCharacterData.mst_unit_id);
		_blueTeamInfo.push_back(info);
		for (int i = 0; i < _allEnemyUnitData.size(); i++)
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
		for (int i = 0; i < _allEnemyUnitData.size(); i++)
		{
			UserBattleInfo enemy;
			enemy.name = _allEnemyUnitData[i].name;
			enemy.unitId = _allEnemyUnitData[i].mst_unit_id;
			_blueTeamInfo.push_back(enemy);
		}
	}
	scheduleUpdate();
	return true;
}
void BattleScene::createContent()
{
	//create e battle background
	_battleBackround = Node::create();
	_battleBackround->setPosition(Vec2::ZERO);
	_battleBackround->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
	addChild(_battleBackround);
	//Load the MTC map
	string pathFIle = FileUtils::getInstance()->fullPathForFilename("map/map3.tmx");
	_myMap = TMXTiledMap::create(pathFIle.c_str());

	if (_myMap != nullptr)
	{
		_battleBackround->addChild(_myMap);
		Size CC_UNUSED s = _myMap->getContentSize();
		//log("ContentSize: %f, %f", s.width, s.height);
		Vec2 ts = _myMap->getTileSize();
		//log("Title Size: %f, %f", ts.x, ts.y);
		Vec2 ts2 = _myMap->getMapSize();
		//log("map Size: %f, %f", ts2.x, ts2.y);
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
	redTeamAreaNode->setPosition(Vec2(_visibleSize.width, 2 * _visibleSize.height - 75));
	_battleBackround->addChild(redTeamAreaNode);

	//red Tower
	auto redTower = Sprite::create("tower_red.png");
	redTower->setTag(TOWER_TAG);

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
	blueTower->setTag(TOWER_TAG);
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
	auto uuId = UserModel::getInstance()->getUuId();
	auto unitInfo = BattleModel::getInstance()->getUnitInforByUuid(uuId.c_str());
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


	testObject = Character::createCharacter(_selectedUnitId + 1);
	testObject->setTag(10);
	testObject->setPosition(_visibleSize);
	_battleBackround->addChild(testObject, MID);
	//testObject->setPosition(Vec2(_visibleSize.width, 100));
	testObject->setPosition(Vec2(unitInfo.position_x, unitInfo.position_y));
	testObject->rotateCharacter(unitInfo.direction);

	testObject->setScale(IMAGE_SCALE);
	testObject->setPhysicsBody(PhysicsBody::createCircle(30, PhysicsMaterial(1, 0, 1)));
	testObject->getPhysicsBody()->setRotationEnable(false);
	testObject->getPhysicsBody()->setContactTestBitmask(0x1);
	testObject->getPhysicsBody()->setCategoryBitmask(cTeamContactCategory);
	testObject->getPhysicsBody()->setCollisionBitmask(cTeamContactCollision);
	testObject->setCharacterMoveSpeed(_mainCharacterData.move_speed);


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

	auto icon = Sprite::create(UserUnitModel::getInstance()->getUnitImageById(_mainCharacterData.mst_unit_id));
	icon->setPosition(slotPos + Vec2(0,0));
	icon->setScale(1.4f);
	icon->setTag(ICON);
	_characterImageViewNode->addChild(icon);
	topMenu->addChild(_characterImageViewNode);

	_menuButton = Button::create();
	_menuButton->loadTextureNormal("image/screen/battle/menu_btn.png");
	_menuButton->addTouchEventListener(CC_CALLBACK_2(BattleScene::menuButtonCallback, this));
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
	_mainCharacterIconInMiniMap->setRotation(-(unitInfo.angle) + 90);




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
	_skill1Button->setTag(TAG_SKILL_1);
	_skill1Button->setSwallowTouches(false);
	_skill1Button->setPosition(Vec2(_visibleSize.width / 2 - 1.5 * baseSize.width - 20, baseSize.height / 2 + baseMargin));
	addChild(_skill1Button);
	displaySkillMpInButton(_skill1Button, _mainCharacterSkillData[0].mp_cost);

	_skill2Button = Button::create();
	_skill2Button->loadTextureNormal(skill2ImagePath.c_str());
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill2Button->setTag(TAG_SKILL_2);
	_skill2Button->setSwallowTouches(false);
	_skill2Button->setPosition(Vec2(_visibleSize.width / 2 - 0.5 *baseSize.width - 10, baseSize.height / 2 + baseMargin));
	addChild(_skill2Button);
	displaySkillMpInButton(_skill2Button, _mainCharacterSkillData[1].mp_cost);

	_skill3Button = Button::create();
	_skill3Button->loadTextureNormal(skill3ImagePath.c_str());
	_skill3Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill3Button->setTag(TAG_SKILL_3);
	_skill3Button->setSwallowTouches(false);
	_skill3Button->setPosition(Vec2(_visibleSize.width / 2 + 0.5*baseSize.width + 10, baseSize.height / 2 + baseMargin));
	addChild(_skill3Button);
	displaySkillMpInButton(_skill3Button, _playerSkills[0].mp_cost);

	_skill4Button = Button::create();
	_skill4Button->loadTextureNormal(skill4ImagePath.c_str());
	_skill4Button->addTouchEventListener(CC_CALLBACK_2(BattleScene::skill1ButtonCallback, this));
	_skill4Button->setTag(TAG_SKILL_4);
	_skill4Button->setSwallowTouches(false);
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
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		auto battleUnitInfo = BattleModel::getInstance()->getUnitInforByUuid(_allEnemyUnitData[i].uuid.c_str());
		auto sp = Character::createCharacter(_allEnemyUnitData[i].mst_unit_id +1);
		sp->rotateCharacter(battleUnitInfo.direction);
		sp->setPosition(Vec2(battleUnitInfo.position_x +(i-1) * 70, battleUnitInfo.position_y));
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

	for (int i = 1; i < _allAlliedUnitData.size(); i++)
	{
		auto battleUnitInfo1 = BattleModel::getInstance()->getUnitInforByUuid(_allAlliedUnitData[i].uuid.c_str());
		auto sp1 = Character::createCharacter(_allAlliedUnitData[i].mst_unit_id + 1);
		sp1->rotateCharacter(battleUnitInfo1.direction);
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
		hpB1->setPosition(Vec2(sp1->getContentSize().width / 2, sp1->getContentSize().height - 10));
		_allEnemyHpBar.push_back(hpB1);
		sp1->addChild(_allEnemyHpBar.back(), 100);
		_allAlliedUnitSprite.push_back(sp1);
		_battleBackround->addChild(_allAlliedUnitSprite.back(), MID);

		auto enemyIcon1 = Sprite::create("image/screen/battle/enemyicon.png");
		enemyIcon1->setOpacity(255);
		enemyIcon1->setColor(Color3B::RED);
		enemyIcon1->setPosition(Vec2(sp1->getPositionX()*positionXScaleRate, sp1->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(enemyIcon1);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);
		_allEnemyAttachDelay.push_back(false);

		vector<string> tmp1 = {};
		_alliedStatusImagePath.push_back(tmp1);
	}
	/*_testAttackTarget = Sprite::create("image/unit_new/move/red/unit_00_02_2.png");
	_testAttackTarget->setPosition(_visibleSize);
	_testAttackTarget->setScale(IMAGE_SCALE);
	_battleBackround->addChild(_testAttackTarget);*/

	auto enemyIcon1 = Sprite::create("image/screen/battle/enemyicon.png");
	enemyIcon1->setOpacity(255);

	auto alliedIcon = Sprite::create("image/screen/battle/enemyicon.png");
	alliedIcon->setOpacity(255);
	alliedIcon->setColor(Color3B::RED);
	vector<string> tmp1 = {};

	switch (_currentPlayerTeamFlg)
	{
	case TEAM_FLG_RED:
		_allAlliedUnitSprite.push_back(redTower);
		_allAlliedUnitHpBar.push_back(redTHpBar);
		_allAlliedUnitData.push_back(_redTeamTowerData);
		alliedIcon->setPosition(Vec2(redTower->getPositionX()*positionXScaleRate, redTower->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(alliedIcon);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);


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
		alliedIcon->setPosition(Vec2(blueTower->getPositionX()*positionXScaleRate, blueTower->getPositionY()*positionYScaleRate));
		_allAlliedUnitIconInMiniMap.push_back(alliedIcon);
		node->addChild(_allAlliedUnitIconInMiniMap.back(), 1);


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

	//TODO HERE IS THE CODE TO TEST BATTLE MOVE SYNC FUNCTION

	auto sv = NodeServer::getInstance()->getClient();

	auto testSprite = Sprite::create("ball.png");
	_battleBackround->addChild(testSprite, 100);
	testSprite->setPhysicsBody(PhysicsBody::createCircle(20, PhysicsMaterial(1, 1, 1)));

	/*auto testCharacter = Character::createCharacter(2);
	_battleBackround->addChild(testCharacter, 100);
	testCharacter->setPhysicsBody(PhysicsBody::createCircle(20, PhysicsMaterial(1, 1, 1)));
	testCharacter->setScale(IMAGE_SCALE);
	testCharacter->getPhysicsBody()->setRotationEnable(false);
	testCharacter->getPhysicsBody()->setContactTestBitmask(0x1);
	testCharacter->getPhysicsBody()->setCategoryBitmask(ALLIED_CONTACT_CATEGORY_BITMAP);
	testCharacter->getPhysicsBody()->setCollisionBitmask(ALLIED_CONTACT_COLLISION_BITMAP);
	sv->on("move_sync_end", [&, testSprite, testCharacter](SIOClient* client, const std::string data){
		log("SYNC end data :%s", data.c_str());
		auto info = UserModel::getInstance()->getUserInfo();
		Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("Parse JSOn error");
			return;
		}
		if (doc.IsObject()) {
			//need same data when battle start for save player and unit data for future logic( attack, move, stop move, skill, dead, respawn)
			for (int i = 0; i < doc["user_unit"].Size(); i++)
			{
				//log("position: %f %f", doc["user_unit"][rapidjson::SizeType(i)]["position_x"].GetDouble(), doc["user_unit"][rapidjson::SizeType(i)]["position_y"].GetDouble());
				if (doc["user_unit"][rapidjson::SizeType(i)]["user_id"].GetInt() == info.user_id) {
					log("this is my unit");
					continue;
				}
				else {
					log("Angle: %f", doc["angle"].GetDouble());
					log("Direc: %d", doc["direction"].GetInt());
					int direc = doc["direction"].GetInt();
					float angle = doc["angle"].GetDouble();
					if (doc["user_unit"][rapidjson::SizeType(i)]["user_id"].GetInt() == 10) continue;
					testCharacter->changeAnimationImagePathByUnitId(doc["user_unit"][rapidjson::SizeType(i)]["mst_unit_id"].GetInt() + 1);
					testCharacter->setPosition(Vec2(doc["user_unit"][rapidjson::SizeType(i)]["position_x"].GetDouble(), doc["user_unit"][rapidjson::SizeType(i)]["position_y"].GetDouble()));
					//TODO need to check this unit is moving or not --> Need event call from client
					//testCharacter->rotateCharacter(direc);
					//
					if (doc["moving"].GetBool())
					{
						testCharacter->actionMoveCharacter(direc);
					}
					else {
						testCharacter->stopMoveAction();
					}
					//testSprite->setPosition(Vec2(doc["user_unit"][rapidjson::SizeType(i)]["position_x"].GetDouble(), doc["user_unit"][rapidjson::SizeType(i)]["position_y"].GetDouble()));
					//testSprite->setRotation(doc["angle"].GetDouble());
					auto uinfo = UserUnitModel::getInstance()->getUnitInfoById(doc["user_unit"][rapidjson::SizeType(i)]["mst_unit_id"].GetInt());
					testCharacter->getPhysicsBody()->setVelocity(Vec2::ZERO);
					
					//testSprite->setRotation(angle);
					testCharacter->getPhysicsBody()->applyImpulse(Vec2(uinfo.move_speed * cos(angle), uinfo.move_speed * sin(angle)));
					return;
				}
			}
		}

	});
	*/
	sv->on("battle_update", [&](SIOClient* client, const std::string data){
		log("BATTLE UPDATE data: %s", data.c_str());
		//same data with battle sync but change for detect unit by uuid
		auto uuid = UserModel::getInstance()->getUuId();
		Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("Parse JSOn error");
			return;
		}
		if (doc.IsObject()) {
			//Update position of all unit
			//sync HP, MP...
			//Update all data of Room_User_Unit_model in Battle Model
			for (int i = 0; i < doc["user_unit"].Size(); i++) {
				log("!EEEE");
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
				/*//bellow code for sync HP and MP of all Unit

				for (auto &unit1  : _allAlliedUnitData)
				{
					if (strcmp(unit1.uuid.c_str(), tempUnit.uuid.c_str()) == 0) {
						unit1.hp = tempUnit.hp;
						unit1.mp = tempUnit.mp;
						//attack, defence, speed....

					}

				}
				for (auto &ene : _allEnemyUnitData)
				{
					if (strcmp(ene.uuid.c_str(), tempUnit.uuid.c_str()) == 0) {
						ene.hp = tempUnit.hp;
						ene.mp = tempUnit.mp;
					}
				}*/


				string sv_uuid = doc["user_unit"][rapidjson::SizeType(i)]["uuid"].GetString();
				if (strcmp(sv_uuid.c_str(), uuid.c_str()) == 0) {
					log("this is my unit");
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
						}
					}
				}


			}

		}
		else {
			log("Error in JSON Object. Type: %d", doc.GetType());
		}


	});

	/*sv->on("battle_public_move", [&, testSprite](SIOClient* client, const std::string& a) {
		//Parser JSON data
		log("public move data: %s", a.c_str());

		rapidjson::Document doc;
		doc.Parse<0>(a.c_str());
		if (doc.HasParseError())
		{
			log("error in parse json");
			return;
		}
		if (doc.IsObject()) {
			testSprite->setPosition(Vec2(doc["position_x"].GetDouble(), doc["position_y"].GetDouble()));
		}
	});*/

	sv->on("unit_move_end", [&](SIOClient* client, const string& data) {
		log("Unit_move_end data: %s", data.c_str());
		//testSprite->getPhysicsBody()->setVelocity(Vec2::ZERO);

	});


	sv->on("attack", [&](SIOClient *client, const string data) {
		log("Another attacks: %s", data.c_str());
		auto uuid = UserModel::getInstance()->getUuId();
		Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("Parse JSOn error");
			return;
		}
		if (doc.IsObject()) {
			int teamId = UserModel::getInstance()->getUserInfo().team_id;
			vector<Sprite*> processUnitSprite = {};
			vector<UserUnitInfo> processUnitData = {};

			vector<Sprite*> targetSpriteList = {};
			vector<UserUnitInfo> targetDataList = {};
			if (doc["team_id"].GetInt() == teamId) {
				processUnitSprite = _allAlliedUnitSprite;
				processUnitData = _allAlliedUnitData;
				targetDataList = _allEnemyUnitData;
				targetSpriteList = _allEnemyUnitSprite;

			}
			else {
				processUnitSprite = _allEnemyUnitSprite;
				processUnitData = _allEnemyUnitData;
				targetDataList = _allAlliedUnitData;
				targetSpriteList = _allAlliedUnitSprite;
			}

			for (int i = 0; i < processUnitData.size(); i++)
			{
				if (strcmp(processUnitData[i].uuid.c_str(), doc["uuid"].GetString()) == 0) {
					log("HERE IS ATTACKER");
					Character* cha = (Character*)processUnitSprite[i];
					//bellow code for detect be attacked unit and sync data:
					Sprite* target;
					for (int j = 0; i < targetDataList.size(); j ++)
					{
						if (strcmp(doc["target"]["uuid"].GetString(), targetDataList[j].uuid.c_str()) == 0) {
							log("Here is target");
							target = targetSpriteList[j];
							break;
						}
					}
					int dame = doc["dame"].GetInt();
					if (dame <= 0) {
						dame = 1;
					}
					log("Dame: %d", dame);
					cha->attackActionByUnitPosition(doc["direction"].GetInt(), doc["user_unit"]["attack_speed"].GetInt()/*, nullptr, [&, target,dame]() {
						
						//show hp lost = final attack dame
						showAttackDame(dame, target->getPosition() + Vec2(0, 100), 1);

						_allEnemyUnitData[i].hp -= dame;
						//save dame deal info to battle result
						saveDameInfo(dame, 0, i, _currentPlayerTeamFlg);
						//check if enemy die
						if (_allEnemyUnitData[i].hp <= 0)
						{
							enemyDieAction(i);
							return;
						}
						//update enemy hp status bar
						_allEnemyHpBar[i]->setPercent(ceil((100.0f * _allEnemyUnitData[i].hp / _allEnemuUnitMaxHp[i])));
					}*/);

					break;
				}
			}



		}

	});
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

Node* BattleScene::createHBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(_visibleSize.width * 2, 100), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);
	
	auto node = Node::create();
	node->setPhysicsBody(wallBd);

	return node;
}

Node* BattleScene::createVBolder()
{
	auto wallBd = PhysicsBody::createBox(Size(100, _visibleSize.height * 2), PhysicsMaterial(1, 0, 0));
	wallBd->setGravityEnable(false);
	wallBd->setDynamic(false);
	wallBd->setContactTestBitmask(0x1);

	auto node = Node::create();
	node->setPhysicsBody(wallBd);

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
	dispath->addEventListenerWithSceneGraphPriority(contactListener, this);
	

}

void BattleScene::update(float delta)
{
	//log("Current Hp: %d", _characterCurentHp);
	_checkTime += delta;
	//TODO: uncomment to send unit status every 0.07 seconds
	if (_checkTime >= 0.07) {
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
		//enemyUnitAutoMoveTest();
	}

	fakeZOrder();

}
void BattleScene::checkForAutoAttack()
{
	if (_onRespwanFlg) return;
	//float area = IMAGE_SCALE*_autoAttackArea->getContentSize().width / 2 + 25;
	//Check for main character attack
	for (int i = 0; i < _allEnemyUnitSprite.size(); i++)
	{
		///Check for main character run attack animation

			auto posDistan = _allEnemyUnitSprite[i]->getPosition() - testObject->getPosition();
			int direc = detectDirectionBaseOnTouchAngle(-posDistan.getAngle()*RAD_DEG + 90);
			if (posDistan.length() - _allEnemyUnitSprite[i]->getBoundingBox().size.width / 4 < ATTACK_AOE*_mainCharacterData.attack_range / 100.0f && _allEnemyUnitSprite[i]->isVisible()) {

				if (/*testObject->getActionByTag(testObject->getCurrentAttackActionTag()) == nullptr && */_onDelayAttackFlg == false) {
					_onDelayAttackFlg = true;
					_mainCharacterIconInMiniMap->setRotation(-(posDistan.getAngle() * RAD_DEG) + 90);

					//auto call1 = CallFuncN::create(CC_CALLBACK_0(BattleScene::characterAttackCallback, this));
				
					BattleAPI::getInstance()->sendAttackEvent(direc,_mainCharacterData, _allEnemyUnitData[i], [&, i, direc](SIOClient *client, const string data) {
						log("Battle attack callback with data: %s", data.c_str());
						Document doc;
						doc.Parse<0>(data.c_str());
						if (doc.HasParseError()) {
							log("ATTACK: Parse JSOn error");
							return;
						}
						testObject->attackActionByUnitPosition(direc, _mainCharacterData.attack_speed, CC_CALLBACK_0(BattleScene::oneSecondAttackCallback,this), CC_CALLBACK_0(BattleScene::characterAttackCallback, this, i, doc["dame"].GetInt()));

						if (_moveMode == MOVE_CIRCLE)
						{
							_miniUnit->attackActionByUnitPosition(direc, _mainCharacterData.attack_speed);
						}

						_indexOfBeAttackEnemy = i;
					});

				}
			}

			//check for enemy unit auto attack
// 			if (posDistan.length() < ATTACK_AOE*_allEnemyUnitData[i].attack_range/100.0f && _allEnemyUnitSprite[i]->isVisible() && !_allEnemyUnitData[i].isStun) {
// 
// 				if (/*_allEnemyUnitSprite[i]->getNumberOfRunningActions() < 1 &&*/ _allEnemyAttachDelay[i] == false) {
// 					_allEnemyUnitSprite[i]->stopAllActionsByTag(_allEnemyUnitSprite[i]->getTag());
// 					string path = "image/unit_new/attack/red/";
// 
// 					auto target_ani = createAttackAnimationWithDefine(10 - direc, path);
// 					auto call2 = CallFuncN::create(CC_CALLBACK_1(BattleScene::enemyAttackCallback, this,i));
// 					auto action2 = Sequence::create(Animate::create(target_ani), call2, nullptr);
// 					_allEnemyAttachDelay[i] = true;
// 					auto forCallback = Sequence::create(DelayTime::create(_allEnemyUnitData[i].attack_speed), CallFuncN::create([&, i](Ref *p) {
// 						if (i < _allEnemyAttachDelay.size()) {
// 							_allEnemyAttachDelay[i] = false;
// 						}
// 					}), nullptr);
// 					action2->setTag(1111);
// 					//rotateCharacter(_alltargetUnit[i], 10 - direc);
// 					_allEnemyUnitSprite[i]->runAction(Spawn::create(action2, forCallback, nullptr));
// 					//_indexOfRunningActionTarget = i;
// 				}
// 			}
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
void BattleScene::characterAttackCallback(int  i, int dame)
{
	//if main character die before atack event end.
	if (_onRespwanFlg) return;
	//log("charater");
	//check for enemy unit still alive
	if (_allEnemyUnitData[i].hp > 0) {
		//default dame = attack -  defence
		/*int dame = (_mainCharacterData.attack - _allEnemyUnitData[i].defence);
		//calculate dame by unit attribute ( fire, water, wind, sand.... or none)
		float defaultDameRate = caculDameRate(_mainCharacterData.element, _allEnemyUnitData[i].element);

		//random dame by rate from 85% to 100%
		dame = ceil(random(0.85f, 1.0f)*dame*defaultDameRate);
		//check if dame < 0 (in case of attack < defence*/
		if (dame <= 0) {
			dame = 1;
		}
		//show hp lost = final attack dame
		showAttackDame(dame, _allEnemyUnitSprite[i]->getPosition() + Vec2(0, 100), 1);

		_allEnemyUnitData[i].hp -= dame;
		//save dame deal info to battle result
		saveDameInfo(dame, 0, i, _currentPlayerTeamFlg);
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
	//_onDelayAttackFlg = false;
}
void BattleScene::oneSecondAttackCallback()
{
	_onDelayAttackFlg = false;
}



void BattleScene::enemyDieAction(int id)
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
void BattleScene::enemyAttackCallback(Ref *pSEnder, int i)
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

void BattleScene::runRespawnAction(int killerId)
{
	if (_onRespwanFlg) return;
	_onRespwanFlg = true;
	//FOR sever fix position
	BattleAPI::getInstance()->sendDeadEvent(_mainCharacterData, [&](SIOClient* client, const std::string json) {

		log("Dead event callback From server with data: %s", json.c_str());

	});

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

			auto unitData = BattleModel::getInstance()->getUnitInforByUuid(UserModel::getInstance()->getUuId());
			testObject->setPosition(Vec2(unitData.position_x, unitData.position_y));
			testObject->rotateCharacter(unitData.direction);
			//testObject->setPosition(Vec2(_visibleSize.width, 100)); //need to change to value base on callback from server
			testObject->setVisible(true);
			auto action3 = Spawn::create(Blink::create(0.6f, 6), Sequence::create(DelayTime::create(RESPAWN_DELAY), CallFuncN::create(CC_CALLBACK_0(BattleScene::removeRespawnFlg, this)), nullptr), nullptr);
			testObject->runAction(action3);
			//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
			//SEND RESPAWN ACTION

		});
		timeLb->runAction(Sequence::create(action, action2, nullptr));

	
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
	_touchStartPoint = touch->getLocation();
	if (_moveDisableFlg == false) {

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
		_selectTargetSprite->setPosition(testObject->getPosition());
		_selectTargetSprite->setVisible(true);
	}

	return true;
}

void BattleScene::onTouchMoved(Touch *touch, Event *unused_event)
{
	// Xac dinh vector giua diem touchMove va touchBegin
	Vec2 distanVector;
	if (_moveMode == MOVE_MANUAL)
	{
		distanVector = touch->getLocation() - _touchStartPoint;
	}

	if (_moveMode == MOVE_CIRCLE)
	{
		distanVector = touch->getLocation() - _miniCircle->getPosition();
	}



	// Xu ly truong hop ngon tay khi cham,se chuyen touchBegin thanh touchMove
	// Test tren Win32 thi se khong gap truong hop nay neu touchOne , nhung neu test tren tanmatsu thi se nhay vao day
	// Nen can check do rong cua ngon tay touc
	if ((touch->getLocation() - _touchStartPoint).length() < 20) {
		_checkOneTapMoveFlg = false;
		return;
	}


	/////////////////////////////////////////////////////////////////////////
	// Xu ly hien thi vong tron di chuyen cho cac truong hop touch and move
	/////////////////////////////////////////////////////////////////////////
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

	/////////////////////////////////////////////////////////////////////////
	// Xu ly logic di chuyen cho cac truong hop touch and move
	/////////////////////////////////////////////////////////////////////////

	// Di vao ben trong vong tron thi se ko di chuyen tiep,su dung cho ca move_manual && move_circle_manual
	if (distanVector.length() < _touchMoveBeginSprite->getContentSize().width / 6 && _moveMode != MOVE_CIRCLE) {
		testObject->stopMoveAction();
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
bool BattleScene::caculAvgAngle(int avg, float angle)
{
	if (angle > avg - 22 && angle < avg + 22) return true;
	return false;
}
void BattleScene::actionCharacter(int directionId, Sprite *characterSprite)
{
	if (characterSprite->getNumberOfRunningActions() > 0) {
		if (characterSprite->getActionByTag(directionId) != nullptr) {
			return;
		}
	}
	characterSprite->stopAllActionsByTag(_currentMoveActionTag);
	if (characterSprite == _miniUnit) {
		_miniUnit->stopAllActions();
	}
	auto action = Animate::create(createMoveAnimationWithDefine(directionId, _moveImagePath));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	if (characterSprite != _miniUnit) {
		_currentMoveActionTag = directionId;
	}
	characterSprite->runAction(repeat);

}
void BattleScene::updateMiniMap()
{
	if (testObject == nullptr) return;
	auto objectPos = testObject->getPosition();

	float positionXScaleRate = _miniMap->getContentSize().width / (_visibleSize.width * 2);
	float positionYScaleRate = _miniMap->getContentSize().height / (_visibleSize.height * 2);
	_selectRect->setPosition(Vec2(objectPos.x*positionXScaleRate,objectPos.y*positionYScaleRate));
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyIconInMinimap[i]->setPosition(Vec2(_allEnemyUnitSprite[i]->getPositionX()*positionXScaleRate, _allEnemyUnitSprite[i]->getPositionY()*positionYScaleRate));
	}
	for (int i = 1; i < _allAlliedUnitData.size(); i++)
	{
		_allAlliedUnitIconInMiniMap[i-1]->setPosition(Vec2(_allAlliedUnitSprite[i]->getPositionX()*positionXScaleRate, _allAlliedUnitSprite[i]->getPositionY()*positionYScaleRate));
	}
	
}


void BattleScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	_touchMoveBeginSprite->setTexture("image/screen/battle/ui_move.png");
	_touchMoveEndSprite->setTexture("image/screen/battle/ui_move_end.png");
	_touchMoveBeginSprite->setVisible(false);
	_touchMoveEndSprite->setVisible(false);

	/////////////////////////////////////////////////////////////////////////
	// LOGIC
	/////////////////////////////////////////////////////////////////////////
	testObject->stopAllActionsByTag(STOP_LONG_MOVE);
	if ((_moveMode == MOVE_MANUAL) || (_moveMode == MOVE_CIRCLE && _checkOneTapMoveFlg == true) || (_moveMode == MOVE_CIRCLE && _checkLongTapMoveFlg == true))
	{
		/* Day la truong hop MOVE_MANUAL || MOVE_CIRCLE_MANUAL || MOVE_CIRCLE_LONGTAP */
		testObject->stopMoveAction();
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
			testObject->stopMoveAction();
			auto distanVector = touch->getLocation() - Vec2(_visibleSize / 2);
			auto vec = AStarPathFindingAlgorithm(testObject->getPosition(), testObject->getPosition() + distanVector);
		}

		if (_moveMode == MOVE_CIRCLE)
		{
			if (_miniCircle->getBoundingBox().containsPoint(touch->getLocation())) {

				log("Day la oneTap circle");
				testObject->setMoveMode(5);
				_miniUnit->setMoveMode(5);

				float moveTime;
				if (_circleProperty == MOVE_CIRCLE_TIME)
				{
					moveTime = 1;
				}
				if (_circleProperty == MOVE_CIRCLE_DISTANCE)
				{
					moveTime = (float)(MOVE_DISTANCE / _mainCharacterData.move_speed);
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

Animation* BattleScene::createMoveAnimationWithDefine(int imageId, string path)
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
Animation* BattleScene::createAttackAnimationWithDefine(int imageId, string path)
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
	return true;
}

bool BattleScene::onPhysicContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve)
{
// 	log("contact pre solve");
	auto spriteA = contact.getShapeA()->getBody()->getNode();
	auto spriteB = contact.getShapeB()->getBody()->getNode();

	return true;
}

void BattleScene::rotateCharacter(Sprite *target, int direc)
{
	//log("Rotate direc %d", direc);
	//auto animation = Animation::create();
	char szName[100] = { 0 };
	sprintf(szName, "unit_00_0%d_%d.png", direc, 1);
	string p = _moveImagePath;
	p.append(szName);
	Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(p.c_str());
	target->setTexture(texture);
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

void BattleScene::changeImageButtonCallback(Ref *pSender, Widget::TouchEventType type)
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
		endBattle();
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
	//BattleAPI::getInstance()->sendMoveBeginEvent(_mainCharacterData, detectDirectionBaseOnTouchAngle(angle));
}

void BattleScene::sendMoveEvent(int direction, float angle, bool onMovingFlg)
{
	BattleAPI::getInstance()->sendMoveEvent(_mainCharacterData,direction,angle, testObject->getPosition(),getUnitStatus(), onMovingFlg );
}

void BattleScene::sendMoveEndEvent()
{
	BattleAPI::getInstance()->sendMoveEndEvent(_mainCharacterData);
}


void BattleScene::autoRestoreHpAndMp()
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
	//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
}

void BattleScene::updateSlider()
{
	_mainCharacterMiniHpBar->setPercent(ceil((100.0f *_mainCharacterData.hp / _allAlliedUnitMaxHp[0])));
	_mainCharacterHpBar->setPercent(_mainCharacterMiniHpBar->getPercent());

	_mainCharacterMpBar->setPercent(ceil((100.0f * _mainCharacterData.mp / _mainCharacerMaxMp)));

	for (int i = 0; i < _allEnemyUnitData.size(); i++)
	{
		_allEnemyHpBar[i]->setPercent(ceil(( 100.0f * _allEnemyUnitData[i].hp / _allEnemuUnitMaxHp[i])));
	}
}
void BattleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
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
		if (skill.range_distance > 0 && skill.mp_cost <= _mainCharacterData.mp)  {
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
		if (skill.target_type == TARGET_ONE && skill.skill_type == TYPE_ATTACK && _indexOfBeAttackEnemy < 0) {
			log("Invalid attack target");
			return;
		}
		if (skill.mp_cost <= _mainCharacterData.mp){
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

void BattleScene::playSkill(UserSkillInfo skillData)
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


void BattleScene::skillRestoreAction(UserSkillInfo skillInfo)
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

void BattleScene::skillBuffAction(UserSkillInfo skillInfo)
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

void BattleScene::skillAttackAction(UserSkillInfo skillInfo)
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

void BattleScene::skillRestoreAll(UserSkillInfo skillInfo)
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
	//TODO
	vector<int> effectUnitList = {};
	if (skillInfo.range_distance > 0) 
	{

		vector<int> allUnitIndex = detectUnitInAoe(skillInfo, ALLIED_FLAG);

		for (int &index : allUnitIndex)
		{
			//TODO change to unique unitID
			effectUnitList.push_back(_allAlliedUnitData[index].mst_unit_id);

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
			//TODO change to unique unit ID
			effectUnitList.push_back(_allAlliedUnitData[i].mst_unit_id);
			_allAlliedUnitData[i].hp += value;
			if (_allAlliedUnitData[i].hp > _allAlliedUnitMaxHp[i]) {
				_allAlliedUnitData[i].hp = _allEnemuUnitMaxHp[i];
			}
			//BattleAPI::getInstance()->battleSyncEvent(_mainCharacterData);
			_allAlliedUnitHpBar[i]->setPercent(100.0f * _allAlliedUnitData[i].hp/ _allEnemuUnitMaxHp[i]);

		}
	}
	//TODO
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, effectUnitList);

	createSorceryEffect(testObject, SORCERY_GREEN);
}

void BattleScene::skillRestoreOne(UserSkillInfo skillInfo)
{
	//TODO
	//
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, { _mainCharacterData.mst_unit_id });
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
		, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
		, nullptr);
	effectHeal->runAction(healSequence);

}

void BattleScene::skillHelpAll(UserSkillInfo skillInfo)
{
	int value = 0;
}

void BattleScene::skillHelpOne(UserSkillInfo skillInfo)
{
	//TODO
	//Change this id by unique unit Id
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, { _mainCharacterData.mst_unit_id });
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
			, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
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
			, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
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
			, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
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
			, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
			, nullptr);
		effectHelpMoveSpeed->runAction(helpmMoveSpeedSequence);

		break;
	}
	default:
		break;
	}


}

void BattleScene::skillAttackAll(UserSkillInfo skillInfo)
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
	//TODO change to unique unitID
	vector<int> effectdUnitId;
	for (auto &in : unitIndex)
	{
		effectdUnitId.push_back(_allEnemyUnitData[in].mst_unit_id);
	}
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, effectdUnitId);

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
					, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
					, nullptr);
				attackFire->runAction(attackFireSequence);

			}), nullptr);
			_allEnemyUnitSprite[index]->setTag(index);
			_allEnemyUnitSprite[index]->runAction(action);
		}
	}
}

void BattleScene::skillAttackOne(UserSkillInfo skillInfo)
{
	log("Attack One");

	//TODO change to unique unitID
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, { _allEnemyUnitData[_indexOfBeAttackEnemy].mst_unit_id });

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
		, CallFuncN::create(CC_CALLBACK_1(BattleScene::removeEffect, this))
		, nullptr);
	attackFire->runAction(attackFireSequence);

	}), nullptr); 

	_allEnemyUnitSprite[_indexOfBeAttackEnemy]->runAction(action);

}


vector<int> BattleScene::detectUnitInAoe(UserSkillInfo skill, int unitFlg, bool drawFlg /*= true*/)

{
	vector<int> resultUnitId;
	vector<Sprite*> allUnit;
	auto teamFlg = UserModel::getInstance()->getUserInfo().team_id;
	switch (unitFlg)
	{
	case ENEMY_FLAG:
		for (auto &unit : _allEnemyUnitSprite)
		{
			allUnit.push_back(unit);
		}
		//More for tower
		break;
	case ALLIED_FLAG:
		for (auto &unit: _allAlliedUnitSprite)
		{
			allUnit.push_back(unit);
			//more for tower
		}
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
// 	bool drawFlg = true;
	if (skill.multi_effect != 1)
	{
		drawFlg = false;
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

void BattleScene::updateHpAndMpViewLabel()
{
	string hp = DataUtils::numberToString(_mainCharacterData.hp).append("/").append(DataUtils::numberToString(_allAlliedUnitMaxHp[0]));
	_hpViewLabel->setString(hp.c_str());
	string mp = DataUtils::numberToString(_mainCharacterData.mp).append("/").append(DataUtils::numberToString(_mainCharacerMaxMp));
	_mpViewlabel->setString(mp.c_str());
}

void BattleScene::endBattle()
{
	BattleAPI::getInstance()->sendBattleEndEvent([&](SIOClient *client, const string data) {
		log("battle end callback with data: %s", data.c_str());
	});
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, BatleResultScene::createScene(_blueTeamInfo,_redTeamInfo)));
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
	auto a = detectUnitInAoe(skill, ENEMY_FLAG);
	_onSelectSkillData = skill;
	for (auto& enemy : a)
	{
		_allEnemyUnitSprite[enemy]->setColor(Color3B::RED);
	}
}


bool BattleScene::detectPointInTriangle(Vec2 point, vector<Vec2> points)
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

}

void BattleScene::saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg)
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

void BattleScene::saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg)
{
	if (teamFlg == _currentPlayerTeamFlg) {
		switch (_currentPlayerTeamFlg)
		{
		case TEAM_FLG_BLUE:
			_blueTeamInfo[killerId].killNum++;
			if(deadUnitId < _allEnemyUnitData.size()) _redTeamInfo[deadUnitId].deadNum++;
			break;
		case TEAM_FLG_RED:
			_redTeamInfo[killerId].killNum++;
			if (deadUnitId < _allEnemyUnitData.size())_blueTeamInfo[deadUnitId].deadNum++;
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
				if (killerId < _allEnemyUnitData.size()) _redTeamInfo[killerId].killNum++;

				break;
			case TEAM_FLG_RED:
				_redTeamInfo[deadUnitId].deadNum++;
				if (killerId < _allEnemyUnitData.size())_blueTeamInfo[killerId].killNum++;
				break;
			default:
				break;
			}
		}
	}
}
/*status type: POISON, STUN, BUFF_ATTACK, BUFF_DEFENCE, DEBUFF_ATTACK*/
void BattleScene::displayUnitStatus(Sprite *parent, int statusType, UserSkillInfo skillInfo, int spIndex /*= 0*/)
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

void BattleScene::skillPoisonAction(UserSkillInfo skillInfo)
{
	vector<int> units = detectUnitInAoe(skillInfo, ENEMY_FLAG);
	//TODO change to unit unitque ID
	vector<int> effectedUnits;
	for (auto &i : units)
	{
		effectedUnits.push_back(_allEnemyUnitData[i].mst_unit_id);
	}
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, effectedUnits);

	for (auto & index :units)
	{
		displayUnitStatus(_allEnemyUnitSprite[index], POISON, skillInfo,index);
		poisonEffectAction(skillInfo, index);
	}
}

void BattleScene::skillStunAction(UserSkillInfo skillInfo)
{
	vector<int> units = detectUnitInAoe(skillInfo,ENEMY_FLAG);
	//TODO change to unique unitID
	vector<int> effectedUnits;
	for (int &index : units)
	{
		effectedUnits.push_back(_allEnemyUnitData[index].mst_unit_id);
	}
	BattleAPI::getInstance()->sendSkillEvent(skillInfo, effectedUnits);

	for (auto& i : units)
	{
		displayUnitStatus(_allEnemyUnitSprite[i], STUN, skillInfo,i);
		_allEnemyUnitData[i].isStun = true;
		_allEnemyUnitSprite[i]->runAction(Sequence::create(DelayTime::create(skillInfo.duration), CallFuncN::create([&,i](Ref *p){
			_allEnemyUnitData[i].isStun = false;
		}), nullptr));
	}
}

void BattleScene::poisonEffectAction(UserSkillInfo skill, int index)
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

void BattleScene::enemyUnitAutoMoveTest()
{
	for (int i = 0; i < _allEnemyUnitData.size(); i++)
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

void BattleScene::actionCharacterCopy(int directionId, Sprite *sprite)
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

void BattleScene::fountainRestoreEffect()
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

void BattleScene::enemyRespawAction(int index)
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


void BattleScene::removeEffect(Ref* pSender)
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
	sprite->setPosition(Vec2(parent->getContentSize().width / 2, parent->getContentSize().height));
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

cocos2d::Vec2 BattleScene::getTitlePosForPosition(Vec2 location)
{
	int x = location.x / _myMap->getTileSize().width;
	int y = ((_myMap->getMapSize().height*_myMap->getTileSize().height - location.y) / _myMap->getTileSize().height);
	return Vec2(x, y);
}
cocos2d::Vec2 BattleScene::getPositionForTitleCoord(Vec2 tileCoord)
{
	int x = (tileCoord.x * _myMap->getTileSize().width) + _myMap->getTileSize().width / 2;
	int y = (_myMap->getMapSize().height * _myMap->getTileSize().height) -
		(tileCoord.y * _myMap->getTileSize().height) - _myMap->getTileSize().height / 2;
	return Vec2(x, y);
}

vector<Vec2> BattleScene::AStarPathFindingAlgorithm(Vec2 curentPos, Vec2 destinationPos)
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
	if (direc != 0)
	{
		actionCharacter(direc, testObject);
		if (_moveMode == MOVE_CIRCLE)
		{
			actionCharacter(direc, _miniUnit);
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

	_miniUnit = Character::createCharacter(_selectedUnitId + 1);
	_miniUnit->setScale(float(0.25));
	addChild(_miniUnit, 100);
	_miniUnit->setPosition(_miniCircle->getPosition());
}


