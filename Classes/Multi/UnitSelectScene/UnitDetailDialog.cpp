#pragma execution_character_set("utf-8")
#include "UnitDetailDialog.h"

UnitDetailDialog* UnitDetailDialog::create(UserUnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	UnitDetailDialog *p = new UnitDetailDialog();
	if (p && p->init(unit, decideCallback, ccelCallback)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

bool UnitDetailDialog::init(UserUnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	if (!LayerBase::init()) {
		return false;
	}
	_unitInfo = unit;
	getUnitSkillDataFromDatabase();
	_menu->setVisible(false);
	_bgImage->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_decideCallback = decideCallback;
	_ccCallback = ccelCallback;
	

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

	auto image = Sprite::create(UserUnit::getInstance()->getUnitImageById(_unitInfo.mst_unit_id));
	image->setPosition(Vec2(150, parent->getContentSize().height / 2));
	parent->addChild(image, 10);
	image->setScale(2);

	string dameType = "無";
	switch (_unitInfo.element)
	{
	case 1:
		dameType = "火";
		break;
	case 2:
		dameType = "草";
		break;
	case 3:
		dameType = "水";
	default:
		break;
	}


	std::stringstream info;
	info << "名前:   " << _unitInfo.name << "\nHP:   " << _unitInfo.hp << "\nHP 回復:   " << _unitInfo.hp_heal << "\nMP:   " << _unitInfo.mp  <<"\nMP回復: "<<_unitInfo.mp_heal<< "\n攻撃力:   " << _unitInfo.attack << "\n防御力:   " << _unitInfo.defence;
	info << "\n攻撃範囲:   " << _unitInfo.attack_range << "\n移動スピード: " << _unitInfo.move_speed << "\n属性: " << dameType << "\nタイプ: " << _unitInfo.element;
	statusLabel = Label::create(info.str().c_str(), JAPANESE_FONT_1_BOLD, 25);
	statusLabel->setColor(Color3B::BLACK);
	statusLabel->setHorizontalAlignment(TextHAlignment::LEFT);
	
	skillLabel = Node::create();
	skillLabel->setVisible(false);

	auto backGroundSize = parent->getContentSize();

	for (int i = 0; i < _allUnitSkill.size(); i++)
	{
		auto sp = Sprite::create(_allUnitSkill[i].skill_icon_path);
		sp->setScale(0.7);
		Vec2 pos = Vec2(0, backGroundSize.height/2 - 100 * i - 250);
		sp->setPosition(pos);
		skillLabel->addChild(sp);
		string content = _allUnitSkill[i].name.append("\n").append(_allUnitSkill[i].skill_des);
		auto lb = Label::create(content.c_str(), JAPANESE_FONT_1_BOLD, 25,Size(400,100));
		lb->setColor(Color3B::BLACK);
		lb->setPosition(pos + Vec2(50, -10));
		lb->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		skillLabel->addChild(lb);
	}

	

	if (statusLabel->getContentSize().height > backGroundSize.height-150)
	{

		auto scroll = extension::ScrollView::create();
		scroll->setViewSize(Size(backGroundSize.width / 2, 300));
		scroll->setPosition(Vec2(backGroundSize.width / 2 - 70, 30));
		scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
		scroll->updateInset();
		scroll->setVisible(true);

		parent->addChild(scroll);
		auto layer = Layer::create();
		layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		layer->setContentSize(Size(backGroundSize.width / 2, statusLabel->getContentSize().height + 50));
		layer->setPosition(Vec2(0, -layer->getContentSize().height / 2));

		auto height = layer->getContentSize().height;

		scroll->setContainer(layer);
		scroll->setContentOffset(scroll->minContainerOffset());
		statusLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		statusLabel->setPosition(Vec2(0, height));
		layer->addChild(statusLabel);
		/*skillLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		
		
		skillLabel->setPosition(Vec2(2, height));
		layer->addChild(skillLabel); */
	}
	else {
		statusLabel->setPosition(Vec2(parent->getContentSize().width / 2 - 70, parent->getContentSize().height - 100));
		statusLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

		parent->addChild(statusLabel);
		
	}
	

	skillLabel->setPosition(Vec2(parent->getContentSize().width / 2 - 70, parent->getContentSize().height - 100));
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
// 	RichText *text1 = RichText::create();
// 	text1->ignoreContentAdaptWithSize(false);
// 	text1->setContentSize(Size(100, 100));
// 	RichElementText *ele1 = RichElementText::create(1, Color3B::RED, 255, "名前: \n", JAPANESE_FONT_1_BOLD, 25);
// 	RichElementText *ele2 = RichElementText::create(2, Color3B::BLACK, 255, _unitInfo.name.c_str(), JAPANESE_FONT_1_BOLD, 25);
// 
// 	text1->pushBackElement(ele1);
// 	text1->pushBackElement(ele2);
// 	parent->addChild(text1);
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

void UnitDetailDialog::getUnitSkillDataFromDatabase()
{
	_allUnitSkill.push_back(UserSkill::getInstance()->getSkillInfoById(_unitInfo.skill1_id));
	_allUnitSkill.push_back(UserSkill::getInstance()->getSkillInfoById(_unitInfo.skill2_id));
}
