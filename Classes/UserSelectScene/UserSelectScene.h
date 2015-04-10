#ifndef USER_SELECT_SCENE_H__
#define USER_SELECT_SCENE_H__
#include "base/LayerBase.h"

#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Model/data/UserModel.h"

#include "Server/API/ListUserPHPAPI.h"
#include "Server/API/UserLoginPHPAPI.h"

#include "json/rapidjson.h"
#include "json/document.h"

class UserSelect :public LayerBase
{
public:
	static Scene *createScene();
	static UserSelect* create();
	bool init();
private:
	vector<UserInfo> _userList;

	virtual void onBackButtonClick(Ref *pSender);
	virtual void createContent();
	virtual void unitSelectButtonClick(Ref *pSender, Widget::TouchEventType type);
};




#endif