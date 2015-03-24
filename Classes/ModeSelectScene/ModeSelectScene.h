#ifndef MODE_SELECT_SCENE_H__
#define MODE_SELECT_SCENE_H__
///Include scene header here///
#include "../base/LayerBase.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Multi/MemberFullDialogLayer.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"

//for test
#include "Multi/BatleScene/BatleScene.h"

///Include library header here///
#include <string.h>

class ModeSelectScene :public LayerBase
{
public:
	static Scene *createScene();
	CREATE_FUNC(ModeSelectScene);
	bool init();
protected:

private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	/*this flag is false when selected room was full*/
	bool _onAccess = true;
	bool _onDisableTouch = false;
	int _retryCount = 1;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void multiButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void soloButtonCallback(Ref *pSender, Widget::TouchEventType type);
	/*
	Check multi-player room can accessible
	Return: true if accessible / false when full or can't access
	*/
	virtual bool checkRoomMember();

	/*virtual void testDialog(Ref *pSender, Widget::TouchEventType type);*/

	/*Create data base*/
	virtual void downloadDatabase();
	virtual void serverCallback(HttpClient* client, HttpResponse* response);
};

#endif