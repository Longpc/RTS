#ifndef SKILL_DETAIL_DIALOG
#define SKILL_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"
typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;

class SkillDetailDialog :public LayerBase
{
public:
	static SkillDetailDialog* create(SkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(SkillInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();

	virtual void displayUnitInfo(Sprite *parent);

	Sprite *statusTab;
	

	SkillInfo _unitInfo;
	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;
};



#endif