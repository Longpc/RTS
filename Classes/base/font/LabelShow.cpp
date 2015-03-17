#include "LabelShow.h"

USING_NS_CC;


LabelShow* LabelShow::createLabel(int value, int typeFont)
{
	LabelShow* label = new LabelShow();
	if (label && label->init(value, typeFont))
	{
		//log("Co vao day ko vay");
		label->autorelease();
		return label;
	}
	CC_SAFE_DELETE(label);
	return nullptr;
}

bool LabelShow::init(int value , int typeFont)
{
	if (!Sprite::init())
	{
		return false;
	}

	_typeFont = typeFont;
	_value = value;

	//log("%d %d", value, typeFont);

	std::stringstream str;
	str << _value;

	std::string valueConvert = str.str();

	//log("%s ", valueConvert);

	if (_typeFont == 1)
	{
		auto label = Label::createWithBMFont("font/fontYellow.fnt", valueConvert, TextHAlignment::CENTER);
		label->setPosition(Vec2::ZERO);

		this->addChild(label);

	}
	if (_typeFont == 2)
	{
		auto label = Label::createWithBMFont("font/fontGreen.fnt", valueConvert, TextHAlignment::CENTER);
		label->setPosition(Vec2::ZERO);

		this->addChild(label);
	}

	return true;
}

void LabelShow::onEnter()
{
	Sprite::onEnter();
}