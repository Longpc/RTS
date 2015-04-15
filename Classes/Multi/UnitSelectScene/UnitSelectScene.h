#ifndef UNIT_SELECT_SCENE_H__
#define UNIT_SELECT_SCENE_H__

///Include scene header here///
#include "base/Define.h"
#include "base/LayerBase.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Multi/UnitSelectScene/UnitDetailDialog.h"
#include "Multi/SkillSelectScene/SkillSelectScene.h"
#include "ModeSelectScene/ModeSelectScene.h"
#include "base/baseButton/ClippingButtonBase.h"

#include "Model/data/UserModel.h"
#include "Server/Server.h"

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
	///VARIABLES///////////////////////////////////////////////////////////////////////
	int _onSelectedSlot = 1;
	int _onSelectedUnitTag = 0;
	int _decidedUnitId = 0;

	int _roomId;
	int _pageFlg;

	Layer *layer;

	ClipingButtonBase* _buttonSlot1;
	ClipingButtonBase* _buttonSlot2;
	ClipingButtonBase* _buttonSlot3;

	Button *lArrow;
	Button *rArrow;

	Label *selectedUnit1Name;
	Label *selectedUnit2Name;
	Label *selectedUnit3Name;

	PageView *_mainPage;
	int _pageNum;

	//vector<UnitInfo> _allUnitInfo;
	vector<UnitInforNew> _allUnitInfoNew;

	bool _onTouchDisable;
	Vec2 _touchBeginPoint;

	int _selectedUnit = 0;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void getDataFromDataBase();
	virtual void onEnter();
	virtual void update(float delta);
	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void createAllUnitView();

	virtual void onTouchUnitSlot1(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot2(Ref *pSender, Widget::TouchEventType type);
	virtual void onTouchUnitSlot3(Ref *pSender, Widget::TouchEventType type);

	virtual void onTouchUnit(Ref *pSender, Widget::TouchEventType type);
	virtual void onSelectUnit(int unitId);

	virtual void decideCallBack(Ref *pSender, Widget::TouchEventType type);
	virtual void cancelCallBack(Ref *pSender, Widget::TouchEventType type);

	virtual void displayUnit(Button * parent,Label *textView,int unitId);

	virtual void onBackButtonClick(Ref *pSender);
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual Button* createSlotBaseSprite(Vec2 pos);
	virtual Sprite* createUnitNameBg(Vec2 pos);
	virtual Label* createUniNameLabel(Vec2 pos);

	virtual void pageViewEvent(Ref *pSender, PageView::EventType type);
	virtual void setSelectedSlot(int slotNum);

	virtual void leftArrowClickCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void rightArrowClickCallback(Ref *pSender, Widget::TouchEventType type);

};

#endif