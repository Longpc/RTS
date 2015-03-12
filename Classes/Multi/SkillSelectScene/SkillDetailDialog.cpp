#include "SkillDetailDialog.h"
SkillDetailDialog* SkillDetailDialog::create(SkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	SkillDetailDialog *p = new SkillDetailDialog();
	if (p && p->init(unit, decideCallback, ccelCallback)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

bool SkillDetailDialog::init(SkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
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
	auto foreground = Sprite::create("image/navigator/unit_frame.png");
	foreground->setPosition(_visibleSize / 2);
	addChild(foreground);

	auto closeButton = Button::create();
	closeButton->loadTextureNormal("image/button/new/button_close.png");
	closeButton->setPosition(Vec2(foreground->getContentSize().width -closeButton->getContentSize().width/2 - 15, foreground->getContentSize().height - closeButton->getContentSize().height/2 - 15));
	closeButton->addTouchEventListener(CC_CALLBACK_2(SkillDetailDialog::closeButtonCallback, this));
	foreground->addChild(closeButton, 11);

	auto decideButton = Button::create();
	decideButton->loadTextureNormal("image/button/new/button_decide.png");
	decideButton->setPosition(Vec2(foreground->getContentSize().width - decideButton->getContentSize().width/2 - 20, 50));
	decideButton->addTouchEventListener(CC_CALLBACK_2(SkillDetailDialog::decideButtonCallback, this));
	foreground->addChild(decideButton, 11);

	displayUnitInfo(foreground);

	return true;
}

void SkillDetailDialog::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_decideCallback != nullptr) {
		_decideCallback(pSender, type);
	}

}

void SkillDetailDialog::closeButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_ccCallback != nullptr) {
		_ccCallback(pSender, type);
	}
}

void SkillDetailDialog::closeDialog()
{
	this->runAction(RemoveSelf::create(false));
}

void SkillDetailDialog::displayUnitInfo(Sprite *parent)
{
	auto image = Sprite::create(_unitInfo._imagePath);
	image->setScale(1.5);
	image->setPosition(Vec2(image->getContentSize().width / 2 + 80, parent->getContentSize().height / 2));
	parent->addChild(image, 10);


	std::stringstream info;
	info << "Name: " << _unitInfo._name << "\nDescription: " << _unitInfo._description << "\nId: " << _unitInfo._skillId;
	auto label = LabelTTF::create(info.str().c_str(), "", 25);
	label->setColor(Color3B::BLACK);
	label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	label->setHorizontalAlignment(TextHAlignment::LEFT);
	label->setPosition(Vec2(parent->getContentSize().width/2 - 70, parent->getContentSize().height - 100));
	parent->addChild(label);

}