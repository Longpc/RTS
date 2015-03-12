#include "UnitDetailDialog.h"

UnitDetailDialog* UnitDetailDialog::create(UnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	UnitDetailDialog *p = new UnitDetailDialog();
	if (p && p->init(unit, decideCallback, ccelCallback)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

bool UnitDetailDialog::init(UnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	if (!LayerBase::init()) {
		return false;
	}
	_menu->setVisible(false);
	_bgImage->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_decideCallback = decideCallback;
	_ccCallback = ccelCallback;
	_unitInfo = unit;

	//auto background = LayerColor::create(Color4B(0, 0, 0, 150));
	//addChild(background, -1);
	_backGround->setOpacity(150);
	auto foreground = Sprite::create("image/dialog/unitDetail/select_scene_detail.png");
	foreground->setPosition(_visibleSize / 2);
	addChild(foreground);

	auto closeButton = Button::create();
	closeButton->loadTextureNormal("image/button/new/button_close.png");
	closeButton->setPosition(Vec2(foreground->getContentSize().width - closeButton->getContentSize().width + 20, foreground->getContentSize().height - closeButton->getContentSize().height));
	closeButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::closeButtonCallback, this));
	foreground->addChild(closeButton,11);

	auto decideButton = Button::create();
	decideButton->loadTextureNormal("image/button/new/button_decide.png");
	decideButton->setPosition(Vec2(foreground->getContentSize().width - decideButton->getContentSize().width/2 - 15, decideButton->getContentSize().height));
	decideButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::decideButtonCallback, this));
	foreground->addChild(decideButton,11);

	displayUnitInfo(foreground);

	return true;
}

void UnitDetailDialog::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_decideCallback != nullptr) {
		_decideCallback(pSender, type);
	}

}

void UnitDetailDialog::closeButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_ccCallback != nullptr) {
		_ccCallback(pSender, type);
	}
}

void UnitDetailDialog::closeDialog()
{
	this->runAction(RemoveSelf::create(false));
}

void UnitDetailDialog::displayUnitInfo(Sprite *parent)
{
	auto image = Sprite::create(_unitInfo._imagePath);
	image->setPosition(Vec2(image->getContentSize().width / 2+80, parent->getContentSize().height / 2));
	parent->addChild(image, 10);
	image->setScale(1.5);

	std::stringstream info;
	info << "Name: " <<_unitInfo._name  << "\nAttack: "<<_unitInfo._attack << "\nDefense: "<<_unitInfo._defence<<"\nSpeed: "<<_unitInfo._hp;
	statusLabel = LabelTTF::create(info.str().c_str(), "", 25);
	statusLabel->setColor(Color3B::BLACK);
	statusLabel->setPosition(Vec2(parent->getContentSize().width/2 - 70, parent->getContentSize().height - 100));
	statusLabel->setHorizontalAlignment(TextHAlignment::LEFT);
	statusLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	parent->addChild(statusLabel);

	std::stringstream skill;
	skill << "Skill: ABC \n\nEffect ...... " << _unitInfo._name ;
	skillLabel = LabelTTF::create(skill.str().c_str(), "", 25);
	skillLabel->setColor(Color3B::BLACK);
	skillLabel->setPosition(Vec2(parent->getContentSize().width/2 - 70, parent->getContentSize().height - 100));
	skillLabel->setHorizontalAlignment(TextHAlignment::LEFT);
	skillLabel->setVisible(false);
	skillLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	parent->addChild(skillLabel);


	statusButton = Button::create();
	statusButton->loadTextureNormal("image/dialog/unitDetail/status_s.png");
	statusButton->setPosition(Vec2(parent->getContentSize().width/2 - 10,parent->getContentSize().height - 50));
	statusButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::statusButtonCallback, this));
	parent->addChild(statusButton,10);

	skillButton = Button::create();
	skillButton->loadTextureNormal("image/dialog/unitDetail/skill.png");
	skillButton->setPosition(statusButton->getPosition() + Vec2(skillButton->getContentSize().width,0));
	skillButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::skillButonCallback, this));
	parent->addChild(skillButton,10);
}

void UnitDetailDialog::statusButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusLabel->setVisible(true);
		skillLabel->setVisible(false);
		statusButton->loadTextureNormal("image/dialog/unitDetail/status_s.png");
		skillButton->loadTextureNormal("image/dialog/unitDetail/skill.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void UnitDetailDialog::skillButonCallback(Ref *pSEnder, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusLabel->setVisible(false);
		skillLabel->setVisible(true);
		statusButton->loadTextureNormal("image/dialog/unitDetail/status.png");
		skillButton->loadTextureNormal("image/dialog/unitDetail/skill_s.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}
