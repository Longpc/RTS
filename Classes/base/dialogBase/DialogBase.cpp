#include "base/dialogBase/DialogBase.h"

bool DialogBase::init()
{
	if (!LayerBase::init()) {
		return false;
	}

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
	return false;
}


