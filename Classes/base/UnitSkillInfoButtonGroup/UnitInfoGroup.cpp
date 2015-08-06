#include "UnitInfoGroup.h"

UnitInfoGroup* UnitInfoGroup::create()
{
	UnitInfoGroup *group = new UnitInfoGroup();
	if (group && group->init()) {
		group->autorelease();
		return group;
	}
	CC_SAFE_DELETE(group);
	return nullptr;
}

bool UnitInfoGroup::init()
{
	if (!Node::init())
	{
		return false;
	}
	_unitButton = ClipingButtonBase::create("image/navigator/selct_scene_circle.png", "image/screen/unitSelect/inactive.png", "image/screen/unitSelect/active.png");
	this->addChild(_unitButton);
	_unitButton->setPosition(Vec2::ZERO);
	_unitButton->addTouchEvent(CC_CALLBACK_2(UnitInfoGroup::onTouchUnitButton, this));

	_playerSkill1Button = ClipingButtonBase::create("image/screen/skillSelect/slot_rect.png", "image/screen/skillSelect/00_frame_inactive.png", "image/screen/skillSelect/00_frame.png");
	_playerSkill1Button->addTouchEvent(CC_CALLBACK_2(UnitInfoGroup::onTouchSkill1Button, this));
	_playerSkill1Button->setPosition(Vec2(_unitButton->getClickableButton()->getContentSize().width, 10 + _unitButton->getClickableButton()->getContentSize().height / 4));
	_playerSkill1Button->setScale(0.5f);
	this->addChild(_playerSkill1Button);

	_playerSkill2Button = ClipingButtonBase::create("image/screen/skillSelect/slot_rect.png", "image/screen/skillSelect/00_frame_inactive.png", "image/screen/skillSelect/00_frame.png");
	_playerSkill2Button->addTouchEvent(CC_CALLBACK_2(UnitInfoGroup::onTouchSkill2Button, this));
	_playerSkill2Button->setPosition(Vec2(_unitButton->getClickableButton()->getContentSize().width, -10 - _unitButton->getClickableButton()->getContentSize().height / 4));
	_playerSkill2Button->setScale(0.5f);
	this->addChild(_playerSkill2Button);

	auto labelBg = Sprite::create("image/screen/unitSelect/name.png");
	labelBg->setPosition(Vec2(_unitButton->getClickableButton()->getContentSize().width / 3, -(_unitButton->getClickableButton()->getContentSize().height / 2 + 30)));
	labelBg->setScaleX(1.4f);
	this->addChild(labelBg);

	_playerNameLabel = Label::createWithSystemFont("waiting...", JAPANESE_FONT_1_BOLD, 20);
	_playerNameLabel->setPosition(labelBg->getPosition());
	_playerNameLabel->setHorizontalAlignment(TextHAlignment::CENTER);
	_playerNameLabel->setColor(Color3B::BLACK);
	this->addChild(_playerNameLabel);

	return true;
}

ClipingButtonBase* UnitInfoGroup::getUnitButton()
{
	return _unitButton;
}
ClipingButtonBase* UnitInfoGroup::getSkillButon(int index)
{
	switch (index)
	{
	case 1: return _playerSkill1Button;
	case 2: return _playerSkill2Button;
	}
}
Button* UnitInfoGroup::getUnitClickableButton()
{
	return _unitButton->getClickableButton();
}

Button* UnitInfoGroup::getSkillClickableButton(int index)
{
	switch (index)
	{
	case 1: return _playerSkill1Button->getClickableButton();
	case 2: return _playerSkill2Button->getClickableButton();
	}
}

void UnitInfoGroup::setSelected(bool selectedFlg)
{
	_unitButton->setSelected(selectedFlg);
	_unitButton->getClickableButton()->setTouchEnabled(selectedFlg);
	_playerSkill1Button->getClickableButton()->setTouchEnabled(selectedFlg);
	_playerSkill2Button->getClickableButton()->setTouchEnabled(selectedFlg);
}

void UnitInfoGroup::setPlayerNameLabel(string name)
{
	if (name.length() < 1) return _playerNameLabel->setString("waiting...");
	_playerNameLabel->setString(name.c_str());
}

void UnitInfoGroup::setUnitIcon(string imgPath)
{
	this->getUnitClickableButton()->loadTextureNormal(imgPath.c_str());
	this->getUnitClickableButton()->setScale(4.0f);
}

void UnitInfoGroup::setSkillIcon(int buttonIndex, string imgPath)
{
	this->getSkillClickableButton(buttonIndex)->loadTextureNormal(imgPath.c_str());
}

void UnitInfoGroup::onTouchUnitButton(Ref *p, Widget::TouchEventType type)
{
	CC_UNUSED_PARAM(p);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		if (_unitTouchCallback) _unitTouchCallback();
		break;
	}
	default:
		break;
	}
}

void UnitInfoGroup::onTouchSkill1Button(Ref *p, Widget::TouchEventType type)
{
	CC_UNUSED_PARAM(p);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		if (_skill1TouchCallback) _skill1TouchCallback();
		break;
	}
	default:
		break;
	}
}

void UnitInfoGroup::onTouchSkill2Button(Ref *p, Widget::TouchEventType type)
{
	CC_UNUSED_PARAM(p);
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		if (_skill2TouchCallback) _skill2TouchCallback();
		break;
	}
	default:
		break;
	}
}

void UnitInfoGroup::resetDefaultStatus()
{
	this->setSelected(false);
	this->setPlayerNameLabel("");
	this->getUnitButton()->resetClickableButton();
	this->getSkillButon(1)->resetClickableButton();
	this->getSkillButon(2)->resetClickableButton();
}
