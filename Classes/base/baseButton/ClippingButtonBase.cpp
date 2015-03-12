#include "ClippingButtonBase.h"

ClipingButtonBase* ClipingButtonBase::create(/*background image, clipnode hold image, click event*/)
{
	ClipingButtonBase *nd = new ClipingButtonBase();
	if (nd && nd->init()) {
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

	return true;
}

void ClipingButtonBase::addTouchEvent()
{

}

void ClipingButtonBase::touchEventCallback(Ref *pSender, Widget::TouchEventType type)
{

}
