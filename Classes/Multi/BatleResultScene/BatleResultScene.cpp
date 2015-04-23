#pragma execution_character_set("utf-8")

#include "BatleResultScene.h"

Scene* BatleResultScene::createScene(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo)
{
	auto scene = Scene::create();
	auto lay = BatleResultScene::create(blueTeamInfo,readTeamInfo);

	scene->addChild(lay);
	return scene;
}
BatleResultScene* BatleResultScene::create(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo)
{
	BatleResultScene *layer = new BatleResultScene();
	if (layer && layer->init(blueTeamInfo, readTeamInfo)) {
		layer->autorelease();
		return layer;
	}

	CC_SAFE_DELETE(layer);
	return NULL;
}
bool BatleResultScene::init(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo)
{
	if (!LayerBase::init()) {
		return false;
	}
	_menu->setVisible(false);

	_blueTeamInfo = blueTeamInfo;
	_readTeamInfo = readTeamInfo;
	_defaultLabel->setString("TEAM BLUE WIN");
	auto homeButton = Button::create();
	homeButton->loadTextureNormal("image/button/new/button_home.png");
	homeButton->setPosition(Vec2(homeButton->getContentSize().width/2 + 20, _visibleSize.height - 70));
	homeButton->setTouchEnabled(true);
	homeButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::nextButtonCallback, this));
	addChild(homeButton, 10);

	createContent();
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
	_allSlot[0]->setSelected(true);

	_blueTeamTabBackground = Sprite::create("image/screen/battleResult/back.png");
	_blueTeamTabBackground->setPosition(Vec2(_visibleSize.width/2,_visibleSize.height/2 - 95));
	addChild(_blueTeamTabBackground, 10);

	createBattleInfo(_blueTeamTabBackground,_blueTeamInfo);
	updateUnitSlot(_blueTeamInfo);

	_redTeamTabBackground = Sprite::create("image/screen/battleResult/back.png");
	_redTeamTabBackground->setPosition(_blueTeamTabBackground->getPosition());
	addChild(_redTeamTabBackground, 10);
	_redTeamTabBackground->setVisible(false);

	createBattleInfo(_redTeamTabBackground, _readTeamInfo);
	
	_blueTeamButton = Button::create();
	_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
	_blueTeamButton->setPosition(Vec2(_blueTeamTabBackground->getPositionX() + _blueTeamTabBackground->getContentSize().width / 2 - _blueTeamButton->getContentSize().width / 2 - 10, _blueTeamTabBackground->getPositionY() - _blueTeamTabBackground->getContentSize().height/2 - 20));
	_blueTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::tabButtonClickCallback, this));
	_blueTeamButton->setTouchEnabled(false);
	addChild(_blueTeamButton, 10);

	_redTeamButton = Button::create();
	_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
	_redTeamButton->setPosition(_blueTeamButton->getPosition() - Vec2(_redTeamButton->getContentSize().width, 0));
	_redTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::tabButtonClickCallback, this));
	addChild(_redTeamButton, 10);


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
		if (bt == _blueTeamButton)
		{
			_allSlot[0]->setSelected(true);
			_blueTeamTabBackground->setVisible(true);
			_redTeamTabBackground->setVisible(false);
			_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
			_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
			_blueTeamButton->setTouchEnabled(false);
			_redTeamButton->setTouchEnabled(true);
			updateUnitSlot(_blueTeamInfo);
		}
		else {
			_allSlot[0]->setSelected(false);
			_blueTeamTabBackground->setVisible(false);
			_redTeamTabBackground->setVisible(true);
			_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab_disable.png");
			_redTeamButton->loadTextureNormal("image/tab/new/red_team_tab.png");
			_blueTeamButton->setTouchEnabled(true);
			_redTeamButton->setTouchEnabled(false);
			updateUnitSlot(_readTeamInfo);
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

void BatleResultScene::updateUnitSlot(vector<UserBattleInfo> info)
{
	for (int i = 0; i < info.size(); i++)
	{
		_unitNameLabel[i]->setString(info[i].name);
		_allSlot[i]->getClickableButton()->loadTextureNormal(UserUnit::getInstance()->getUnitImageById(info[i].unitId));
		//update image
	}
}




