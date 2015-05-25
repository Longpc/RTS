#ifndef  _SKILL_SELECT
#define _SKILL_SELECT
#include "base/LayerBase.h"
#include "base/Define.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"
#include "Multi/SkillSelectScene/SkillDetailDialog.h"
#include "Multi/BatleScene/BattleScene.h"
#include "base/baseButton/ClippingButtonBase.h"

#include "Server/Server.h"
#include "Model/data/UserModel.h"
#include "Model/data/UserSkillModel.h"
#include "Model/data/BattleModel.h"

class SkillSelectScene : public LayerBase
{
public:
	static Scene *createScene();
	static SkillSelectScene* create();
	bool init();

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	int _onSelectedSlot = 1;
	int _onSelectSkillTag = 0;
	int _roomId;

	ClipingButtonBase *_skillSlot1;
	ClipingButtonBase *_skillSlot2;

	Label *_skill1NameLabel;
	Label *_skill2NameLabel;

	PageView *_mainPageView;
	int _pageNum;
	Button *lArrow;
	Button *rArrow;

	int _selectedUnitId;
	vector<UserSkillInfo> _allSkillInfo;

	bool _onTouchDisable = false;
	Vec2 _beginTouchPoint;

	int _selectedSkillNum = 0;
	vector<int> _allSelectedSkilId;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void onEnter();
	virtual void update(float delta);

	virtual void getSkillDataFromDatabase();

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);

	virtual void createAllUnitView();

	virtual void onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type);
	//virtual void onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type);

	virtual void onTouchUnit(Ref *pSender, Widget::TouchEventType type);
	virtual void onSelectUnit(int unitId);

	virtual void decideCallBack(Ref *pSender, Widget::TouchEventType type);
	virtual void cancelCallBack(Ref *pSender, Widget::TouchEventType type);

	virtual void displayUnit(Button * parent,Label *label, int unitId);

	virtual void onBackButtonClick(Ref *pSender);
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual Button* createSlotBaseSprite(Vec2 pos);
	virtual Sprite* createUnitNameBg(Vec2 pos);
	virtual Label* createUniNameLabel(Vec2 pos);

	virtual void pageViewEvent(Ref *pSender, PageView::EventType type);
	virtual void setSelectedSlot(int slotNum);

	virtual void leftArrowClickCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void rightArrowClickCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void startBattleCallback(SIOClient* client, const std::string& data);

};

#endif