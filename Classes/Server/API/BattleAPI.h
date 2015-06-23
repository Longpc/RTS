#ifndef BATTLE_API_H__
#define BATTLE_API_H__

#include "cocos2d.h"
#include "Server/Server.h"
#include "Model/data/UserModel.h"
#include "Model/data/UserUnitModel.h"
#include "base/database/DataUtils.h"

#include <string>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

typedef std::function<void(SIOClient* client, const std::string data)> SocketIOCallback;

using namespace std;
USING_NS_CC;
using namespace rapidjson;

class BattleAPI
{
public:
	static BattleAPI *getInstance();

	static void destroyInstance();

	void sendMoveEvent(UserUnitInfo unitdata, int moveDirection,float angle, Vec2 position, int statusId, bool onMovingFlg);
	
	void sendMoveEndEvent(UserUnitInfo unitdata);
	
	void sendAttackEvent(int direction,UserUnitInfo attackerUnit, UserUnitInfo targetData, SocketIOCallback callback);
	
	void sendSkillEvent(UserSkillInfo skillData, UserUnitInfo attacker/*, SocketIOCallback callback*/);
	
	void sendBuffSkillEvent(UserSkillInfo skill, SocketIOCallback callback);
	
	void sendDeadEvent(UserUnitInfo unitData, SocketIOCallback callback);

	void sendDameDealEvent(int dame, string targetUuid, SocketIOCallback callback);

	void sendKillEvent(string killerUuid, string targetUnit, SocketIOCallback callback);
	
	void sendRepawnEvent(SocketIOCallback callback);
	
	void battleSyncEvent(UserUnitInfo unitData);

	void sendTowerAttackEvent(string towerUuid, string targetUuid, int direction, int teamId);

	void sendBattleEndEvent(int winTeamId);

	void sendTestMoveLogic(Vec2 titleCordPost);

	void sendCheckMapEvent(SocketIOCallback callback);

	void sendNeutralTowerAttackEvent(int teamID, int towerIndex, int direction, SocketIOCallback callback);


	bool init();
private:
	static BattleAPI *s_BattleAPI;

	Document::GenericValue* convertUnitDataToJsonObject(UserUnitInfo unitData, Document::AllocatorType& allo);
	Document::GenericValue* convertSkillDataToJsonObject(UserSkillInfo skillData, Document::AllocatorType& allo);
};


#endif