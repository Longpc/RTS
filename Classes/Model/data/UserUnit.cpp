#include "UserUnit.h"

UserUnit* UserUnit::s_UserUnit = nullptr;
UserUnit * UserUnit::getInstance()
{
	if (!s_UserUnit)
	{
		s_UserUnit = new (std::nothrow) UserUnit();
		s_UserUnit->setUserUnitList({});
	}
	return s_UserUnit;
}

vector<UserUnitInfo> UserUnit::createUserUnitDataFromJson(rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& value)
{
	vector<UserUnitInfo> vInfo;
	UserUnitInfo info;
	log("Value type: %d", value.GetType());
	for (int i = 0; i < value.Size(); i++)
	{
		info.mst_unit_id = DataUtils::stringToFloat(value[i]["mst_unit_id"].GetString());
		info.name = value[i]["name"].GetString();
		info.hp = DataUtils::stringToFloat(value[i]["hp"].GetString());
		info.hp_heal = DataUtils::stringToFloat(value[i]["hp_heal"].GetString());
		info.mp = DataUtils::stringToFloat(value[i]["mp"].GetString());
		info.mp_heal = DataUtils::stringToFloat(value[i]["mp_heal"].GetString());
		info.attack = DataUtils::stringToFloat(value[i]["attack"].GetString());
		info.attack_range = DataUtils::stringToFloat(value[i]["attack_range"].GetString());
		info.attack_speed = DataUtils::stringToFloat(value[i]["attack_speed"].GetString());
		info.defence = DataUtils::stringToFloat(value[i]["defence"].GetString());
		info.move_speed = DataUtils::stringToFloat(value[i]["move_speed"].GetString());
		info.element = DataUtils::stringToFloat(value[i]["element"].GetString());
		info.skill1_id = DataUtils::stringToFloat(value[i]["mst_skill_id_1"].GetString());
		info.skill2_id = DataUtils::stringToFloat(value[i]["mst_skill_id_2"].GetString());

		vInfo.push_back(info);
		log("unit %d name: %s",i+1,value[i]["name"].GetString());
	}
	setUserUnitList(vInfo);
	return getUserUnitList();
}

std::string UserUnit::getUnitImageById(int id)
{
	switch (id)
	{
	case 1:
		return "image/unit/1.png";
	case  3:
		return "image/unit/3.png";
	case 4:
		return "image/unit/4.png";
	case 5:
		return "image/unit/5.png";
	case 2:
		return "image/unit/2.png";
	default:
		return "image/unit/1.png";
	}
}

UserUnitInfo UserUnit::getUnitInfoById(int mst_unit_id)
{
	for (auto &unit : getUserUnitList())
	{
		if (unit.mst_unit_id == mst_unit_id)
		{
			return unit;
		}
	}
}

string UserUnit::convertFromUserUnitInfoToJson(UserUnitInfo unitdata)
{
	Document doc;
	doc.SetObject();
	Document::AllocatorType& allo = doc.GetAllocator();
	doc.AddMember("mst_unit_id", unitdata.mst_unit_id, allo);
	doc.AddMember("hp_heal", unitdata.hp_heal, allo);
	doc.AddMember("hp", unitdata.hp, allo);
	doc.AddMember("mp", unitdata.mp, allo);
	doc.AddMember("mp_heal", unitdata.mp_heal, allo);
	doc.AddMember("attack", unitdata.attack,allo);
	doc.AddMember("attack_range", unitdata.attack_range, allo);
	doc.AddMember("attack_speed", unitdata.attack_speed, allo);
	doc.AddMember("defence", unitdata.defence, allo);
	doc.AddMember("move_speed", unitdata.move_speed, allo);
	doc.AddMember("element", unitdata.element, allo);
	doc.AddMember("position_x", 0, allo);
	doc.AddMember("position_y", 0, allo);
	doc.AddMember("mst_skill_id1", unitdata.skill1_id, allo);
	doc.AddMember("mst_skill_id2", unitdata.skill2_id, allo);
	StringBuffer  buffer;
	Writer<StringBuffer> writer(buffer);
	doc.Accept(writer);
	
	return buffer.GetString();
}

