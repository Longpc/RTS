#pragma execution_character_set("utf-8")

#include "LayerBase.h"

bool LayerBase::init()
{
	if (!Layer::init()) {
		return false;
	}
	_visibleSize = Director::getInstance()->getWinSize();
	_backGround = LayerColor::create(Color4B(0, 0, 0, 255));
	addChild(_backGround,-100);

	_bgImage = Sprite::create("image/screen/bg.png");
	_bgImage->setScale(MAX(float((_visibleSize.width) / _bgImage->getContentSize().width), float((_visibleSize.height) / _bgImage->getContentSize().height)));
	_bgImage->setPosition(_visibleSize / 2);
	addChild(_bgImage);

	_pageTitleSprite = Sprite::create("image/navigator/title.png");
	_pageTitleSprite->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - _pageTitleSprite->getContentSize().height / 2));
	addChild(_pageTitleSprite);

	_item1 = MenuItemImage::create("image/button/new/button_return.png", "", "image/button/new/button_return_diasable.png", CC_CALLBACK_1(LayerBase::onBackButtonClick, this));
	_menu = Menu::create(_item1, nullptr);
	_item1->setPosition(Vec2( _item1->getContentSize().width/2, _visibleSize.height - _item1->getContentSize().height/2));
	_menu->setPosition(Vec2::ZERO);

	_defaultLabel = Label::createWithSystemFont("", JAPANESE_FONT_1_HEAVY, 30);
	_defaultLabel->setPosition(Vec2(_visibleSize.width / 2, _visibleSize.height - 40));
	_defaultLabel->setColor(Color3B::BLACK);
	addChild(_defaultLabel);

	addChild(_menu,1);

	_usernameBg = Sprite::create("image/navigator/user_name.png");
	_usernameBg->setScaleX(0.5f);
	_usernameBg->setScaleY(0.8f);
	_usernameBg->setPosition(Vec2(_visibleSize.width - _usernameBg->getContentSize().width*0.5f / 2, _visibleSize.height - _usernameBg->getContentSize().height*0.8f / 2));
	addChild(_usernameBg);

	_curUserInfo.name = "Long";

	_userNameLabel = Label::createWithSystemFont(_curUserInfo.name, JAPANESE_FONT_1_BOLD, 30);
	_userNameLabel->setPosition(Vec2(50,_usernameBg->getContentSize().height/2));
	_userNameLabel->setColor(Color3B::BLACK);
	_usernameBg->addChild(_userNameLabel,1);

	this->setSwallowsTouches(true);

	return true;
}

void LayerBase::onEnter()
{
	Layer::onEnter();

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->setSwallowTouches(false);
	touchListener->onTouchBegan = CC_CALLBACK_2(LayerBase::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(LayerBase::onTouchEnded, this);
	touchListener->onTouchMoved = CC_CALLBACK_2(LayerBase::onTouchMoved, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	

}

void LayerBase::onExit()
{
	Layer::onExit();
}

void LayerBase::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void LayerBase::onBackButtonClick(Ref *pSender)
{
	removeLayerBase();
}

void LayerBase::removeLayerBase()
{
	this->runAction(RemoveSelf::create(false));
}

bool LayerBase::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void LayerBase::onTouchEnded(Touch *touch, Event *unused_event)
{

}

void LayerBase::onTouchMoved(Touch *touch, Event *unused_event)
{

}
