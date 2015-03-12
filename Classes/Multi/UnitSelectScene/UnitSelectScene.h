#ifndef UNIT_SELECT_SCENE_H__
#define UNIT_SELECT_SCENE_H__

///Include scene header here///
#include "base/Define.h"
#include "base/LayerBase.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Multi/UnitSelectScene/UnitDetailDialog.h"
#include "Multi/SkillSelectScene/SkillSelectScene.h"
#include "ModeSelectScene/ModeSelectScene.h"


///Include library header here///
#include <string.h>

class MultiUnitSelectScene : public LayerBase
{
public:
	/*
	Create scene with content from room Information.
	param:
	@roomId: selected room Id
	@pageFlg: to know the father of this screen
	*/
	static Scene *createScene(int roomId,int pageFlg);
	static MultiUnitSelectScene *create(int roomId, int pageFlg);
	bool init(int roomId, int pageFlg);
protected:
private:
	virtual void onEnter();
	virtual void update(float delta);
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	ClippingNode* createSlot(Vec2 position);
	virtual void createAllUnitView();

	virtual void onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type);

	virtual void onTouchUnit(Ref *pSender, Widget::TouchEventType type);
	virtual void onSelectUnit(int unitId);

	virtual void decideCallBack(Ref *pSender, Widget::TouchEventType type);
	virtual void cancelCallBack(Ref *pSender, Widget::TouchEventType type);

	virtual void displayUnit(Button * parent,LabelTTF *textView,int unitId);

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
	int _pageFlg;

	Layer *layer;
	Button *button1;
	Button *button2;
	Button *button3;

	Button *baseSlot1;
	Button *baseSlot2;
	Button *baseSlot3;

	Button *lArrow;
	Button *rArrow;

	LabelTTF *selectedUnit1Name;
	LabelTTF *selectedUnit2Name;
	LabelTTF *selectedUnit3Name;

	ClippingNode *_slot1;
	ClippingNode *_slot2;
	ClippingNode *_slot3;

	PageView *_mainPage;
	int _pageNum;

	vector<UnitInfo> _allUnitInfo;

	bool _onTouchDisable;
	Vec2 _touchBeginPoint;

	int _selectedUnit = 0;

};

#endif