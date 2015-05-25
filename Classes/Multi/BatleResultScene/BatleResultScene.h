#ifndef BATLE_SCENE_RESULT
#define BATLE_SCENE_RESULT
#include "base/LayerBase.h"
#include "base/Define.h"
#include "ModeSelectScene/ModeSelectScene.h"
#include "base/baseButton/ClippingButtonBase.h"

#include "Model/data/UserUnitModel.h"
class BatleResultScene : public LayerBase
{

public:
	static Scene* createScene(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo);
	static BatleResultScene* create(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo);
	bool init(vector<UserBattleInfo> blueTeamInfo, vector<UserBattleInfo> readTeamInfo);
private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *_blueTeamTabBackground;
	Sprite *_redTeamTabBackground;
	Button *_blueTeamButton;
	Button *_redTeamButton;

	vector<Label*> _unitNameLabel;

	vector<ClipingButtonBase*> _allSlot;

	vector<UserBattleInfo> _blueTeamInfo;
	vector<UserBattleInfo> _readTeamInfo;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void createContent();
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void tabButtonClickCallback(Ref *pSender, Widget::TouchEventType type);

	virtual Sprite* createUnitNameBg(Vec2 pos);
	virtual Label* createUniNameLabel(Vec2 pos);
	virtual void createBattleInfo(Sprite *parent, vector<UserBattleInfo> info);
	virtual void updateUnitSlot(vector<UserBattleInfo> info);
};


#endif