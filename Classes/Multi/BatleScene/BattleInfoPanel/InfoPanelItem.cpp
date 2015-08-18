#pragma execution_character_set("utf-8")
#include "InfoPanelItem.h"

InfoPanelItem* InfoPanelItem::createItem(int teamId)
{
	auto skillShow = new InfoPanelItem();
	if (skillShow) {
		skillShow->init(teamId);
		skillShow->autorelease();
		return skillShow;
	}
	CC_SAFE_DELETE(skillShow);
	return nullptr;
}

bool InfoPanelItem::init(int teamId)
{
	if (!Node::init()) {
		return false;
	}
	Color4F color = Color4F::WHITE;
	if (teamId == TEAM_FLG_BLUE) 
	{
		color = Color4F(0, 0, 1, 0.35f);
	}
	else 
	{
		color = Color4F(1, 0, 0, 0.35f);
	}
	auto backGround = DrawNode::create();
	backGround->drawSolidRect(Vec2(-100, -25), Vec2(100, 25), color);
	this->addChild(backGround);
	_mainLabel = Label::createWithSystemFont("", JAPANESE_FONT_1_BOLD, 20);
	_mainLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	this->addChild(_mainLabel);
	return true;
}
void InfoPanelItem::onEnter()
{
	Node::onEnter();
	//this->runAction(Sequence::create(DelayTime::create(SHOWTIME), RemoveSelf::create(true), nullptr));
}

void InfoPanelItem::selfRemove(bool cleanFlg)
{
	//this->runAction(Sequence::create(MoveBy::create(0.15f, Vec2(200, 0)), RemoveSelf::create(true), nullptr));
	this->removeFromParentAndCleanup(cleanFlg);
}

InfoPanelItem::InfoPanelItem()
{

}

InfoPanelItem::~InfoPanelItem()
{

}

void InfoPanelItem::setItemType(int type)
{

}

void InfoPanelItem::setPrimaryUnitIcon(string path)
{
	auto unitIcon = Sprite::create(path);
	unitIcon->setPosition(Vec2(-100, 0));
	unitIcon->setScale(1.8f);
	this->addChild(unitIcon, 1);
}

void InfoPanelItem::setSecondUnitIcon(string path)
{
	auto unitIcon = Sprite::create(path);
	unitIcon->setPosition(Vec2(-35, 0));
	unitIcon->setScale(1.8f);
	this->addChild(unitIcon, 1);
}

void InfoPanelItem::setNeutralType(int type) 
{
	string path = "";
	if (type == NEUTRAL_TYPE::CANNON)
	{
		path = "image/screen/battle/icon/cannon.png";
	}
	if (type == NEUTRAL_TYPE::TOWER)
	{
		path = "image/screen/battle/icon/tower.png";
	}
	if (type == NEUTRAL_TYPE::UNIT)
	{
		path = "image/screen/battle/icon/unit.png";
	}
	auto icon = Sprite::create(path.c_str());
	icon->setPosition(Vec2(0, 0));
	icon->setScale(0.8f);
	this->addChild(icon, 1);
}

void InfoPanelItem::setLabelContent(string content)
{
	_mainLabel->setString(content);
}


