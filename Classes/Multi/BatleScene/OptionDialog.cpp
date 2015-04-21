#pragma execution_character_set("utf-8")
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
	if (!DialogBase::init(dCall, ccCall))
	{
		return false;
	}
	getCloseButton()->setVisible(false);
	selectMode = UserDefault::getInstance()->getIntegerForKey(MOVE_KEY);
	circleType = UserDefault::getInstance()->getIntegerForKey(MOVE_CIRCLE_TYPE);
	circleProperty = UserDefault::getInstance()->getIntegerForKey(NOVE_CIRCLE_PROPERTY);
	log("selectMode: %d", selectMode);
	backGround = getBackGroundSprite();
	Size size = backGround->getContentSize();

	////////////////////////////////////////////////////////////////////////////
	// MOVE_AUTO 
	////////////////////////////////////////////////////////////////////////////
	_mode1 = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");
	_mode1->setPosition(Vec2(size.width * 1 / 6, size.height * 5 / 6)); // 2/3
	_mode1->addEventListener(CC_CALLBACK_2(OptionDialog::touchMode1Callback, this));

	auto optionText1 = Label::createWithSystemFont("自動敵でタッチ点まで移動する", JAPANESE_FONT_1_BOLD, 30);
	optionText1->setColor(Color3B::BLACK);
	optionText1->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText1->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText1->setPosition(_mode1->getPosition() + Vec2(50, 0));

	////////////////////////////////////////////////////////////////////////////
	// MOVE_MANUAL
	////////////////////////////////////////////////////////////////////////////
	_mode2 = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");
	_mode2->setPosition(Vec2(size.width * 1 / 6, size.height * 2 / 3)); // 1/3
	_mode2->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove2Callback, this));

	auto optionText2 = Label::createWithSystemFont("タッチの方向で移動する", JAPANESE_FONT_1_BOLD, 30);
	optionText2->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText2->setColor(Color3B::BLACK);
	optionText2->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText2->setPosition(_mode2->getPosition() + Vec2(50, 0));

	////////////////////////////////////////////////////////////////////////////
	// MOVE_CIRCLE
	////////////////////////////////////////////////////////////////////////////
	_mode3 = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");
	_mode3->setPosition(Vec2(size.width * 1 / 6, size.height * 1 / 2));
	_mode3->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove3Callback, this));

	auto optionText3 = Label::createWithSystemFont("丸でユニットを移動する", JAPANESE_FONT_1_BOLD, 30);
	optionText3->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText3->setColor(Color3B::BLACK);
	optionText3->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText3->setPosition(_mode3->getPosition() + Vec2(50, 0));

	//////////////////////
	// LEFT
	//////////////////////
	_mode3Left = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");

	_mode3Left->setPosition(Vec2(size.width * 1 / 4, size.height * 1 / 3));
	_mode3Left->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove3LeftCallback, this));

	optionText3Left = Label::createWithSystemFont("左", JAPANESE_FONT_1_BOLD, 30);

	optionText3Left->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText3Left->setColor(Color3B::BLACK);
	optionText3Left->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText3Left->setPosition(_mode3Left->getPosition() + Vec2(20, 0));

	//////////////////////
	// RIGHT
	//////////////////////
	_mode3Right = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");

	_mode3Right->setPosition(Vec2(size.width * 1 / 2, size.height * 1 / 3));
	_mode3Right->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove3RightCallback, this));

	optionText3Right = Label::createWithSystemFont("右", JAPANESE_FONT_1_BOLD, 30);

	optionText3Right->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText3Right->setColor(Color3B::BLACK);
	optionText3Right->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText3Right->setPosition(_mode3Right->getPosition() + Vec2(20, 0));

	//////////////////////
	// TIME
	//////////////////////
	_mode3Time = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");

	_mode3Time->setPosition(Vec2(size.width * 1 / 4, size.height * 1 / 6));
	_mode3Time->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove3TimeCallback, this));

	optionText3Time = Label::createWithSystemFont("時間", JAPANESE_FONT_1_BOLD, 30);

	optionText3Time->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText3Time->setColor(Color3B::BLACK);
	optionText3Time->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText3Time->setPosition(_mode3Time->getPosition() + Vec2(20, 0));

	//////////////////////
	// DISTANCE
	//////////////////////
	_mode3Distance = CheckBox::create("image/navigator/checkbox/check_box_normal.png",
		"image/navigator/checkbox/check_box_normal_press.png",
		"image/navigator/checkbox/check_box_active.png",
		"image/navigator/checkbox/check_box_normal_disable.png",
		"image/navigator/checkbox/check_box_active_disable.png");

	_mode3Distance->setPosition(Vec2(size.width * 1 / 2, size.height * 1 / 6));
	_mode3Distance->addEventListener(CC_CALLBACK_2(OptionDialog::touchMove3DistanceCallback, this));

	optionText3Distance = Label::createWithSystemFont("距離", JAPANESE_FONT_1_BOLD, 30);

	optionText3Distance->setHorizontalAlignment(TextHAlignment::LEFT);
	optionText3Distance->setColor(Color3B::BLACK);
	optionText3Distance->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
	optionText3Distance->setPosition(_mode3Distance->getPosition() + Vec2(20, 0));


	switch (selectMode)
	{
	case MOVE_AUTO:
		_mode1->setSelected(true);
		_mode1->setTouchEnabled(false);

		_mode3Left->setVisible(false);
		_mode3Right->setVisible(false);
		optionText3Left->setVisible(false);
		optionText3Right->setVisible(false);

		_mode3Time->setVisible(false);
		_mode3Distance->setVisible(false);
		optionText3Time->setVisible(false);
		optionText3Distance->setVisible(false);

		break;
	case MOVE_MANUAL:
		_mode2->setSelected(true);
		_mode2->setTouchEnabled(false);

		_mode3Left->setVisible(false);
		_mode3Right->setVisible(false);
		optionText3Left->setVisible(false);
		optionText3Right->setVisible(false);

		_mode3Time->setVisible(false);
		_mode3Distance->setVisible(false);
		optionText3Time->setVisible(false);
		optionText3Distance->setVisible(false);

		break;
	case MOVE_CIRCLE:
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);

		_mode3Left->setVisible(true);
		_mode3Right->setVisible(true);

		_mode3Time->setVisible(true);
		_mode3Distance->setVisible(true);


		switch (circleType)
		{
		case MOVE_CIRCLE_LEFT:
			_mode3Left->setSelected(true);
			_mode3Left->setTouchEnabled(false);
			_mode3Right->setSelected(false);
			_mode3Right->setTouchEnabled(true);
			circleType = MOVE_CIRCLE_LEFT;
			break;
		case MOVE_CIRCLE_RIGHT:
			_mode3Left->setSelected(false);
			_mode3Left->setTouchEnabled(true);
			_mode3Right->setSelected(true);
			_mode3Right->setTouchEnabled(false);
			circleType = MOVE_CIRCLE_RIGHT;
			break;
		default:
			break;
		}

		switch (circleProperty)
		{
		case MOVE_CIRCLE_TIME:
			_mode3Time->setSelected(true);
			_mode3Time->setTouchEnabled(false);
			_mode3Distance->setSelected(false);
			_mode3Distance->setTouchEnabled(true);
			circleProperty = MOVE_CIRCLE_TIME;
			break;
		case MOVE_CIRCLE_DISTANCE:
			_mode3Time->setSelected(false);
			_mode3Time->setTouchEnabled(true);
			_mode3Distance->setSelected(true);
			_mode3Distance->setTouchEnabled(false);
			circleProperty = MOVE_CIRCLE_DISTANCE;
			break;
		default:
			break;
		}

		optionText3Left->setVisible(true);
		optionText3Right->setVisible(true);
		optionText3Time->setVisible(true);
		optionText3Distance->setVisible(true);

		break;

	default:
		break;
	}

	backGround->addChild(_mode1);
	backGround->addChild(optionText1);
	backGround->addChild(_mode2);
	backGround->addChild(optionText2);
	backGround->addChild(_mode3);
	backGround->addChild(optionText3);

	backGround->addChild(_mode3Left);
	backGround->addChild(optionText3Left);
	backGround->addChild(_mode3Right);
	backGround->addChild(optionText3Right);

	backGround->addChild(_mode3Time);
	backGround->addChild(optionText3Time);
	backGround->addChild(_mode3Distance);
	backGround->addChild(optionText3Distance);

	return true;
}

////////////////////////////////////////////////////////////////////////////
// SELECT MOVE_TYPE CALLBACK
////////////////////////////////////////////////////////////////////////////

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
		_mode3->setSelected(false);
		_mode3->setTouchEnabled(true);

		_mode3Left->setVisible(false);
		_mode3Right->setVisible(false);
		optionText3Left->setVisible(false);
		optionText3Right->setVisible(false);

		_mode3Time->setVisible(false);
		_mode3Distance->setVisible(false);
		optionText3Time->setVisible(false);
		optionText3Distance->setVisible(false);

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
		_mode3->setSelected(false);
		_mode3->setTouchEnabled(true);

		_mode3Left->setVisible(false);
		_mode3Right->setVisible(false);
		optionText3Left->setVisible(false);
		optionText3Right->setVisible(false);

		_mode3Time->setVisible(false);
		_mode3Distance->setVisible(false);
		optionText3Time->setVisible(false);
		optionText3Distance->setVisible(false);

		selectMode = MOVE_MANUAL;
		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::touchMove3Callback(Ref *pSender, CheckBox::EventType type)
{
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);

		_mode3Left->setVisible(true);
		_mode3Right->setVisible(true);
		_mode3Time->setVisible(true);
		_mode3Distance->setVisible(true);

		optionText3Left->setVisible(true);
		optionText3Right->setVisible(true);
		optionText3Time->setVisible(true);
		optionText3Distance->setVisible(true);

		_mode3Left->setSelected(true);
		_mode3Time->setSelected(true);

		selectMode = MOVE_CIRCLE;
		circleType = MOVE_CIRCLE_LEFT;
		circleProperty = MOVE_CIRCLE_TIME;

		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::touchMove3LeftCallback(Ref *pSender, CheckBox::EventType type){
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);

		_mode3Left->setSelected(true);
		_mode3Left->setTouchEnabled(false);
		_mode3Right->setSelected(false);
		_mode3Right->setTouchEnabled(true);

		optionText3Left->setVisible(true);
		optionText3Right->setVisible(true);

		selectMode = MOVE_CIRCLE;
		circleType = MOVE_CIRCLE_LEFT;

		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}
void OptionDialog::touchMove3RightCallback(Ref *pSender, CheckBox::EventType type){
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);

		_mode3Left->setSelected(false);
		_mode3Left->setTouchEnabled(true);
		_mode3Right->setSelected(true);
		_mode3Right->setTouchEnabled(false);

		optionText3Left->setVisible(true);
		optionText3Right->setVisible(true);

		selectMode = MOVE_CIRCLE;
		circleType = MOVE_CIRCLE_RIGHT;

		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::touchMove3TimeCallback(Ref *pSender, CheckBox::EventType type){
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);

		_mode3Time->setSelected(true);
		_mode3Time->setTouchEnabled(true);
		_mode3Distance->setSelected(false);
		_mode3Distance->setTouchEnabled(true);

		optionText3Time->setVisible(true);
		optionText3Distance->setVisible(true);

		selectMode = MOVE_CIRCLE;
		circleProperty = MOVE_CIRCLE_TIME;

		break;
	}
	case cocos2d::ui::CheckBox::EventType::UNSELECTED:
		break;
	default:
		break;
	}
}

void OptionDialog::touchMove3DistanceCallback(Ref *pSender, CheckBox::EventType type){
	switch (type)
	{
	case cocos2d::ui::CheckBox::EventType::SELECTED:
	{
		_mode3->setSelected(true);
		_mode3->setTouchEnabled(false);
		_mode2->setSelected(false);
		_mode2->setTouchEnabled(true);
		_mode1->setSelected(false);
		_mode1->setTouchEnabled(true);

		_mode3Time->setSelected(false);
		_mode3Time->setTouchEnabled(true);
		_mode3Distance->setEnabled(true);
		_mode3Distance->setTouchEnabled(false);

		optionText3Time->setVisible(true);
		optionText3Distance->setVisible(true);

		selectMode = MOVE_CIRCLE;
		circleProperty = MOVE_CIRCLE_DISTANCE;

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
}

void OptionDialog::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		UserDefault::getInstance()->setIntegerForKey(MOVE_KEY, selectMode);
		if (selectMode == MOVE_CIRCLE)
		{
			UserDefault::getInstance()->setIntegerForKey(MOVE_CIRCLE_TYPE, circleType);
			UserDefault::getInstance()->setIntegerForKey(NOVE_CIRCLE_PROPERTY, circleProperty);
		}

		DialogBase::decideButtonCallback(pSender, type);
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

