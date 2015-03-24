#ifndef OPTION_DIALOG_H__
#define OPTION_DIALOG_H__

#include "base/dialogBase/DialogBase.h"

#define MOVE_AUTO 1
#define MOVE_MANUAL 2
#define MOVE_KEY "MOVE_MODE"

class OptionDialog : public DialogBase
{
public:
	static OptionDialog* create(MyTouchEvent decideCallback, MyTouchEvent cancelCallback);
	bool init(MyTouchEvent dCall, MyTouchEvent ccCall);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *backGround;
	int selectMode;
	CheckBox *_mode1;
	CheckBox *_mode2;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void onExit();
	void touchMode1Callback(Ref *pSender, CheckBox::EventType type);
	void touchMove2Callback(Ref *pSender, CheckBox::EventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);

	
};


#endif