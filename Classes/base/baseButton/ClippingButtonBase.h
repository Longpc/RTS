#ifndef CLIPING_BUTTON_H__
#define CLIPING_BUTTON_H__

#include "base/Define.h"
#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
using namespace std;

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;
class ClipingButtonBase :public Node
{
public:
	ClipingButtonBase();
	static ClipingButtonBase* create(const string& maskImage, const string& normalBackground, const string& selectedBackground);
	bool init();
	void addTouchEvent(MyTouchEvent evt);
	void setMashSpriteImagePath(const string& imagePath);
	void setNormalBackGroundImagePath(const string& imagePath);
	void setSelectedBackGroundImagePath(const string& imagePath);
	Button* getClickableButton();
	Button* getBackgroundButton();
	void setSelected(bool selectedFlg);
private:
	ClippingNode *_clipNode;
	Button *_clickableButton;
	Button *_backGround;
	MyTouchEvent _clickEvent;
	string _normal;
	string _selected;

	virtual void clickEventCallback(Ref *pSender, Widget::TouchEventType type);

};





#endif
