#ifndef BATTLE_API_H__
#define BATTLE_API_H__

#include "cocos2d.h"
#include "Server/Server.h"
#include "Model/data/UserModel.h"
#include "Model/data/UserUnitModel.h"
#include "Model/data/BattleModel.h"
#include "base/database/DataUtils.h"

#include <string>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"



using namespace std;
USING_NS_CC;
using namespace rapidjson;

class BattleAPI
{
public:
	static BattleAPI *getInstance();

	static void destroyInstance();

	void sendMoveEvent(Vec2 position, Vec2 veloc);
	
	void sendMoveEndEvent(UserUnitInfo unitdata);
	
	void sendAttackEvent(Vec2 direction,UserUnitInfo attackerUnit, UserUnitInfo targetData, SocketIOCallback callback);
	
	void sendSkillEvent(UserSkillInfo skillData, UserUnitInfo attacker, float angle/*, SocketIOCallback callback*/);
		
	void sendDeadEvent(UserUnitInfo unitData, Vec2 deadTitleCoor, SocketIOCallback callback);

	void sendDameDealEvent(int dame, string targetUuid, SocketIOCallback callback);

	void sendKillEvent(string killerUuid, string targetUnit, Vec2 deadPos, SocketIOCallback callback);
	
	void sendRepawnEvent(SocketIOCallback callback);
	
	void battleSyncEvent(UserUnitInfo unitData);

	void sendTowerAttackEvent(string towerUuid, string targetUuid, int direction, int teamId);

	void sendBattleEndEvent(int winTeamId);

	void sendTestMoveLogic(Vec2 titleCordPost);

	void sendCheckMapEvent(SocketIOCallback callback);

	void sendNeutralTowerAttackEvent(int teamID, int towerIndex, Vec2 direc, SocketIOCallback callback);

	void sendWarpEvent(int wormIndex, int outGateIndex, SocketIOCallback callback);

	void sendNeutralUnitAttackEvent(int team_id, int unitIndex, Vec2 direc, SocketIOCallback callback);

	void sendCannonAttackEvent(int team_id, int unitIndex, Vec2 direc, SocketIOCallback callback);
	
	void sendCannonLunchEvent(int team_id, int cannonIndex, SocketIOCallback callback);

	void sendMiniOnMoveEvent(int team_id, int minionIndex, string parentUuid, Vec2 pos, Vec2 veloc);

	bool init();
private:
	static BattleAPI *s_BattleAPI;

	Document::GenericValue* convertUnitDataToJsonObject(UserUnitInfo unitData, Document::AllocatorType& allo);
	Document::GenericValue* convertSkillDataToJsonObject(UserSkillInfo skillData, Document::AllocatorType& allo);
};


#endif