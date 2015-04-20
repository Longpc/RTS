#ifndef USER_SELECT_SCENE_H__
#define USER_SELECT_SCENE_H__
#include "base/LayerBase.h"

#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Model/data/UserModel.h"

#include "Server/API/ListUserPHPAPI.h"
#include "Server/API/UserLoginPHPAPI.h"
#include "Server/API/StartPHPAPI.h"
#include "Server/Server.h"

#include "json/rapidjson.h"
#include "json/document.h"

class UserSelect :public LayerBase
{
public:
	//static Scene *createScene();
	UI_SCENE_CREATE_FUNC(UserSelect);
	CREATE_FUNC(UserSelect);
	//static UserSelect* create();
	bool init();
private:
	vector<UserInfo> _userList;

	virtual void onBackButtonClick(Ref *pSender);
	virtual void createContent();
	virtual void unitSelectButtonClick(Ref *pSender, Widget::TouchEventType type);
};




#endif