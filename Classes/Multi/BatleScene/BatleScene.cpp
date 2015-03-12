#include "BatleScene.h"

Scene* BatleScene::createScene()
{
	auto scene = Scene::create();
	auto lay = BatleScene::create();
	
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

	scheduleUpdate();
}

void BatleScene::update(float delta)
{

}

void BatleScene::updateTime()
{

}

void BatleScene::createContent()
{
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


	_timeViewLabel = Label::create("10:00:11", "", 25);
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
	_skill2Button->addTouchEventListener(CC_CALLBACK_2(BatleScene::skill4ButtonCallback, this));
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
}

void BatleScene::skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

void BatleScene::skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}

bool BatleScene::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void BatleScene::onTouchMoved(Touch *touch, Event *unused_event)
{

}

void BatleScene::onTouchEnded(Touch *touch, Event *unused_event)
{

}

void BatleScene::menuButtonCallback(Ref *pSender, Widget::TouchEventType type)
{

}




