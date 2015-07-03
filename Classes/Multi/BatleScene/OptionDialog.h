#ifndef OPTION_DIALOG_H__
#define OPTION_DIALOG_H__

#include "base/dialogBase/DialogBase.h"

class OptionDialog : public DialogBase
{
public:
	static OptionDialog* create(MyTouchEvent decideCallback, MyTouchEvent cancelCallback);
	bool init(MyTouchEvent dCall, MyTouchEvent ccCall);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *backGround;
	int _screenOrien;
	int selectMode;
	int circleType;
	int circleProperty;
	CheckBox * _verticalModeCb;
	CheckBox *_horizontalModeCb;

	// Auto
	CheckBox *_mode1;
	// Manual
	CheckBox *_mode2;
	// Cirlce
	CheckBox *_mode3;
	CheckBox *_mode3Left;
	CheckBox *_mode3Right;
	CheckBox *_mode3Time;
	CheckBox *_mode3Distance;
	Label* optionText3Left;
	Label* optionText3Right;
	Label* optionText3Time;
	Label* optionText3Distance;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void onExit();
	void touchMode1Callback(Ref *pSender, CheckBox::EventType type);
	void touchMove2Callback(Ref *pSender, CheckBox::EventType type);
	void touchMove3Callback(Ref *pSender, CheckBox::EventType type);
	void touchMove3LeftCallback(Ref *pSender, CheckBox::EventType type);
	void touchMove3RightCallback(Ref *pSender, CheckBox::EventType type);
	void touchMove3TimeCallback(Ref *pSender, CheckBox::EventType type);
	void touchMove3DistanceCallback(Ref *pSender, CheckBox::EventType type);

	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual CheckBox* createCheckBox();

	void horizontalModeCbcallbacl(Ref *pSender, CheckBox::EventType type);
	void verticalModeCbCallback(Ref *pSender, CheckBox::EventType type);
};


#endif