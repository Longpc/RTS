#ifndef _MULTI_MODE_SELECT_SCENE_H__
#define _MULTI_MODE_SELECT_SCENE_H__
///Include scene header here///
#include "base/Define.h"
#include "base/LayerBase.h"
#include "UserSelectScene/UserSelectScene.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"
#include "Multi/MemberFullDialogLayer.h"

#include "Model/data/UserModel.h"
#include "Server/API/StartPHPAPI.h"
#include "Server/Server.h"
#include "Model/data/RoomModel.h"


///Include library header here///
#include <string.h>
#include "extensions/cocos-ext.h"
#define REMOVE_CHILD 1111
USING_NS_CC_EXT;
class MultiTeamSelectScene :public LayerBase
{
public:
	static Scene *createScene(int userId);
	CREATE_FUNC(MultiTeamSelectScene);
	bool init();
protected:
private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	Sprite *_usernameBg;

	vector<RoomUser> _roomUserInfo;
	vector<RoomUser> _blueTeamUserInfo;
	vector<RoomUser> _redTeamUserInfo;

	Sprite *_blueTeamBg;
	Sprite *_redTeamBg;

	int _curRoomId = 1;
	int _curUserId = 1;

	RoomUser _currentUserInfo;

	bool _onAccess = true;

	string _userName;
	float _tempCount = 0;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	void update(float delta);
	
	virtual void onBackButtonClick(Ref *pSender);

	virtual void redTeamButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void blueTeamButtonCallback(Ref *pSender, Widget::TouchEventType type);
	/*
	Get room information base on roomID
	and Show room info in bottom left of scene
	param: int roomId : current room Id
	*/
	virtual void getRoomInfo(int roomId);
	/*
	Get team info an show
	param: int teamId : current  teamId
	*/
	virtual void getAndShowTeamInfo(int teamId,Sprite *parent);

	virtual Sprite* createUsernameBackground(Vec2 position);
	virtual Label* createLabelWithStringandPosition(string text, Vec2 pos);

	/*
	Check selected room accessible
	param:
	@ int roomId : ID of room to check
	Return: true when accessible and false for other cases
	*/
	virtual bool checkTeamFull(int teamId);
	/*
	Enter select team
	@ int teamId: selected team id
	*/
	virtual void enterTeam(int teamId);

	virtual void testFullTeam(Ref *pSender, Widget::TouchEventType type);

};
#endif