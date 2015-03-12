#ifndef CLIPING_BUTTON
#define CLIPING_BUTTON

#include "base/Define.h"
#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
using namespace std;

class ClipingButtonBase :public Node
{
public:
	ClipingButtonBase* create(/*background image, clipnode hold image, click event*/);
	bool init();
	void addTouchEvent();
	void touchEventCallback(Ref *pSender, Widget::TouchEventType type);
private:
	ClippingNode *_clipNode;
	Button *baseSlot;
	Button *bolderbackground;

};



#endif
