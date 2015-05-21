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

void BattleAPI::sendMoveEvent(UserUnitInfo unitdata, int moveDirection,float angle, Vec2 position, int statusId)
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
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid",uu.c_str(),allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

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

void BattleAPI::sendAttackEvent()
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
	doc.AddMember("room_user", "{room_user}", allo);
	doc.AddMember("user_unit", "{user_unit}", allo);
	doc.AddMember("attack", "{attack}", allo);
	doc.AddMember("attack.target", "{attack_target}", allo);
	doc.AddMember("attack.target.object_type", "{attack target}", allo);
	doc.AddMember("attack.target.object_id", 12, allo);
	doc.AddMember("attack.back_attack", 0, allo);
	string uu = UserModel::getInstance()->getUuId().c_str();
	doc.AddMember("uuid", uu.c_str(), allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	c->emit("attack", buffer.GetString());
	c->on("attack_end", [&](SIOClient* client, const std::string data){
		log("attack_end data :%s",data.c_str());
	});
	return;
}

void BattleAPI::sendSkillEvent(UserSkillInfo skillData, vector<int> targetsId)
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

	auto s = convertSkillDataToJsonObject(skillData, allo);
	doc.AddMember("mst_skill", *s, allo);
	rapidjson::Value targetList;
	targetList.SetArray();
	for (int i = 0; i < targetsId.size(); i++)
	{
		targetList.PushBack(targetsId[i], allo);
		//targetList.AddMember("target_unique_id", targetsId[i], allo);
	}
	doc.AddMember("target_list", targetList, allo);

	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);

	log("Emit data: %s", buffer.GetString());
	string eventName = "skill_";
	if (skillData.skill_type == 1) {
		eventName.append("player");
	}
	else {
		eventName.append("unit");
	}

	c->emit(eventName.c_str(), buffer.GetString());
	c->on("skill_end", [&](SIOClient* client, const std::string data){
		log("skill_end :%s", data.c_str());
	});
}

void BattleAPI::sendDeadEvent(UserUnitInfo unitData)
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
	sv->on("dead_end", [&](SIOClient* client, const std::string data) {
		log("Dead callback data: %s", data.c_str());
	});
}

void BattleAPI::sendRepawnEvent()
{
	auto c = NodeServer::getInstance()->getClient();
	if (c == nullptr) return;
	c->emit("respawn", "{\"data\": \"test respwan\"}");
	c->on("respawn_end", [&](SIOClient* client, const std::string data){
		log("attack_end data :%s", data.c_str());
	});
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

void BattleAPI::sendBattleEndEvent()
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
	sv->on("battle_end_end", [&](SIOClient*, const std::string&) {
		log("server callback");
	});
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

	return unitDataValue;
}
Document::GenericValue* BattleAPI::convertSkillDataToJsonObject(UserSkillInfo skillData, Document::AllocatorType& allo)
{

	rapidjson::Value *skillDataValue = new rapidjson::Value();
	skillDataValue->SetObject();
	skillDataValue->AddMember("mst_unit_id", skillData.mst_skill_id, allo);
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



