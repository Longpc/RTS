#ifndef DIALOG_BASE
#define DIALOG_BASE

#include "base/LayerBase.h"
#include "base/Define.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;

class DialogBase :public LayerBase
{
public:

	bool init();
private:
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	Sprite* _dialogBackground;

};


#endif