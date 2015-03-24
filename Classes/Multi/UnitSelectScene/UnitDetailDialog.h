#ifndef UNIT_DETAIL_DIALOG
#define UNIT_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"
#include "dataController/SkillData/SkillData.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;
class UnitDetailDialog :public LayerBase
{
public:
	static UnitDetailDialog* create(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Label *statusLabel;
	Node *skillLabel;

	Button *statusButton;
	Button *skillButton;

	UnitInforNew _unitInfo;
	vector<SkillInfoNew> _allUnitSkill;

	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void getUnitSkillDataFromDatabase();

	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();

	virtual void displayUnitInfo(Sprite *parent);
	virtual void statusButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skillButonCallback(Ref *pSEnder, Widget::TouchEventType type);

};


#endif