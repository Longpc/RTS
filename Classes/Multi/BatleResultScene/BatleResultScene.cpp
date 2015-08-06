#pragma execution_character_set("utf-8")

#include "BatleResultScene.h"

Scene* BatleResultScene::createScene(int winTeam)
{
	auto scene = Scene::create();
	auto lay = BatleResultScene::create(winTeam);

	scene->addChild(lay);
	return scene;
}
BatleResultScene* BatleResultScene::create(int winTeam)
{
	BatleResultScene *layer = new BatleResultScene();
	if (layer && layer->init(winTeam)) {
		layer->autorelease();
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}
bool BatleResultScene::init(int winTeam)
{
	if (!LayerBase::init()) {
		return false;
	}
	_gameMode = UserDefault::getInstance()->getIntegerForKey("MODE");
	_menu->setVisible(false);
	auto a = UserModel::getInstance()->getUserInfo();
	_userNameLabel->setString(a.name.c_str());
	_currentTeam = UserModel::getInstance()->getUserInfo().team_id;
	if (winTeam == TEAM_FLG_BLUE)
	{
		_defaultLabel->setString("BLUE TEAM WON");
	}
	else
	{
		_defaultLabel->setString("RED TEAM WON");
	}
	auto homeButton = Button::create();
	homeButton->loadTextureNormal("image/button/new/button_home.png");
	homeButton->setPosition(Vec2(homeButton->getContentSize().width/2 + 20, _visibleSize.height - 70));
	homeButton->setTouchEnabled(true);
	homeButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::nextButtonCallback, this));
	addChild(homeButton, 10);
	if (UserDefault::getInstance()->getIntegerForKey("MODE") == MULTI_MODE)
	{
		if (_isSendRequest == false) {
			_isSendRequest = true;
			auto sv = NodeServer::getInstance()->getClient();
			sv->emit("get_battle_result", "hello");
			sv->on("battle_result", [&](SIOClient* client, const string data) {
				log("battle result with data: %s", data.c_str());
				if (_isReceiveResponse) return;
				_isReceiveResponse = true;
				Document doc;
				doc.Parse<0>(data.c_str());
				if (doc.HasParseError()) {
					log("Parse JSOn error");
					return;
				}
				if (doc.IsArray()) {

					vector<UserBattleInfo> alliedTeam;
					vector<UserBattleInfo> enemyTeam;
					for (int i = 0; i < doc.Size(); i++)
					{
						int unitID = doc[rapidjson::SizeType(i)]["unit_id"].GetInt();
						auto unitInfo = UserUnitModel::getInstance()->getUnitInfoById(unitID);
						UserBattleInfo temp;
						temp.name = unitInfo.name;
						temp.unitId = unitID;
						temp.totalDealDame = doc[rapidjson::SizeType(i)]["totalDealDame"].GetInt();
						temp.totalReceivedDame = doc[rapidjson::SizeType(i)]["totalReceiveDame"].GetInt();
						temp.killNum = doc[rapidjson::SizeType(i)]["killNum"].GetInt();
						temp.deadNum = doc[rapidjson::SizeType(i)]["deadNum"].GetInt();
						temp.assistNum = doc[rapidjson::SizeType(i)]["assistNum"].GetInt();
						temp.maxKillCombo = doc[rapidjson::SizeType(i)]["maxKillCombo"].GetInt();
						temp.longestKillstreak = doc[rapidjson::SizeType(i)]["longestKillStreak"].GetInt();


						if (doc[rapidjson::SizeType(i)]["team_id"].GetInt() == _currentTeam)
						{
							alliedTeam.push_back(temp);
							if (strcmp(doc[rapidjson::SizeType(i)]["uuid"].GetString(), UserModel::getInstance()->getUuId().c_str()) == 0) {
								_saveYourUnitIndex = alliedTeam.size() - 1;
							}
						}
						else
						{
							enemyTeam.push_back(temp);
						}
					}

					if (_currentTeam == TEAM_FLG_BLUE)
					{
						_blueTeamBattleResult = alliedTeam;
						_redTeamBattleResult = enemyTeam;
					}
					else
					{
						_blueTeamBattleResult = enemyTeam;
						_redTeamBattleResult = alliedTeam;
					}

					createContent();
				}
			});
		} //end of check request
	}//end of check gameMode
	else
	{
		createContent();
	}
	

	
	return true;
}

void BatleResultScene::nextButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_gameMode == MULTI_MODE)
		{
			auto sv = NodeServer::getInstance()->getClient();
			if (sv != nullptr)
			{
				string uu = UserModel::getInstance()->getUuId();
				sv->emit("clean_battle_result", uu.c_str());
				NodeServer::destroyInstance();
				MapServer::destroyInstace();
				MoveServer::destroyInstance();
				RoomUserModel::destroyInstance();
			}
		}
		Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY, ModeSelectScene::createScene()));
		break; 
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleResultScene::createContent()
{

	for (int i = 0; i < 3; i++)
	{
		auto slot = ClipingButtonBase::create("image/navigator/selct_scene_circle.png", "image/screen/unitSelect/inactive.png", "image/screen/unitSelect/active.png");
		slot->setPosition(Vec2(_visibleSize.width / 2 + 200*(i-1)  , _visibleSize.height - 150));
		slot->setSelected(false);
		Button * back = slot->getBackgroundButton();
		back->addChild(createUnitNameBg(Vec2(back->getContentSize().width/2, 0)));
		_unitNameLabel.push_back(createUniNameLabel(Vec2(back->getContentSize().width / 2, 0)));
		back->addChild(_unitNameLabel.back());
		_allSlot.push_back(slot);
		addChild(_allSlot.back(), 100);
	}
	/*to special current player unit*/
	//_allSlot[0]->setSelected(true);

	_blueTeamTabBackground = Sprite::create("image/screen/battleResult/back.png");
	_blueTeamTabBackground->setPosition(Vec2(_visibleSize.width/2,_visibleSize.height/2 - 95));
	addChild(_blueTeamTabBackground, 10);

	createBattleInfo(_blueTeamTabBackground,_blueTeamBattleResult);

	//updateUnitSlot(_blueTeamBattleResult);

	_redTeamTabBackground = Sprite::create("image/screen/battleResult/back.png");
	_redTeamTabBackground->setPosition(_blueTeamTabBackground->getPosition());
	addChild(_redTeamTabBackground, 10);
	//_redTeamTabBackground->setVisible(false);

	createBattleInfo(_redTeamTabBackground, _redTeamBattleResult);
	
	_blueTeamButton = Button::create();
	_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
	_blueTeamButton->setPosition(Vec2(_blueTeamTabBackground->getPositionX() + _blueTeamTabBackground->getContentSize().width / 2 - _blueTeamButton->getContentSize().width / 2 - 10, _blueTeamTabBackground->getPositionY() - _blueTeamTabBackground->getContentSize().height/2 - 20));
	_blueTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::tabButtonClickCallback, this));
	//_blueTeamButton->setTouchEnabled(false);
	addChild(_blueTeamButton, 10);

	_redTeamButton = Button::create();
	_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
	_redTeamButton->setPosition(_blueTeamButton->getPosition() - Vec2(_redTeamButton->getContentSize().width, 0));
	_redTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::tabButtonClickCallback, this));
	addChild(_redTeamButton, 10);
	if (_currentTeam == TEAM_FLG_BLUE) {
		updateUnitSlot(_blueTeamBattleResult, true);
		_redTeamTabBackground->setVisible(false);
		_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
		_blueTeamButton->setTouchEnabled(false);
		_blueTeamButton->setTag(100);
		_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
	}
	else 
	{
		updateUnitSlot(_redTeamBattleResult, true);
		_blueTeamTabBackground->setVisible(false);
		_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab_disable.png");
		_redTeamButton->setTouchEnabled(false);
		_redTeamButton->setTag(100);
		_redTeamButton->loadTextureNormal("image/tab/new/red_team_tab.png");
	}

}


void BatleResultScene::tabButtonClickCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		auto bt = (Button*)(pSender);
		bool checkFlg = false;
		if (bt->getTag() == 100) checkFlg = true;
		if (bt == _blueTeamButton)
		{
			_allSlot[0]->setSelected(true);
			_blueTeamTabBackground->setVisible(true);
			_redTeamTabBackground->setVisible(false);
			_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
			_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
			_blueTeamButton->setTouchEnabled(false);
			_redTeamButton->setTouchEnabled(true);
			updateUnitSlot(_blueTeamBattleResult,checkFlg);
		}
		else {
			_allSlot[0]->setSelected(false);
			_blueTeamTabBackground->setVisible(false);
			_redTeamTabBackground->setVisible(true);
			_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab_disable.png");
			_redTeamButton->loadTextureNormal("image/tab/new/red_team_tab.png");
			_blueTeamButton->setTouchEnabled(true);
			_redTeamButton->setTouchEnabled(false);
			updateUnitSlot(_redTeamBattleResult,checkFlg);
		}
		
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}


Sprite* BatleResultScene::createUnitNameBg(Vec2 pos)
{
	auto sp = Sprite::create("image/screen/unitSelect/name.png");
	sp->setPosition(pos);
	return sp;
}
Label* BatleResultScene::createUniNameLabel(Vec2 pos)
{
	auto lb = Label::createWithSystemFont("", JAPANESE_FONT_1_BOLD, 20);
	lb->setPosition(pos);
	lb->setHorizontalAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::BLACK);
	return lb;
}

void BatleResultScene::createBattleInfo(Sprite *parent, vector<UserBattleInfo> info)
{
	string tempStr = "キル\nデス\nアシスト\n連続キル\n同時キル\n与ダメージ\n被ダメージ";

	auto tempLabel = Label::createWithSystemFont(tempStr.c_str(), JAPANESE_FONT_1_REGULAR, 30);
	tempLabel->setPosition(Vec2(40, parent->getContentSize().height - 50));
	tempLabel->setColor(Color3B::BLACK);
	tempLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	tempLabel->setHorizontalAlignment(TextHAlignment::LEFT);
	parent->addChild(tempLabel);

	for (int i = 0; i < info.size(); i++)
	{
		stringstream dm;
		dm << info[i].killNum << "\n" << info[i].deadNum << "\n" << info[i].assistNum << "\n" << info[i].longestKillstreak << "\n" << info[i].maxKillCombo << "\n" << info[i].totalDealDame << "\n" << info[i].totalReceivedDame;
		auto label = Label::createWithSystemFont(dm.str().c_str(), JAPANESE_FONT_1_BOLD, 30);
		label->setColor(Color3B::BLACK);
		label->setPosition(Vec2(270 + 200 * i, parent->getContentSize().height - 50));
		label->setHorizontalAlignment(TextHAlignment::RIGHT);
		label->setAnchorPoint(Vec2::ANCHOR_TOP_RIGHT);
		parent->addChild(label);
	}
}

void BatleResultScene::updateUnitSlot(vector<UserBattleInfo> info, bool checkFlg)
{
	for (int i = 0; i < info.size(); i++)
	{
		_unitNameLabel[i]->setString(info[i].name);
		_allSlot[i]->getClickableButton()->loadTextureNormal(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(info[i].unitId));
		_allSlot[i]->getClickableButton()->setScale(4.0f);
		//update image
		if (checkFlg && i == _saveYourUnitIndex) {
			_allSlot[i]->setSelected(true);
		}
		else{
			_allSlot[i]->setSelected(false);
		}
	}
	if (info.size() == _allSlot.size()) return;

	for (int j = info.size(); j < _allSlot.size(); j++)
	{
		_allSlot[j]->setSelected(false);
		_unitNameLabel[j]->setString("");
		_allSlot[j]->resetClickableButton();
	}
}

BatleResultScene::~BatleResultScene()
{
	_isSendRequest = false;
}




