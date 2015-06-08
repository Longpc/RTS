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
	auto a = NodeServer::getInstance()->getClient();

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
void BattleAPI::sendMoveEvent(UserUnitInfo unitdata, int moveDirection,float angle, Vec2 position, int statusId, bool onMovingFlg)
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
	doc.AddMember("direction", moveDirection, allo);
	doc.AddMember("angle", angle, allo);
	doc.AddMember("position_x", position.x, allo);
	doc.AddMember("position_y", position.y, allo);
	doc.AddMember("status", statusId, allo);
	doc.AddMember("moving", onMovingFlg, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid",uu.c_str(),allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	//log("send move data: %s", buffer.GetString());
	a->emit("move", buffer.GetString());
	a->on("move_end", [&](SIOClient* client, const std::string& data)
	{
		log("move callback: %s", data.c_str());
	});
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
	log("Move End data: %s", buffer.GetString());
	a->on("move_end_end", [&](SIOClient* client, const std::string& data)
	{
		log("move end callback: %s", data.c_str());
	});
}

void BattleAPI::sendAttackEvent(int direction,UserUnitInfo unit, UserUnitInfo targetUnit, SocketIOCallback callback)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	int unitId = UserModel::getInstance()->getSelectedUnitId();
	
	log("target uuid: %s", targetUnit.uuid.c_str());
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
	doc.AddMember("direction", direction, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	log("Attack send data: %s", buffer.GetString());
	c->emit("attack", buffer.GetString());
	c->on("attack_end", callback);
	return;
}

void BattleAPI::sendSkillEvent(UserSkillInfo skillData, UserUnitInfo attacker/*, SocketIOCallback callback*/)
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

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("Skill: %s", buffer.GetString());
	c->emit("play_skill", buffer.GetString());
	//c->on("play_skill_end", callback);
}

void BattleAPI::sendBuffSkillEvent(UserSkillInfo skill, SocketIOCallback callback)
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	auto userData = UserModel::getInstance()->getUserInfo();
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("user_id", userData.user_id, allo);
	doc.AddMember("room_id", userData.room_id, allo);
	doc.AddMember("team_id", userData.team_id, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	auto unitInfo = UserUnitModel::getInstance()->getUnitInfoById(UserModel::getInstance()->getSelectedUnitId());
	doc.AddMember("user_unit", *convertUnitDataToJsonObject(unitInfo, allo), allo);
	auto s = convertSkillDataToJsonObject(skill, allo);
	doc.AddMember("mst_skill", *s, allo);

	rapidjson::Value targetList;
	targetList.SetArray();
	

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("Emit data: %s", buffer.GetString());
	c->emit("buff_skill", buffer.GetString());
	c->on("buffskill_end", callback);
}




void BattleAPI::sendDeadEvent(UserUnitInfo unitData, SocketIOCallback callBack)
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
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("dead json: %s", buffer.GetString());

	
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

void BattleAPI::sendBattleEndEvent(SocketIOCallback callBack)
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
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);
	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	sv->emit("battle_end", buffer.GetString());
	sv->on("battle_end_end", callBack);
}
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
	log("UUID: %s", uu.c_str());
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




