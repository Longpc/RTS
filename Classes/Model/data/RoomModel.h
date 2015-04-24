#ifndef ROOM_MODEL_H__
#define ROOM_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserSkill.h"
#include "Model/data/UserUnit.h"
#include "Model/data/UserModel.h"

#include "Server/Server.h"

#include "base/database/DataUtils.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

USING_NS_CC;
using namespace rapidjson;
using namespace std;

struct RoomUser
{
	string _uuid;
	int room_id = 0;
	int user_id = 0;
	int team_id = 0;
	int _ready = 0; 
	int state = 0;
	int npc = 0;
};

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define BUILDING_MYDLL 12
#ifdef BUILDING_MYDLL
#define MYCLASS_DECLSPEC __declspec(dllexport)
#else
#define MYCLASS_DECLSPEC __declspec(dllimport)
#endif
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
# define MYCLASS_DECLSPEC CC_DLL
#endif

class MYCLASS_DECLSPEC RoomModel :public Ref
{
public:
	static RoomModel * getInstance();
	bool init();

	CC_SYNTHESIZE(int, _roomId, RoomId);
	CC_SYNTHESIZE(vector<RoomUser>, _roomUserList,RoomUserList);
	CC_SYNTHESIZE(vector<int>, _redTeamUserList, RedTeamUserList);
	CC_SYNTHESIZE(vector<int>, _blueTeamUserList, BlueTeamUserList);

	void addUserInfoToUserList(RoomUser data);
	void setTeamForUserByUserId(int room_id, int user_id, int team_id);
private:
	static RoomModel * s_RoomModel;
};

#endif