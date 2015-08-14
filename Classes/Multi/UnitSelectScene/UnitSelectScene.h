#ifndef UNIT_SELECT_SCENE_H__
#define UNIT_SELECT_SCENE_H__

///Include scene header here///
#include "base/Define.h"
#include "base/LayerBase.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Multi/UnitSelectScene/UnitDetailDialog.h"
#include "Multi/SkillSelectScene/SkillDetailDialog.h"
#include "Multi/SkillSelectScene/SkillSelectScene.h"
#include "ModeSelectScene/ModeSelectScene.h"
#include "base/baseButton/ClippingButtonBase.h"
#include "base/UnitSkillInfoButtonGroup/UnitInfoGroup.h"

#include "Model/data/UserModel.h"

#include "Model/data/UserUnitModel.h"

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
	~MultiUnitSelectScene();
protected:
private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	int _roomId;
	int _pageFlg;

	Layer *layer;
	vector<UnitInfoGroup*> _allUnitGroup;

	UnitInfoGroup *_unitGroup1;
	UnitInfoGroup *_unitGroup2;
	UnitInfoGroup *_unitGroup3;

	Button * nextButton;
	Sprite* _pagebackGround;
	Button *lArrow;
	Button *rArrow;

	PageView *_mainPage;
	int _pageNum;

	vector<UserSkillInfo> _allSkillInfo;
	vector<UserUnitInfo> _allUnitInfoNew;
	bool _onTouchDisable;
	Vec2 _touchBeginPoint;

	int _onSelectSkillButtonIndex = -1;
	int _selectedSkillNum = 0;
	vector<int> _allSelectedSkilId = {};
	int _selectedUnit = 0;


	bool _sendUnitFlg = false;
	bool _sendSkillFlg = false;

	bool _isSentRequest = false;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void getDataFromDataBase();
	virtual void onEnter();

	virtual void updateContent(Ref *p);
	virtual void checkFlag();

	bool onTouchBegan(Touch *touch, Event *unused_event);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onTouchMoved(Touch *touch, Event *unused_event);
	/*to set page view show unit list or skill list 
	* @type : 1 for unit and 2 for skill
	*/
	virtual void createPageView(int type);

	virtual void onTouchUnitButton();
	virtual void onTouchSkill1Button();
	virtual void onTouchSkill2Button();

	virtual void onTouchPageItem(Ref *pSender, Widget::TouchEventType type, int pageType);

	virtual void decideCallBack(int index, int pageType);
	virtual void cancelCallBack();


	virtual void onBackButtonClick(Ref *pSender);
	virtual void sendReadyButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void startBattleCallback(SIOClient* client, const std::string& data);
	virtual void startBattle();

	virtual void pageViewEvent(Ref *pSender, PageView::EventType type);

	virtual void arrowButonClick(Ref *pSender, Widget::TouchEventType type, int pageScroll);

	virtual void sendSelectUnitInfo(int unitId);
	virtual void sendSElectSkillInfo();



};

#endif