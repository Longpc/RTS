#ifndef SKILL_DETAIL_DIALOG
#define SKILL_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"

#include "Model/data/UserSkill.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;

class SkillDetailDialog :public LayerBase
{
public:
	static SkillDetailDialog* create(UserSkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(UserSkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *statusTab;
	UserSkillInfo _skillInfo;
	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();
	virtual void displayUnitInfo(Sprite *parent);

};



#endif