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

	void sendMoveEvent();
	void sendAttackEvent();
	void sendSkillEvent();
	void sendRepawnEvent();
	void battleSyncEvent(UnitInforNew unitData);

	bool init();
private:
	static BattleAPI *s_BattleAPI;
	
	Document::GenericValue& convertUnitDataToJsonObject(UnitInforNew unitData, Document::AllocatorType& allo);

};


#endif