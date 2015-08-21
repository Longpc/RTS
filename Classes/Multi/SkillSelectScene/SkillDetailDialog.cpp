#pragma execution_character_set("utf-8")
#include "SkillDetailDialog.h"
SkillDetailDialog* SkillDetailDialog::create(UserSkillInfo skill, MyTouchEventVoid decideCallback, MyTouchEventVoid ccelCallback)
{
	SkillDetailDialog *p = new SkillDetailDialog();
	if (p && p->init(skill, decideCallback, ccelCallback)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

bool SkillDetailDialog::init(UserSkillInfo unit, MyTouchEventVoid decideCallback, MyTouchEventVoid ccelCallback)
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
	_skillInfo = unit;

	//auto background = LayerColor::create(Color4B(0, 0, 0, 150));
	//addChild(background, -1);
	_backGround->setOpacity(150);
	auto foreground = Sprite::create("image/dialog/unitDetail/select_scene_detail.png");
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
	this->setSwallowsTouches(true);

	return true;
}

void SkillDetailDialog::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		closeDialog();
		if (_decideCallback != nullptr) _decideCallback();
		break;
	}
	default:
		break;
	}
	

}

void SkillDetailDialog::closeButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		closeDialog();
		if (_ccCallback != nullptr) _ccCallback();
		break;
	}
	default:
		break;
	}
	
}

void SkillDetailDialog::closeDialog()
{
	this->runAction(RemoveSelf::create(false));
}

void SkillDetailDialog::displayUnitInfo(Sprite *parent)
{
	auto image = Sprite::create(_skillInfo.skill_icon_path);
	image->setScale(1.5);
	image->getTexture()->setAliasTexParameters();
	image->setPosition(Vec2(image->getContentSize().width / 2 + 100, parent->getContentSize().height / 2));
	parent->addChild(image, 10);


	std::stringstream info;
	info << "" << _skillInfo.name << "\n\n" << _skillInfo.skill_des << "\n必要MP        :      " << _skillInfo.mp_cost <<"\n復元時間      :      "<<_skillInfo.cooldown_time <<"秒";
	auto label = Label::createWithSystemFont(info.str().c_str(), JAPANESE_FONT_1_BOLD, 27,Size(parent->getContentSize().width/2,300));
	label->setColor(Color3B::BLACK);
	label->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	label->setHorizontalAlignment(TextHAlignment::LEFT);
	label->setPosition(Vec2(parent->getContentSize().width/2 - 75, parent->getContentSize().height - 110));
	parent->addChild(label);

}