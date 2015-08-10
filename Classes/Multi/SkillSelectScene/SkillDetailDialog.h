#ifndef SKILL_DETAIL_DIALOG
#define SKILL_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"

#include "Model/data/UserSkillModel.h"


class SkillDetailDialog :public LayerBase
{
public:
	static SkillDetailDialog* create(UserSkillInfo unit, MyTouchEventVoid decideCallback, MyTouchEventVoid ccelCallback);
	bool init(UserSkillInfo unit, MyTouchEventVoid decideCallback, MyTouchEventVoid ccelCallback);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *statusTab;
	UserSkillInfo _skillInfo;
	MyTouchEventVoid _decideCallback;
	MyTouchEventVoid _ccCallback;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();
	virtual void displayUnitInfo(Sprite *parent);

};



#endif