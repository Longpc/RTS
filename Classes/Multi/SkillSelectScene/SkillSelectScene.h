#ifndef  _SKILL_SELECT
#define _SKILL_SELECT
#include "base/LayerBase.h"
#include "base/Define.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"
#include "Multi/SkillSelectScene/SkillDetailDialog.h"
#include "Multi/BatleScene/BatleScene.h"

class SkillSelectScene : public LayerBase
{
public:
	static Scene *createScene(int unit);
	static SkillSelectScene* create(int unit);
	bool init(int unit);

private:
	virtual void onEnter();
	virtual void update(float delta);

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);

	virtual void createAllUnitView();
	virtual ClippingNode* createSlot(Vec2 position);

	virtual void onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type);
	//virtual void onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type);

	virtual void onTouchUnit(Ref *pSender, Widget::TouchEventType type);
	virtual void onSelectUnit(int unitId);

	virtual void decideCallBack(Ref *pSender, Widget::TouchEventType type);
	virtual void cancelCallBack(Ref *pSender, Widget::TouchEventType type);

	virtual void displayUnit(Button * parent,LabelTTF *label, int unitId);


	virtual void onBackButtonClick(Ref *pSender);
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual Button* createSlotBaseSprite(Vec2 pos);
	virtual Sprite* createUnitNameBg(Vec2 pos);
	virtual LabelTTF* createUniNameLabel(Vec2 pos);

	virtual void pageViewEvent(Ref *pSender, PageView::EventType type);
	virtual void setSelectedSlot(int slotNum);

	int _onSelectedSlot = 1;
	int _onSelectedUnitId = 0;
	int _roomId;

	Button *button1;
	Button *button2;
	//Button *button3;

	Button *_slot1BackGroundButton;
	Button *_slot2BackGroundButton;
	//Button *_slot3BackGroundButton;

	LabelTTF *_skill1NameLabel;
	LabelTTF *_skill2NameLabel;
	//LabelTTF *_skill3nameLabel;

	ClippingNode *_slot1;
	ClippingNode *_slot2;
	//ClippingNode *_slot3;

	PageView *_mainPage;
	int _pageNum;
	Button *lArrow;
	Button *rArrow;


	int _unitInfo;

	vector<SkillInfo> _allSkillInfo;

	bool _onTouchDisable = false;
	Vec2 _beginTouchPoint;


	int _selectedSkillNum = 0;
};

#endif