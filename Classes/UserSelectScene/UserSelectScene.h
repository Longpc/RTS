#ifndef USER_SELECT_SCENE_H__
#define USER_SELECT_SCENE_H__
#include "base/LayerBase.h"

#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"

class UserSelect :public LayerBase
{
public:
	static Scene *createScene();
	static UserSelect* create();
	bool init();
private:
	virtual void onBackButtonClick(Ref *pSender);
	virtual void createContent();
	virtual void unitSelectButtonClick(Ref *pSender, Widget::TouchEventType type);
};




#endif