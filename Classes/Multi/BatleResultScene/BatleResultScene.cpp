#include "BatleResultScene.h"

Scene* BatleResultScene::createScene()
{
	auto scene = Scene::create();
	auto lay = BatleResultScene::create();

	scene->addChild(lay);
	return scene;
}

bool BatleResultScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	_menu->setVisible(false);
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

	_slot1BackgroundButon = createSlotBaseSprite(Vec2(_visibleSize.width / 2 - 200, _visibleSize.height - 150));
	addChild(_slot1BackgroundButon);
	_slot1BackgroundButon->loadTextureNormal("image/screen/unitSelect/active.png");
	float baseX = _slot1BackgroundButon->getContentSize().width / 2;
	_slot1BackgroundButon->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit1Name = createUniNameLabel(Vec2(baseX, 0));
	_slot1BackgroundButon->addChild(selectedUnit1Name);


	_slot2BackgroundButon = createSlotBaseSprite(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	addChild(_slot2BackgroundButon);
	_slot2BackgroundButon->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit2Name = createUniNameLabel(Vec2(baseX, 0));
	_slot2BackgroundButon->addChild(selectedUnit2Name);

	_slot3BackgroundButon = createSlotBaseSprite(Vec2(_visibleSize.width / 2 + 200, _visibleSize.height - 150));
	addChild(_slot3BackgroundButon);
	_slot3BackgroundButon->addChild(createUnitNameBg(Vec2(baseX, 0)));
	selectedUnit3Name = createUniNameLabel(Vec2(baseX, 0));
	_slot3BackgroundButon->addChild(selectedUnit3Name);

	_slot1 = createSlot(Vec2(_visibleSize.width / 2 - 200, _visibleSize.height - 150));
	_slot2 = createSlot(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	_slot3 = createSlot(Vec2(_visibleSize.width / 2 + 200, _visibleSize.height - 150));

	addChild(_slot1);
	addChild(_slot2);
	addChild(_slot3);

	statusTab = Sprite::create("image/screen/unitSelect/back.png");
	statusTab->setPosition(Vec2(_visibleSize.width/2,_visibleSize.height/2 - 95));
	addChild(statusTab, 10);

	std::stringstream info;
	info << "Name: TEAM BLUE" << "\nAttack: " << 1000000 << "\nDefense: " << 11110 << "\nSpeed: " << 10;
	auto label = LabelTTF::create(info.str().c_str(), "", 25);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(150, statusTab->getContentSize().height - 100));
	label->setHorizontalAlignment(TextHAlignment::LEFT);
	statusTab->addChild(label);

	skillTab = Sprite::create("image/screen/unitSelect/back.png");
	skillTab->setPosition(statusTab->getPosition());
	addChild(skillTab, 10);
	skillTab->setVisible(false);

	std::stringstream info2;
	info2 << "Name: TEAM RED " << "\nAttack: " << 100 << "\nDefense: " << 100 << "\nSpeed: " << 10110;
	auto label2 = LabelTTF::create(info2.str().c_str(), "", 25);
	label2->setColor(Color3B::BLACK);
	label2->setPosition(Vec2(150, statusTab->getContentSize().height - 100));
	label2->setHorizontalAlignment(TextHAlignment::LEFT);
	skillTab->addChild(label2);

	_blueTeamButton = Button::create();
	_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
	_blueTeamButton->setPosition(Vec2(statusTab->getPositionX() + statusTab->getContentSize().width / 2 - _blueTeamButton->getContentSize().width / 2 - 10, statusTab->getPositionY() - statusTab->getContentSize().height/2 - 20));
	_blueTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::statusButtonCallback, this));
	addChild(_blueTeamButton, 10);

	_redTeamButton = Button::create();
	_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
	_redTeamButton->setPosition(_blueTeamButton->getPosition() - Vec2(_redTeamButton->getContentSize().width, 0));
	_redTeamButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::skillButonCallback, this));
	addChild(_redTeamButton, 10);


}

ClippingNode* BatleResultScene::createSlot(Vec2 position)
{
	auto clipNode = ClippingNode::create();
	clipNode->setAlphaThreshold(0);
	clipNode->setPosition(Vec2::ZERO);
	clipNode->setTag(111);
	clipNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);

	auto mask = Sprite::create("image/navigator/selct_scene_circle.png");
	mask->setPosition(position);
	clipNode->setStencil(mask);

	// add "HelloWorld" splash screen"
	auto sprite = Sprite::create("image/unit/1(3).png");

	// position the sprite on the center of the screen
	sprite->setPosition(position);

	// add the sprite as a child to this layer
	clipNode->addChild(sprite, 0);

	return clipNode;
}

void BatleResultScene::statusButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusTab->setVisible(true);
		skillTab->setVisible(false);
		_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
		_redTeamButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void BatleResultScene::skillButonCallback(Ref *pSEnder, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusTab->setVisible(false);
		skillTab->setVisible(true);
		_blueTeamButton->loadTextureNormal("image/tab/new/blue_team_tab_disable.png");
		_redTeamButton->loadTextureNormal("image/tab/new/red_team_tab.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
Button* BatleResultScene::createSlotBaseSprite(Vec2 pos)
{
	auto sp = Button::create();
	sp->loadTextureNormal("image/screen/unitSelect/inactive.png");
	sp->setEnabled(false);
	sp->setPosition(pos);
	return sp;
}

Sprite* BatleResultScene::createUnitNameBg(Vec2 pos)
{
	auto sp = Sprite::create("image/screen/unitSelect/name.png");
	sp->setPosition(pos);
	return sp;
}
LabelTTF* BatleResultScene::createUniNameLabel(Vec2 pos)
{
	auto lb = LabelTTF::create("", "fonts/Marker Felt.ttf", 20);
	lb->setPosition(pos);
	lb->setHorizontalAlignment(TextHAlignment::CENTER);
	lb->setColor(Color3B::BLACK);
	return lb;
}


