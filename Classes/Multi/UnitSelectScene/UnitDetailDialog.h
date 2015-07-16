#ifndef UNIT_DETAIL_DIALOG
#define UNIT_DETAIL_DIALOG
#include "base/LayerBase.h"
#include "base/Define.h"
#include "dataController/SkillData/SkillData.h"

#include "Model/data/UserModel.h"
#include "Model/data/UserSkillModel.h"
#include "Model/data/UserUnitModel.h"

typedef std::function<void(Ref *pSender, Widget::TouchEventType type)> MyTouchEvent;
class UnitDetailDialog :public LayerBase
{
public:
	static UnitDetailDialog* create(UserUnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);
	bool init(UserUnitInfo unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback);

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Node *_statusNode;
	Node *skillLabel;

	Button *statusButton;
	Button *skillButton;

	UserUnitInfo _unitInfo;
	vector<UserSkillInfo> _allUnitSkill;

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