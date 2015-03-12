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
	auto nextButton = Button::create();
	nextButton->loadTextureNormal("image/button/new/button_home.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 70, _visibleSize.height - 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::nextButtonCallback, this));
	addChild(nextButton, 10);

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
		Director::getInstance()->replaceScene(TransitionMoveInL::create(1, ModeSelectScene::createScene()));
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
	_slot1 = createSlot(Vec2(_visibleSize.width / 2 - 150, _visibleSize.height - 150));
	_slot2 = createSlot(Vec2(_visibleSize.width / 2, _visibleSize.height - 150));
	_slot3 = createSlot(Vec2(_visibleSize.width / 2 + 150, _visibleSize.height - 150));

	addChild(_slot1);
	addChild(_slot2);
	addChild(_slot3);
	statusTab = Sprite::create("image/navigator/dialog_background.png");
	statusTab->setPosition(Vec2(_visibleSize.width/2,_visibleSize.height/2 - 50));
	addChild(statusTab, 10);

	std::stringstream info;
	info << "Name: TEAM BLUE" << "\nAttack: " << 1000000 << "\nDefense: " << 11110 << "\nSpeed: " << 10;
	auto label = LabelTTF::create(info.str().c_str(), "", 25);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(150, statusTab->getContentSize().height - 100));
	label->setHorizontalAlignment(TextHAlignment::LEFT);
	statusTab->addChild(label);

	skillTab = Sprite::create("image/navigator/dialog_background.png");
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

	statusButton = Button::create();
	statusButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
	statusButton->setPosition(Vec2(statusTab->getPositionX() - statusTab->getContentSize().width / 2 + statusButton->getContentSize().width / 2 + 10, statusTab->getPositionY() - statusTab->getContentSize().height / 2 - 40));
	statusButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::statusButtonCallback, this));
	addChild(statusButton, 10);

	skillButton = Button::create();
	skillButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
	skillButton->setPosition(statusButton->getPosition() + Vec2(skillButton->getContentSize().width, 0));
	skillButton->addTouchEventListener(CC_CALLBACK_2(BatleResultScene::skillButonCallback, this));
	addChild(skillButton, 10);


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
	auto sprite = Sprite::create("image/unit/33.png");

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
		statusButton->loadTextureNormal("image/tab/new/blue_team_tab.png");
		skillButton->loadTextureNormal("image/tab/new/red_tab_disable.png");
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
		statusButton->loadTextureNormal("image/tab/new/blue_team_tab_disable.png");
		skillButton->loadTextureNormal("image/tab/new/red_team_tab.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

