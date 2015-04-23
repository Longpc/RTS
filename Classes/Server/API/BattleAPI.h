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

using namespace std;
USING_NS_CC;
using namespace rapidjson;

class BattleAPI
{
public:
	static BattleAPI *getInstance();

	static void destroyInstance();

	void sendMoveEvent(UserUnitInfo unitdata, int moveDirection, Vec2 position, int statusId);
	void sendAttackEvent();
	void sendSkillEvent(UserSkillInfo skillData, vector<int> targetsId = {});
	void sendRepawnEvent();
	void battleSyncEvent(UserUnitInfo unitData);
	void sendBattleEndEvent();

	bool init();
private:
	static BattleAPI *s_BattleAPI;

	Document::GenericValue* convertUnitDataToJsonObject(UserUnitInfo unitData, Document::AllocatorType& allo);
	Document::GenericValue* convertSkillDataToJsonObject(UserSkillInfo skillData, Document::AllocatorType& allo);
};


#endif