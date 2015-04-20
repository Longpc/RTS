#ifndef BATTLE_API_H__
#define BATTLE_API_H__

#include "cocos2d.h"
#include "Server/Server.h"
#include "Model/data/UserModel.h"
#include "base/database/DataUtils.h"

#include <string>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

typedef struct UnitData_temp
{
	int user_id = 0;
	int room_id = 0;
	int mst_unit_id = 0;
	int team_id = 0;
	int hp = 0;
	int mp = 0;
	int position_x = 0;
	int position_y = 0;
	int status = 0;
};
using namespace std;
USING_NS_CC;
using namespace rapidjson;

class BattleAPI
{
public:
	static BattleAPI *getInstance();

	static void destroyInstance();

	void sendMoveEvent(UnitData_temp unitdata);
	void sendAttackEvent();
	void sendSkillEvent();
	void sendRepawnEvent();
	void battleSyncEvent(UserUnitInfo unitData);

	bool init();
private:
	static BattleAPI *s_BattleAPI;
	
	Document::GenericValue& convertUnitDataToJsonObject(UnitInforNew unitData, Document::AllocatorType& allo);

};


#endif