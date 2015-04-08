#ifndef USER_SELECT_SCENE_H__
#define USER_SELECT_SCENE_H__
#include "base/LayerBase.h"

#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"
#include "Model/data/UserModel.h"

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

	virtual void getUserListFromServer();
	virtual void serverAPICallback(HttpClient* client, HttpResponse* response);

	virtual void postUserInfoToServer(int user_id);
	virtual void loginCallback(HttpClient* client, HttpResponse* response);
};




#endif