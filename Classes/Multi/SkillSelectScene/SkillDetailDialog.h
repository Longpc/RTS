#ifndef SKILL_DETAIL_DIALOG
#define SKILL_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"
typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;

class SkillDetailDialog :public LayerBase
{
public:
	static SkillDetailDialog* create(SkillInfoNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(SkillInfoNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *statusTab;
	SkillInfoNew _skillInfo;
	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();
	virtual void displayUnitInfo(Sprite *parent);

};



#endif