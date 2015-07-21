#include "BattleAPI.h"

BattleAPI * BattleAPI::s_BattleAPI = nullptr;
BattleAPI * BattleAPI::getInstance()
{
	if (!s_BattleAPI)
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
/************************************************************************/
/* Send unit move event to server 
	@param: unitdata: Moving unit information
	@param: moveDirection: Move direction
	@param: angle: Move angle
	@param: position: Unit 's current position

*/
/************************************************************************/
void BattleAPI::sendMoveEvent(UserUnitInfo unitdata, Vec2 position, int direction, bool movingFlg)
{
	
	auto a = NodeServer::getInstance()->getClient();
	if (a == nullptr) return;
	Document doc;
	doc.SetObject();
	auto userData = UserModel::getInstance()->getUserInfo();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("user_unit", *convertUnitDataToJsonObject(unitdata, allo), allo);
	doc.AddMember("position_x", position.x, allo);
	doc.AddMember("position_y", position.y, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("direction", direction, allo);
	doc.AddMember("moving", movingFlg, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//log("send move data: %s", buffer.GetString());
	a->emit("move", buffer.GetString());
}
void BattleAPI::sendMoveEndEvent(UserUnitInfo unitdata)
{
	auto a = NodeServer::getInstance()->getClient();
	if (a == nullptr) return;
	Document doc;
	doc.SetObject();
	auto userData = UserModel::getInstance()->getUserInfo();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("user_unit", *convertUnitDataToJsonObject(unitdata, allo), allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	a->emit("move_end", buffer.GetString());
	//log("Move End data: %s", buffer.GetString());
	a->on("move_end_end", [&](SIOClient* client, const std::string& data)
	{
		log("move end callback: %s", data.c_str());
	});
}

void BattleAPI::sendAttackEvent(Vec2 direction,UserUnitInfo unit, UserUnitInfo targetUnit, SocketIOCallback callback)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	
	//log("target uuid: %s", targetUnit.uuid.c_str());
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("unit_id", unitId, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	doc.AddMember("user_unit", *convertUnitDataToJsonObject(unit, allo), allo);
	doc.AddMember("target", *convertUnitDataToJsonObject(targetUnit, allo), allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);	
	doc.AddMember("direction_x", direction.x, allo);
	doc.AddMember("direction_y", direction.y, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	//log("Attack send data: %s", buffer.GetString());
	c->emit("attack", buffer.GetString());
	//c->on("attack_end", callback);
	return;
}

void BattleAPI::sendSkillEvent(UserSkillInfo skillData, UserUnitInfo attacker, float angle/*, SocketIOCallback callback*/)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("unit_id", unitId, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	auto u = convertUnitDataToJsonObject(attacker, allo);
	doc.AddMember("mst_unit", *u, allo);
	auto s = convertSkillDataToJsonObject(skillData, allo);
	doc.AddMember("mst_skill", *s, allo);
	doc.AddMember("angle", angle, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//log("Skill: %s", buffer.GetString());
	c->emit("play_skill", buffer.GetString());
	//c->on("play_skill_end", callback);
}

void BattleAPI::sendDameDealEvent(int dame, string targetUuid, SocketIOCallback callback)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) return;

	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	Document doc;
	doc.SetObject();

	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("unit_id", unitId, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("dame",dame, allo);
	doc.AddMember("target", targetUuid.c_str(), allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//log("send dame Event data : %s", buffer.GetString());
	sv->emit("dame_deal", buffer.GetString());

}

void BattleAPI::sendKillEvent(string killerUuid, string targetUnit,Vec2 deadPos, SocketIOCallback callback)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) return;

	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	Document doc;
	doc.SetObject();

	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("unit_id", unitId, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	doc.AddMember("uuid", killerUuid.c_str(), allo);
	doc.AddMember("kill", 1, allo);
	doc.AddMember("target", targetUnit.c_str(), allo);
	doc.AddMember("dead_x", deadPos.x, allo);
	doc.AddMember("dead_y", deadPos.y, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("Send kill event data : %s", buffer.GetString());
	sv->emit("kill_unit", buffer.GetString());
}


void BattleAPI::sendDeadEvent(UserUnitInfo unitData, Vec2 deadTitleCoor, SocketIOCallback callBack)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unit_id = UserModel::getInstance()->getSelectedUnitId();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	doc.AddMember("unit_id", unit_id, allo);
	doc.AddMember("user_unit", *convertUnitDataToJsonObject(unitData, allo), allo);
	doc.AddMember("dead_time", 5, allo);
	doc.AddMember("dead_x", deadTitleCoor.x, allo);
	doc.AddMember("dead_y", deadTitleCoor.y, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//log("dead json: %s", buffer.GetString());

	
	sv->emit("dead", buffer.GetString());
	sv->on("dead_end", callBack);
}

void BattleAPI::sendRepawnEvent(SocketIOCallback callback)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unit_id = UserModel::getInstance()->getSelectedUnitId();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();



	c->emit("respawn", "{\"data\": \"test respwan\"}");
	c->on("respawn_end", callback);
}

void BattleAPI::sendTowerAttackEvent(string towerUuid, string targetUuid, int direction, int teamId)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) return;

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("tower", towerUuid.c_str(), allo);
	doc.AddMember("target", targetUuid.c_str(), allo);
	doc.AddMember("direction", direction, allo);
	doc.AddMember("team_id", teamId, allo);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("Tower Attack: %s", buffer.GetString());


	sv->emit("tower_attack", buffer.GetString());
}

void BattleAPI::battleSyncEvent(UserUnitInfo unitData)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("unit_id", unitId, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);

	auto b = convertUnitDataToJsonObject(unitData,allo);
	doc.AddMember("user_unit", *b, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	//
	log("json: %s", buffer.GetString());
	c->emit("sync", buffer.GetString());
	

	//TEST
	
}

void BattleAPI::sendBattleEndEvent(int winTeamId)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("win_team_id", winTeamId, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sv->emit("battle_end", buffer.GetString());
}

void BattleAPI::sendTestMoveLogic(Vec2 titleCordPost)
{
	

	auto userData = UserModel::getInstance()->getUserInfo();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("pos_x", titleCordPost.x, allo);
	doc.AddMember("pos_y", titleCordPost.y, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	doc.AddMember("user_id", userData.user_id, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("get_title", buffer.GetString());
}

void BattleAPI::sendCheckMapEvent(SocketIOCallback callBack)

{
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("check_map", "test");
	sv->on("check_map_end", callBack);
}

void BattleAPI::sendNeutralTowerAttackEvent(int teamID, int towerIndex, Vec2 direc, SocketIOCallback callBack)
{
	auto userData = UserModel::getInstance()->getUserInfo();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("team_id", teamID, allo);
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("direc_x", direc.x, allo);
	doc.AddMember("direc_y", direc.y, allo);
	doc.AddMember("index", towerIndex, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	//log("Neutral attack send data: %s", buffer.GetString());
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("attack_neutral_tower", buffer.GetString());
	sv->on("attack_neutral_tower_end", callBack);
}

void BattleAPI::sendWarpEvent(int wormIndex, int outGateIndex, SocketIOCallback callback)
{
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("index", wormIndex, allo);
	doc.AddMember("outgate", outGateIndex, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("warp_begin", buffer.GetString());
	sv->on("warp_begin_end", callback);


}

void BattleAPI::sendNeutralUnitAttackEvent(int team_id, int unitIndex,Vec2 direc, SocketIOCallback callback)
{
	auto userData = UserModel::getInstance()->getUserInfo();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("index", unitIndex, allo);
	doc.AddMember("team_id", team_id, allo);
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("direc_x", direc.x, allo);
	doc.AddMember("direc_y", direc.y, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("attack_neutral_unit", buffer.GetString());
	sv->on("attack_neutral_unit_end", callback);

}

void BattleAPI::sendCannonAttackEvent(int team_id, int unitIndex, Vec2 direc, SocketIOCallback callback)
{
	auto userData = UserModel::getInstance()->getUserInfo();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("index", unitIndex, allo);
	doc.AddMember("team_id", team_id, allo);
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("direc_x", direc.x, allo);
	doc.AddMember("direc_y", direc.y, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	auto sv = NodeServer::getInstance()->getClient();

	if (sv == nullptr) {
		return;
	}
	sv->emit("attack_cannon", buffer.GetString());
	sv->on("attack_cannon_end", callback);

}

void BattleAPI::sendCannonLunchEvent(int team_id, int cannonIndex, Vec2 lunchVector) {
	
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) {
		return;
	}
	auto userData = UserModel::getInstance()->getUserInfo();
	string uu = UserModel::getInstance()->getUuId().c_str();

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();

	doc.AddMember("uuid", uu.c_str(), allo);
	doc.AddMember("index", cannonIndex, allo);
	doc.AddMember("team_id", team_id, allo);
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("vec_x", lunchVector.x, allo);
	doc.AddMember("vec_y", lunchVector.y, allo);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	sv->emit("cannon_lunch", buffer.GetString());	
}

void BattleAPI::sendMiniOnMoveEvent(int team_id, int minionIndex, string parentUuid, Vec2 pos, Vec2 veloc)
{
	auto sv = NodeServer::getInstance()->getClient();
	if (sv == nullptr) {
		return;
	}
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("uuid", parentUuid.c_str(), allo);
	doc.AddMember("index", minionIndex, allo);
	doc.AddMember("team_id", team_id, allo);
	doc.AddMember("pos_x", pos.x, allo);
	doc.AddMember("pos_y", pos.y, allo);
	doc.AddMember("veloc_x", veloc.x, allo);
	doc.AddMember("veloc_y", veloc.y, allo);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	sv->emit("minion_move", buffer.GetString());
}

/*Converter*/
Document::GenericValue* BattleAPI::convertUnitDataToJsonObject(UserUnitInfo unitData, Document::AllocatorType& allo)
{
	rapidjson::Value *unitDataValue = new rapidjson::Value()/*(kObjectType)*/;
	unitDataValue->SetObject();
	unitDataValue->AddMember("mst_unit_id", unitData.mst_unit_id, allo);
	unitDataValue->AddMember("hp", unitData.hp, allo);
	unitDataValue->AddMember("hp_heal", unitData.hp_heal, allo);
	unitDataValue->AddMember("mp", unitData.mp, allo);
	unitDataValue->AddMember("mp_heal", unitData.mp_heal, allo);
	unitDataValue->AddMember("attack", unitData.attack, allo);
	unitDataValue->AddMember("attack_range", unitData.attack_range, allo);
	unitDataValue->AddMember("attack_speed", unitData.attack_speed, allo);
	unitDataValue->AddMember("defence", unitData.defence, allo);
	unitDataValue->AddMember("move_speed", unitData.move_speed, allo);
	unitDataValue->AddMember("element", unitData.element, allo);
	unitDataValue->AddMember("mst_skill_id_1", unitData.skill1_id, allo);
	unitDataValue->AddMember("mst_skill_id_2", unitData.skill2_id, allo);
	//log("uuid: %s", unitData.uuid.c_str());
	string uu = unitData.uuid.c_str();
	rapidjson::Value str(kStringType);
	//log("UUID: %s", uu.c_str());
	str.SetString(uu.c_str(),allo);
	unitDataValue->AddMember("uuid", str.GetString(), allo);

	return unitDataValue;
}
Document::GenericValue* BattleAPI::convertSkillDataToJsonObject(UserSkillInfo skillData, Document::AllocatorType& allo)
{

	rapidjson::Value *skillDataValue = new rapidjson::Value();
	skillDataValue->SetObject();
	skillDataValue->AddMember("mst_skill_id", skillData.mst_skill_id, allo);
	skillDataValue->AddMember("skill_type", skillData.skill_type, allo);
	skillDataValue->AddMember("mp_cost", skillData.mp_cost, allo);
	skillDataValue->AddMember("cooldown_time", skillData.cooldown_time, allo);
	skillDataValue->AddMember("range_type", skillData.range_type, allo);
	skillDataValue->AddMember("range_distance", skillData.range_distance, allo);
	skillDataValue->AddMember("multi_effect", skillData.multi_effect, allo);
	skillDataValue->AddMember("target_type", skillData.target_type, allo);
	skillDataValue->AddMember("effect_type", skillData.effect_type, allo);
	skillDataValue->AddMember("buff_effect_type", skillData.buff_effect_type, allo);
	skillDataValue->AddMember("duration", skillData.duration, allo);
	skillDataValue->AddMember("correct_type", skillData.correct_type, allo);
	skillDataValue->AddMember("corrett_value", skillData.corrett_value, allo);

	return skillDataValue;
}