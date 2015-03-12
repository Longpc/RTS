
#include "ModeSelectScene.h"

Scene * ModeSelectScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ModeSelectScene::create();
	
	scene->addChild(layer);

	return scene;
}

bool ModeSelectScene::init()
{
	if (!LayerBase::init()) {
		return false;
	}
	_item1->setEnabled(false);
	_usernameBg->setVisible(false);

	String *str = String::createWithFormat("Please select game mode");
	if (_defaultLabel != nullptr) {
		_defaultLabel->setString(str->getCString());
	}

	Sprite *unit = Sprite::create("image/unit/navi.png");
	unit->setPosition(Vec2(_visibleSize.width* 1/4, _visibleSize.height/2 - 20));
	addChild(unit);

	Button *multiButton = Button::create();
	multiButton->loadTextureNormal("image/button/new/multi.png");
	multiButton->setTouchEnabled(true);
	multiButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::multiButtonCallback, this));
	multiButton->setPosition(Vec2(_visibleSize.width * 3 / 4, _visibleSize.height *2/ 3));
	addChild(multiButton);

	Button *soloButton = Button::create();
	soloButton->loadTextureNormal("image/button/new/solo.png");
	soloButton->setTouchEnabled(true);
	soloButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::soloButtonCallback, this));
	soloButton->setPosition(Vec2(_visibleSize.width * 3 / 4, _visibleSize.height *1/ 3));
	addChild(soloButton);

	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(ModeSelectScene::testDialog, this));
	addChild(nextButton, 10);

	return true;
}



void ModeSelectScene::multiButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onDisableTouch) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		if (checkRoomMember() == true) {
			Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiTeamSelectScene::createScene()));
		}
		else {
			//WARNING LAYER
			auto dialog = MemberFullDialog::create();
			getParent()->addChild(dialog);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void ModeSelectScene::soloButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	if (_onDisableTouch) return;
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		Director::getInstance()->replaceScene(TransitionMoveInR::create(SCREEN_TRANSI_DELAY, MultiUnitSelectScene::createScene(1,SOLO_MODE)));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
bool ModeSelectScene::checkRoomMember()
{
	return _onAccess;
}

void ModeSelectScene::testDialog(Ref *pSender, Widget::TouchEventType type)
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
