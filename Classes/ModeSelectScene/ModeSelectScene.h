#ifndef MODE_SELECT_SCENE_H__
#define MODE_SELECT_SCENE_H__
///Include scene header here///
#include "../base/LayerBase.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Multi/MemberFullDialogLayer.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"

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
	virtual void multiButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void soloButtonCallback(Ref *pSender, Widget::TouchEventType type);
	/*
	Check multi-player room can accessible
	Return: true if accessible / false when full or can't access
	*/
	virtual bool checkRoomMember();

	virtual void testDialog(Ref *pSender, Widget::TouchEventType type);

	bool _onAccess = true;
	bool _onDisableTouch = false;
};

#endif