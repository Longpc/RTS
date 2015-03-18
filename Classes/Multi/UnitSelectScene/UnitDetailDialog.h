#ifndef UNIT_DETAIL_DIALOG
#define UNIT_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;
class UnitDetailDialog :public LayerBase
{
public:
	static UnitDetailDialog* create(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	virtual void getUnitSkillDataFromDatabase();

	virtual void closeButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void decideButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void closeDialog();

	virtual void displayUnitInfo(Sprite *parent);
	virtual void statusButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skillButonCallback(Ref *pSEnder, Widget::TouchEventType type);


	LabelTTF *statusLabel;
	LabelTTF *skillLabel;

	Button *statusButton;
	Button *skillButton;

	UnitInforNew _unitInfo;
	vector<SkillInfoNew> _allUnitSkill;

	MyTouchEvent _decideCallback;
	MyTouchEvent _ccCallback;

	virtual void testLog();
};


#endif