#ifndef BATTLE_MODEL_H__
#define BATTLE_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserSkillModel.h"
#include "Model/data/UserUnitModel.h"
#include "Model/data/UserModel.h"

#include "base/database/DataUtils.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

USING_NS_CC;
using namespace rapidjson;
using namespace std;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define BUILDING_MYDLL 12
#ifdef BUILDING_MYDLL
#define MYCLASS_DECLSPEC __declspec(dllexport)
#else
#define MYCLASS_DECLSPEC __declspec(dllimport)
#endif
#endif

struct Room_User_Unit_Model
{
	string _id = "";
	int status = 0;
	int mp = 0;
	int hp = 0;
	float position_x = 0;
	float position_y = 0;
	int direction = 0;
	int team_id = 0;
	int room_id = 0;
	int user_id = 0;
	int mst_unit_id = 0;
};
struct Room_User_Model
{
	string _id = "";
	int npc = 0;
	int state = 0;
	int ready = 0;
	int team_id = 0;
	int user_id = 0;
	int room_id = 0;
	string room_user_id = "";
};
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
# define MYCLASS_DECLSPEC CC_DLL
#endif

class MYCLASS_DECLSPEC BattleModel :public Ref
{
public:
	static BattleModel* getInstance();
	bool init();
	void parserJsonToInitData(string jsonData);

	CC_SYNTHESIZE(vector<int>, _playerSkills, PlayerSkills);
	CC_SYNTHESIZE(vector < Room_User_Model>, _roomUserList, RoomUserList);
	CC_SYNTHESIZE(vector<Room_User_Unit_Model>, _roomUserUnitList, RoomUserUnitList);

private:
	static BattleModel * s_BattleModel;
};
#endif