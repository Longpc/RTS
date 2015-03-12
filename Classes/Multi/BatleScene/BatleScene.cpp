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
	_defaultLabel->setString("Batle Scene");
	auto nextButton = Button::create();
	nextButton->loadTextureNormal("CloseNormal.png");
	nextButton->setPosition(Vec2(_visibleSize.width - 50, 70));
	nextButton->setTouchEnabled(true);
	nextButton->addTouchEventListener(CC_CALLBACK_2(BatleScene::nextButtonCallback, this));
	addChild(nextButton, 10);
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
		Director::getInstance()->replaceScene(TransitionMoveInR::create(1, BatleResultScene::createScene()));
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;

	default:
		break;
	}
}

