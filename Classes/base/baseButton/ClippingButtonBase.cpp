#include "ClippingButtonBase.h"

ClipingButtonBase* ClipingButtonBase::create(const string& maskImage, const string& normalBackground, const string& selectedBackground)
{
	ClipingButtonBase *nd = new ClipingButtonBase();
	if (nd && nd->init()) {
		nd->setMashSpriteImagePath(maskImage);
		nd->setNormalBackGroundImagePath(normalBackground);
		nd->setSelectedBackGroundImagePath(selectedBackground);
		nd->autorelease();
		return nd;
	}

	CC_SAFE_DELETE(nd);
	return NULL;
}

bool ClipingButtonBase::init()
{
	if (!Node::init()) {
		return false;
	}

	_backGround = Button::create();
	_backGround->setTouchEnabled(false);
	addChild(_backGround);

	_clipNode = ClippingNode::create();
	_clipNode->setAlphaThreshold(0);
	_clipNode->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	addChild(_clipNode);

	_clickableButton = Button::create();
	_clickableButton->loadTextureNormal("image/screen/unitSelect/slot.png");
	_clickableButton->addTouchEventListener(CC_CALLBACK_2(ClipingButtonBase::clickEventCallback, this));
	_clipNode->addChild(_clickableButton);

	return true;
}

void ClipingButtonBase::addTouchEvent(MyTouchEvent evt)
{
	if (evt != nullptr)
	{
		_clickEvent = evt;
	}
}

void ClipingButtonBase::setMashSpriteImagePath(const string& imagePath)
{
	auto mask = Sprite::create(imagePath);
	_clipNode->setStencil(mask);
}

void ClipingButtonBase::setNormalBackGroundImagePath(const string& imagePath)
{
	if (imagePath.length() > 0)
	{
		_backGround->loadTextureNormal(imagePath);
		_normal = imagePath;
	}
}

void ClipingButtonBase::setSelectedBackGroundImagePath(const string& imagePath)
{
	if (imagePath.length()>0)
	{
		_selected = imagePath;
	}
}

void ClipingButtonBase::clickEventCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
	{
		if (_clickEvent != nullptr)
		{
			_clickEvent(pSender, type);
		}
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
		break;
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

Button* ClipingButtonBase::getBackgroundButton()
{
	return _backGround;
}

Button* ClipingButtonBase::getClickableButton()
{
	return _clickableButton;
}
void ClipingButtonBase::resetClickableButton()
{
	_clickableButton->loadTextureNormal("image/screen/unitSelect/slot.png");
}

void ClipingButtonBase::setSelected(bool selectedFlg)
{
	if (selectedFlg) {
		_backGround->loadTextureNormal(_selected);
	}
	else {
		_backGround->loadTextureNormal(_normal);
	}
}

ClipingButtonBase::ClipingButtonBase() :_normal(""), _selected(""), _clickableButton(nullptr), _clipNode(nullptr), _clickEvent(nullptr), _backGround(nullptr)
{

}


