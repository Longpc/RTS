#ifndef LAYER_BASE_H__
#define LAYER_BASE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "extensions/cocos-ext.h"
#include "base/Define.h"
#include <stdlib.h>
#include <math.h>

USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
using namespace std;

class LayerBase :public Layer
{
public:
	bool init();
	virtual void onEnter();
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onExit();
	virtual void onEnterTransitionDidFinish();

	virtual void onBackButtonClick(Ref *pSender);
	virtual void removeLayerBase();

	Button *_backButton;
	Sprite *_pageTitleSprite;

	LayerColor *_backGround;
	Sprite *_bgImage;
	Size _visibleSize;
	LabelTTF *_defaultLabel;
	Menu *_menu;
	MenuItemImage * _item1;

	UserInfo _curUserInfo;
	Sprite *_usernameBg;

	bool _disableFlag;

protected:
private:
	
};

#endif