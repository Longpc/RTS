#include "BattleAPI.h"

BattleAPI * BattleAPI::s_BattleAPI = nullptr;
BattleAPI * BattleAPI::getInstance()
{
	if (s_BattleAPI)
	{
		s_BattleAPI = new (std::nothrow)BattleAPI();
		s_BattleAPI->init();
	}
	return s_BattleAPI;
}

void BattleAPI::destroyInstance()
{
	CC_SAFE_DELETE(s_BattleAPI);
}

bool BattleAPI::init()
{


	return true;
}

void BattleAPI::sendMoveEvent()
{
	auto a = NodeServer::getInstance()->getClient();

	a->emit("move", "{\"data\": \"test Move\"}");
	a->on("move_end", [&](SIOClient* client, const std::string& data)
	{
		log("move callback: %s", data.c_str());
	});
}

void BattleAPI::sendAttackEvent()
{
	auto c = NodeServer::getInstance()->getClient();

	c->emit("attack", "{\"data\": \"test Attack\"}");
	c->on("attack_end", [&](SIOClient* client, const std::string data){
		log("attack_end data :%s",data.c_str());
	});
}

void BattleAPI::sendSkillEvent()
{
	auto c = NodeServer::getInstance()->getClient();

	c->emit("skill", "{\"data\": \"test skill\"}");
	c->on("skill_end", [&](SIOClient* client, const std::string data){
		log("attack_end data :%s", data.c_str());
	});
}

void BattleAPI::sendRepawnEvent()
{
	auto c = NodeServer::getInstance()->getClient();

	c->emit("respawn", "{\"data\": \"test respwan\"}");
	c->on("respawn_end", [&](SIOClient* client, const std::string data){
		log("attack_end data :%s", data.c_str());
	});
}

void BattleAPI::battleSyncEvent(UnitInforNew unitData)
{
	auto c = NodeServer::getInstance()->getClient();
	auto userData = UserModel::getInstance()->getUserInfo();
	auto roomId = UserModel::getInstance()->getRoomId();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	//UnitInforNew unitData = UserModel::getInstance()->getUserUnitsInfo();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData._id, allo);
	doc.AddMember("room_id", roomId, allo);
	doc.AddMember("unit_id", unitId, allo);

// 	rapidjson::Value unitDataArray(kArrayType);

	rapidjson::Value unitDataValue;
	unitDataValue.SetObject();
	unitDataValue.AddMember("mst_unit_id", unitData.id, allo);
	unitDataValue.AddMember("name", unitData.name.c_str(), allo);
	unitDataValue.AddMember("hp", unitData.hp, allo);
	unitDataValue.AddMember("hp_heal", unitData.hp_restore, allo);
	unitDataValue.AddMember("mp", unitData.mp, allo);
	unitDataValue.AddMember("mp_heal", unitData.mp_restore, allo);
	unitDataValue.AddMember("attack", unitData.attack_dame, allo);
	unitDataValue.AddMember("attack_range", unitData.attack_range, allo);
	unitDataValue.AddMember("attack_speed", unitData.attack_delay, allo);
	unitDataValue.AddMember("defence", unitData.defence, allo);
	unitDataValue.AddMember("move_speed", unitData.move_speed, allo);
	unitDataValue.AddMember("element", unitData.type, allo);
	unitDataValue.AddMember("mst_skill_id_1", unitData.id, allo);
	unitDataValue.AddMember("mst_skill_id_2", unitData.id, allo);

	doc.AddMember("user_unit", unitDataValue/*convertUnitDataToJsonObject(unitData,allo)*/, allo);
	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//
	log("json: %s", buffer.GetString());
	c->emit("sync", buffer.GetString());
	c->on("sync_end", [&](SIOClient* client, const std::string data){
		log("SYNC end data :%s", data.c_str());
	});

	//TEST
	
}

Document::GenericValue& BattleAPI::convertUnitDataToJsonObject(UnitInforNew unitData, Document::AllocatorType& allo)
{
	rapidjson::Value unitDataValue/*(kObjectType)*/;
	unitDataValue.SetObject();
	unitDataValue.AddMember("mst_unit_id", unitData.id, allo);
	unitDataValue.AddMember("name", unitData.name.c_str(), allo);
	unitDataValue.AddMember("hp", unitData.hp, allo);
	unitDataValue.AddMember("hp_heal", unitData.hp_restore, allo);
	unitDataValue.AddMember("mp", unitData.mp, allo);
	unitDataValue.AddMember("mp_heal", unitData.mp_restore, allo);
	unitDataValue.AddMember("attack", unitData.attack_dame, allo);
	unitDataValue.AddMember("attack_range", unitData.attack_range, allo);
	unitDataValue.AddMember("attack_speed", unitData.attack_delay, allo);
	unitDataValue.AddMember("defence", unitData.defence, allo);
	unitDataValue.AddMember("move_speed", unitData.move_speed, allo);
	unitDataValue.AddMember("element", unitData.type, allo);
	unitDataValue.AddMember("mst_skill_id_1", unitData.id, allo);
	unitDataValue.AddMember("mst_skill_id_2", unitData.id, allo);
	return unitDataValue;
}
