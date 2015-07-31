#include "base/dialogBase/DialogBase.h"
DialogBase* DialogBase::create(MyTouchEvent decideCallback, MyTouchEvent ccCallback)
{
	DialogBase *d = new DialogBase();
	if (d && d->init(decideCallback, ccCallback)) {
		d->autorelease();
		return d;
	}
	CC_SAFE_DELETE(d);
	return NULL;
}

bool DialogBase::init(MyTouchEvent dCall, MyTouchEvent ccCAll)
{
	if (!LayerBase::init()) {
		return false;
	}
	_menu->setVisible(false);
	_bgImage->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);

	_decideCallback = dCall;
	_ccCallback = ccCAll;

	_backGround->setOpacity(150);
	_dialogBackground = Sprite::create("image/navigator/frame.png");
	_dialogBackground->setPosition(_visibleSize / 2);
	addChild(_dialogBackground);

	closeButton = Button::create();
	closeButton->loadTextureNormal("image/button/new/button_close.png");
	closeButton->setPosition(Vec2(_dialogBackground->getContentSize().width - closeButton->getContentSize().width + 20, _dialogBackground->getContentSize().height - closeButton->getContentSize().height));
	closeButton->addTouchEventListener(CC_CALLBACK_2(DialogBase::closeButtonCallback, this));
	_dialogBackground->addChild(closeButton, 11);

	decideButton = Button::create();
	decideButton->loadTextureNormal("image/button/new/button_decide.png");
	decideButton->setPosition(Vec2(_dialogBackground->getContentSize().width - decideButton->getContentSize().width / 2 - 15, decideButton->getContentSize().height));
	decideButton->addTouchEventListener(CC_CALLBACK_2(DialogBase::decideButtonCallback, this));
	_dialogBackground->addChild(decideButton, 11);

	return true;
}

void DialogBase::closeButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		closeDialog();
		if (_ccCallback != nullptr)
		{
			_ccCallback(pSender, type);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void DialogBase::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		closeDialog();
		if (_decideCallback != nullptr) {
			_decideCallback(pSender, type);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void DialogBase::closeDialog()
{
	runAction(RemoveSelf::create(true));
}

bool DialogBase::onTouchBegan(Touch *touch, Event *unused_event)
{
	Rect a = _dialogBackground->getBoundingBox();
	if (a.containsPoint(touch->getLocation()))
	{
		return true;
	}

	closeDialog();
	return true;
}

Sprite* DialogBase::getBackGroundSprite()
{
	return _dialogBackground;
}

void DialogBase::setDialogbackGroundImagePath()
{

}

void DialogBase::onExit()
{
	LayerBase::onExit();
}

Button* DialogBase::getCloseButton()
{
	return closeButton;
}



