#include "BattleModel.h"

BattleModel* BattleModel::s_BattleModel = nullptr;
BattleModel* BattleModel::getInstance()
{
	if (!s_BattleModel) {
		s_BattleModel = new (std::nothrow)BattleModel();
		s_BattleModel->init();
	}
	return s_BattleModel;
}

bool BattleModel::init()
{
	setPlayerSkills({});
	return true;
}

//received data from server and parser for initial battle information (selected units, skills, player information....)
void BattleModel::parserJsonToInitData(string jsonData)
{
	log("battle start data %s", jsonData.c_str());

	vector<int> skillIds = {};
	rapidjson::Document doc;
	doc.Parse<0>(jsonData.c_str());
	if (doc.HasParseError()) {
		log("Battle Model: error in parser Json data");
		return;
	}
	if (doc.IsObject()/*doc.IsObject() && doc.HasMember("args")*/) {
		auto a = UserModel::getInstance()->getUuId();
		vector<Room_User_Model> userList;
		vector<Room_User_Unit_Model> unitList;
		for (int i = 0; i < doc/*["args"][rapidjson::SizeType(0)]*/["room_user"].Size(); i++)
		{
			Room_User_Model temp;
			temp._id = doc["room_user"][i]["_id"].GetString();
			temp.npc = doc["room_user"][i]["npc"].GetInt();
			temp.state = doc["room_user"][i]["state"].GetInt();
			temp.ready = doc["room_user"][i]["ready"].GetInt();
			temp.team_id = doc["room_user"][i]["team_id"].GetInt();
			temp.user_id = doc["room_user"][i]["user_id"].GetInt();
			temp.room_id = doc["room_user"][i]["room_id"].GetInt(); 
			temp.uuid = doc["room_user"][i]["room_user_id"].GetString();
			//uuid
			userList.push_back(temp);
		}
		setRoomUserList(userList);

		log("BattleModel: Room use unit array length: %d", doc["room_user_unit"].Size());
		for (int i = 0; i < doc["room_user_unit"].Size(); i++)
		{
			Room_User_Unit_Model temp1;
			temp1._id = doc["room_user_unit"][i]["_id"].GetString();
			temp1.status = doc["room_user_unit"][i]["status"].GetInt();
			temp1.mp = doc["room_user_unit"][i]["mp"].GetInt();
			temp1.hp = doc["room_user_unit"][i]["hp"].GetInt();
			temp1.position_x = doc["room_user_unit"][i]["position_x"].GetDouble();
			temp1.position_y = doc["room_user_unit"][i]["position_y"].GetDouble();
			temp1.direction = doc["room_user_unit"][i]["direction"].GetInt();
			temp1.team_id = doc["room_user_unit"][i]["team_id"].GetInt();
			temp1.user_id = doc["room_user_unit"][i]["user_id"].GetInt();
			temp1.room_id = doc["room_user_unit"][i]["room_id"].GetInt();
			temp1.mst_unit_id = doc["room_user_unit"][i]["mst_unit_id"].GetInt();
			temp1.uuid = doc["room_user_unit"][i]["uuid"].GetString();
			temp1.angle = doc["room_user_unit"][i]["angle"].GetInt();
			temp1.moving = doc["room_user_unit"][i]["moving"].GetBool();
			unitList.push_back(temp1);
		}
		setRoomUserUnitList(unitList);
	}
	else {
		log("Battle Model: wrong data %d", doc.GetType());
		return;
	}
}

Room_User_Unit_Model BattleModel::getUnitInforByUuid(string uuid)
{
	auto unitList = getRoomUserUnitList();
	for (auto &unit : unitList)
	{
		if (strcmp(unit.uuid.c_str(), uuid.c_str()) == 0)
		{
			return unit;
		}
	}
	log("find error!");
}

vector<Room_User_Unit_Model> BattleModel::getTeamUserUnitList(int teamId)
{
	auto unitList = getRoomUserUnitList();
	vector<Room_User_Unit_Model> teamUnitList = {};
	for (auto &unit : unitList)
	{
		if (unit.team_id == teamId)
		{
			teamUnitList.push_back(unit);
		}
	}
	return teamUnitList;
}

void BattleModel::updateUserUnit(Room_User_Unit_Model unitData)
{
	auto tempList = getRoomUserUnitList();
	for (auto &a : tempList)
	{
		if (strcmp(a.uuid.c_str(), unitData.uuid.c_str()) == 0) {

			break;
		}
	}
	setRoomUserUnitList(tempList);
	//log("BattleModel: updated unit with uuid: %s", unitData.uuid.c_str());
	return;
}

