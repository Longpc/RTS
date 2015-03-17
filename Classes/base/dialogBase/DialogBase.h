#ifndef DIALOG_BASE
#define DIALOG_BASE

#include "base/LayerBase.h"
#include "base/Define.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;

class DialogBase :public LayerBase
{
public:
	static DialogBase* create(MyTouchEvent decideCallback, MyTouchEvent ccCallback);
	bool init(MyTouchEvent dCall, MyTouchEvent ccCall);
	virtual Sprite* getBackGroundSprite();
	virtual void setDialogbackGroundImagePath();
	virtual Button* getCloseButton();
	virtual void onExit();
private :
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	int selectType;
	Sprite* _dialogBackground;
	Button* decideButton;
	Button* closeButton;

};


#endif