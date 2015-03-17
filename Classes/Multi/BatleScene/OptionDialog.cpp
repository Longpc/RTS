#include "OptionDialog.h"

OptionDialog* OptionDialog::create(MyTouchEvent decideCallback, MyTouchEvent cancelCallback)
{
	OptionDialog *d = new OptionDialog();
	if (d && d->init(decideCallback, cancelCallback)) {
		d->autorelease();
		return d;
	}
	CC_SAFE_DELETE(d);
	return NULL;
}

bool OptionDialog::init(MyTouchEvent dCall, MyTouchEvent ccCall)
{
	if (!DialogBase::init(dCall,ccCall))
	{
		return false;
	}
	getCloseButton()->setVisible(false);
	selectMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);
		backGround = getBackGroundSprite();
	Size size = backGround->getContentSize();
	_mode1 = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");
	_mode1->setPosition(Vec2(size.width * 1 / 6, size.height * 2 / 3));
	_mode1->addEventListener(CC_CALLBACK_2(OptionDialog::touchMode1Callback, this));

	_mode2 = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");
	_mode2->setPosition(Vec2(size.width * 1 / 6, size.height * 1 / 3));
	_mode2->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove2Callback, this));

	switch (selectMode)
	{
	case MOVE_AUTO:
		_mode1->setSelected(true);
		_mode1->setTouchEnabled(false);
		break;
	case MOVE_MANUAL:
		_mode2->setSelected(true);
		_mode2->setTouchEnabled(false);
		break;
	default:
		break;
	}

	backGround->addChild(_mode1);
	backGround->addChild(_mode2);

	return true;
}

void OptionDialog::touchMode1Callback(Ref *pSender, CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode1->setSelected(true);
		_mode1->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		selectMode = MOVE_AUTO;
		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::touchMove2Callback(Ref *pSender, CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode2->setSelected(true);
		_mode2->setTouchEnabled(false);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);
		selectMode = MOVE_MANUAL;
		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::onExit()
{
	DialogBase::onExit();

	UserDefault::getInstance()->setIntegerForKey(MOVE_KEY, selectMode);
}

