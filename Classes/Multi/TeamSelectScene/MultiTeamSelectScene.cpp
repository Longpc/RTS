#pragma execution_character_set("utf-8")
#include "MultiTeamSelectScene.h"

Scene * MultiTeamSelectScene::createScene(int userId)
{
	auto scene = Scene::create();
	auto layer = MultiTeamSelectScene::create();

	scene->addChild(layer);

	return scene;
}

bool MultiTeamSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}

	
	if (_defaultLabel != nullptr) {
		_defaultLabel->setString("チームを選択してください");
	}
	//getUserInfor
	_userNameLabel->setString("テスト");
	auto blueTeamBg = Sprite::create("image/screen/base.png");
	blueTeamBg->setPosition(Vec2(_visibleSize.width / 2,10+(_visibleSize.height - 100) * 1 / 4));
	addChild(blueTeamBg, 1);
	auto redTeamBg = Sprite::create("image/screen/base.png");
	redTeamBg->setPosition(Vec2(_visibleSize.width / 2, 10+(_visibleSize.height - 100) * 3 / 4));
	addChild(redTeamBg, 1);

	Button *blueButton = Button::create();
	blueButton->loadTextureNormal("image/button/new/blue.png");
	blueButton->setTouchEnabled(true);
	blueButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::blueTeamButtonCallback, this));
	blueButton->setPosition(Vec2(_visibleSize.width *3 / 4, blueTeamBg->getPositionY()-blueButton->getContentSize().height/2));
	addChild(blueButton,2);

	Button *redButton = Button::create();
	redButton->loadTextureNormal("image/button/new/red.png");
	redButton->setTouchEnabled(true);
	redButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::redTeamButtonCallback, this));
	redButton->setPosition(Vec2(_visibleSize.width * 3 / 4, redTeamBg->getPositionY() - redButton->getContentSize().height / 2));
	addChild(redButton,2);

	/*Debug dialog button
	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(MultiTeamSelectScene::testFullTeam, this));
	addChild(nextButton, 10);*/

	getRoomInfo(_curRoomId);
	getAndShowTeamInfo(_blueTeamId,blueTeamBg);
	getAndShowTeamInfo(_redTeamId, redTeamBg);

	return true;
}

void MultiTeamSelectScene::onBackButtonClick(Ref *pSender)
{
	Director::getInstance()->replaceScene(TransitionMoveInL::create(SCREEN_TRANSI_DELAY,UserSelect::createScene()));
}

void MultiTeamSelectScene::redTeamButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkTeamFull(_redTeamId) == true) {
			enterTeam(_redTeamId);
		}
		else {
			//SHOW DIALOG
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog, 100);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiTeamSelectScene::blueTeamButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkTeamFull(_blueTeamId) == true) {
			enterTeam(_blueTeamId);
		}
		else {
			//SHOW DIALOg
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog, 100);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void MultiTeamSelectScene::getRoomInfo(int roomId)
{
	//get data from server
	//create sample data
	/*
	Old version had scrollview to show list user in current room
	for (int i = 0; i < 6;i++)
	{
		std::stringstream name;
		name << "User " << i;

		UserInfo temp;
		temp._name = name.str().c_str();
		temp._onTeam = i % 2;
		_roomUserInfo.push_back(temp);
	}

	auto roomLabel = LabelTTF::create("Room members", "fonts/Marker Felt.ttf", 30);
	roomLabel->setPosition(Vec2(roomLabel->getContentSize().width/2, 350));
	addChild(roomLabel);
	auto scroll = extension::ScrollView::create();
	scroll->setViewSize(Size(roomLabel->getContentSize().width, 300));
	scroll->setPosition(Vec2::ZERO);
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
	scroll->updateInset();
	scroll->setVisible(true);

	addChild(scroll);
	auto layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	layer->setContentSize(Size(roomLabel->getContentSize().width, _roomUserInfo.size()+50+100));
	layer->setPosition(Vec2(0, -layer->getContentSize().height / 2));
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	for (int i = 0; i < _roomUserInfo.size(); i++)
	{
		auto userName = LabelTTF::create(_roomUserInfo[i]._name, "fonts/Marker Felt.ttf", 25);
		userName->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		if (_roomUserInfo[i]._onTeam == 0) {
			userName->setOpacity(70);
		}
		userName->setPosition(Vec2(0, layer->getContentSize().height- 50 * i+1));
		layer->addChild(userName);
	}
	*/

}
void MultiTeamSelectScene::getAndShowTeamInfo(int teamId,Sprite *parent)
{
	//get data form server
	vector<UserInfo> teamInfo;
	for (int i = 0; i < 2; i++)
	{
		std::stringstream name;
		name << "Blue " << i+1;

		UserInfo temp;
		temp._name = name.str().c_str();
		temp._onTeam = _blueTeamId;
		teamInfo.push_back(temp);
	}
	std::stringstream memNum;
	memNum << "メンバー数: "<< teamInfo.size();
	auto teamLabel = Label::createWithSystemFont(memNum.str().c_str(), JAPANESE_FONT_1_BOLD, 30);
	teamLabel->setPosition(Vec2(50, parent->getContentSize().height - 20));
	teamLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	teamLabel->setColor(Color3B::BLACK);
	parent->addChild(teamLabel);

	float baseX = parent->getContentSize().width*1/3 - 45;
	float baseY = parent->getContentSize().height * 2 / 3 - 10;

	auto username1Bg = createUsernameBackground(Vec2(baseX, baseY));
	username1Bg->addChild(createLabelWithStringandPosition(teamInfo[0]._name, Vec2(10, username1Bg->getContentSize().height / 2)));
	parent->addChild(username1Bg);

	auto userName2Bg = createUsernameBackground(Vec2(baseX, baseY * 2 / 3 -5));
	userName2Bg->addChild(createLabelWithStringandPosition(teamInfo[1]._name, Vec2(10, username1Bg->getContentSize().height / 2)));
	parent->addChild(userName2Bg);

	auto username3Bg = createUsernameBackground(Vec2(baseX, baseY * 1 / 3 - 10));
	//username3Bg->addChild(createLabelWithStringandPosition(_blueTeamUserInfo[2]._name, Vec2(10, username3Bg->getContentSize().height / 2)));
	parent->addChild(username3Bg);

	/*auto scroll = extension::ScrollView::create();
	scroll->setViewSize(Size(teamLabel->getContentSize().width, 150));
	scroll->setPosition(Vec2(teamLabel->getPositionX(),50));
	scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
	scroll->updateInset();
	scroll->setVisible(true);

	addChild(scroll);
	auto layer = Layer::create();
	layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	layer->setContentSize(Size(teamLabel->getContentSize().width, 150));
	layer->setPosition(Vec2(teamLabel->getPositionX(), -layer->getContentSize().height / 2));
	scroll->setContainer(layer);
	scroll->setContentOffset(scroll->minContainerOffset());

	for (int i = 0; i < _blueTeamUserInfo.size(); i++)
	{
		auto userName = LabelTTF::create(_blueTeamUserInfo[i]._name, "fonts/Marker Felt.ttf", 25);
		userName->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		userName->setPosition(Vec2(0, layer->getContentSize().height - 50 * i + 1));
		layer->addChild(userName);
	}
	*/
}

bool MultiTeamSelectScene::checkTeamFull(int teamId)
{
	return _onAccess;
}

void MultiTeamSelectScene::enterTeam(int teamId)
{
	Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(teamId,MULTI_MODE)));
}

void MultiTeamSelectScene::testFullTeam(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (_onAccess) {
			_onAccess = false;
		}
		else {
			_onAccess = true;
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

Sprite* MultiTeamSelectScene::createUsernameBackground(Vec2 position)
{
	auto sp = Sprite::create("image/navigator/user_name.png");
	sp->setPosition(position);
	return sp;
}

Label* MultiTeamSelectScene::createLabelWithStringandPosition(string text, Vec2 pos)
{
	auto lb = Label::createWithSystemFont(text.c_str(), JAPANESE_FONT_1_BOLD, 25);
	lb->setHorizontalAlignment(TextHAlignment::LEFT);
	lb->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	lb->setColor(Color3B::BLACK);
	lb->setPosition(pos);
	return lb;
}



