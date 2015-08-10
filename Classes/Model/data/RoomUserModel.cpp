#pragma execution_character_set("utf-8")
#include "RoomUserModel.h"

RoomUserModel* RoomUserModel::s_RoomModel = nullptr;

RoomUserModel* RoomUserModel::getInstance() {
	if (!s_RoomModel) {
		s_RoomModel = new (std::nothrow)RoomUserModel();
		s_RoomModel->init();
	}
	return s_RoomModel;
}

bool RoomUserModel::init()
{
	auto sv = NodeServer::getInstance()->getClient();

	//handler for another player join room event
	sv->on("room_public_connect", [&](SIOClient* client, const std::string& data) {
		CC_UNUSED_PARAM(client);
		initDataWhenJoinRoom(data);
	});
	//handler for another player disconnect event
	sv->on("room_public_disconnect", [&](SIOClient* client, const std::string& data) {
		CC_UNUSED_PARAM(client);
		log("room_public_disconnect with data: %s", data.c_str());
		initDataWhenJoinRoom(data);
	});

	//handler for another player select team event
	sv->on("room_public_select_team_end", [&](SIOClient * client, const string& data) {
		CC_UNUSED_PARAM(client);
		log("team select public data: %s", data.c_str());
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("error parser Json in room_public_select_team_end");
			return;
		}
		if (doc.IsObject()) {
			int team_id = doc["team_id"].GetInt();
			int room_id = doc["room_id"].GetInt();
			int user_id = doc["user_id"].GetInt();
			string uuid = doc["uuid"].GetString();

			setTeamForUserByUserId(room_id, user_id, team_id);
			if (team_id != UserModel::getInstance()->getUserInfo().team_id)
			{
				updateTeamUnitSkillList(uuid.c_str());
			}
			parseTeamData(data);
			log("data updated");
			NotificationCenter::getInstance()->postNotification("update_team", (Ref*)(intptr_t)user_id);
			
		}
		else {
			log("something wrong: %d", doc.GetType());
		}
	});
	sv->on("out_team", [&](SIOClient *c, const string& data) {
		CC_UNUSED_PARAM(c);
		log("out team: %s", data.c_str());
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("error parser Json in out_team");
			return;
		}
		if (doc.IsObject()) {
			auto user_id = doc["user_id"].GetInt();
			auto room_id = doc["room_id"].GetInt();
			setTeamForUserByUserId(room_id, user_id, 0);
			NotificationCenter::getInstance()->postNotification("update_team", (Ref*)(intptr_t)user_id);
		}
	});
	//handler for another player selected unit event
	sv->on("room_public_select_unit_end", [&](SIOClient *client, const string& data) {
		CC_UNUSED_PARAM(client);
		log("unit select public data: %s", data.c_str());
		this->setTempData(data.c_str());
		this->parseTeamData(data);
	});

	//handler for another player select skill event
	sv->on("room_public_select_skill_end", [&](SIOClient *client, const string& data) {
		CC_UNUSED_PARAM(client);
		log("room_public_select_skill_end: %s", data.c_str());
		this->setTempData(data.c_str());
		this->parseTeamData(data);
	});
	return true;
}

void RoomUserModel::addUserInfoToUserList(RoomUser data)
{
	auto a = getRoomUserList();
	a.push_back(data);
	setRoomUserList(a);
	updateTeamUserList();
}

void RoomUserModel::setTeamForUserByUserId(int room_id, int user_id, int team_id)
{
	auto userList = getRoomUserList();
	for (auto &user : userList)
	{
		if (user.room_id == room_id && user.user_id == user_id) {
			user.team_id = team_id;
			break;
		}
	}
	setRoomUserList(userList);
	updateTeamUserList();
}

void RoomUserModel::updateTeamUserList()
{
	auto roomUserList = this->getRoomUserList();
	vector<RoomUser> tempList1;
	vector<RoomUser> tempList2;
	for (int i = 0; i < roomUserList.size(); i++)
	{
		if (roomUserList[i].team_id == TEAM_FLG_BLUE)
		{
			roomUserList[i].name = ListUserAPI::getInstance()->getUserNameByUserId(roomUserList[i].user_id);
			tempList1.push_back(roomUserList[i]);
			continue;
		}
		if (roomUserList[i].team_id == TEAM_FLG_RED) {
			roomUserList[i].name = ListUserAPI::getInstance()->getUserNameByUserId(roomUserList[i].user_id);
			tempList2.push_back(roomUserList[i]);
			continue;
		}
	}
	this->setBlueTeamUserList(tempList1);
	this->setRedTeamUserList(tempList2);

}

void RoomUserModel::updateTeamUnitSkillList(string uuid)
{
	auto teamUnitList = getTeamUnitList();
	if (teamUnitList.size() > 0) 
	{
		for (int i = 0; i < teamUnitList.size(); i ++)
		{
			if (strcmp(teamUnitList[i].uuid.c_str(), uuid.c_str()) == 0)
			{
				teamUnitList.erase(teamUnitList.begin() + i);
				break;
			}
		}
	}
	auto teamSKilList = getTeamSkillList();
	if (teamSKilList.size() > 0) 
	{
		int foundNum = 0;
		for (int j = 0; j < teamSKilList.size(); j ++)
		{
			if (strcmp(teamSKilList[j].uuid.c_str(), uuid.c_str()) == 0)
			{
				teamSKilList.erase(teamSKilList.begin() + j);
				j -= 1;
				foundNum += 1;
				if (foundNum == 2) break;
			}
		}
	}
	setTeamUnitList(teamUnitList);
	setTeamSkillList(teamSKilList);
}

RoomUser RoomUserModel::parseJsonToRoomUserData(string data)
{
	RoomUser temp;
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		 return temp;
	}
	if (doc.IsObject()) {
		temp.room_id = doc["room_id"].GetInt();
		temp.user_id = doc["user_id"].GetInt();
		temp._uuid = doc["uuid"].GetString();
	}
	else {
		log("RoomModel parse JSON type %d", doc.GetType());
	}
	return temp;
}

void RoomUserModel::initDataWhenJoinRoom(string jsonData)
{
	rapidjson::Document doc;
	doc.Parse<0>(jsonData.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		return;
	}
	if (doc.IsObject()) {
		vector<RoomUser> tempList = {};
		log("Data size: %d", doc["data"].Size());
		for (int i = 0; i < doc["data"].Size(); i++)
		{
			RoomUser temp; 
			temp.room_id = doc["data"][i]["room_id"].GetInt();
			temp.state = doc["data"][i]["state"].GetInt();
			temp.team_id = doc["data"][i]["team_id"].GetInt();
			temp.user_id = doc["data"][i]["user_id"].GetInt();
			temp._uuid = doc["data"][i]["room_user_id"].GetString();
			temp._ready = doc["data"][i]["ready"].GetInt();
			tempList.push_back(temp);
		}
		setRoomUserList(tempList);
		updateTeamUserList();
		log("init data when connected completed");
		NotificationCenter::getInstance()->postNotification("switchUserButton", (Ref*)(intptr_t)doc["selectedId"].GetInt());
	}
	else {
		log("Data type: %d", doc.GetType());
	}
}

void RoomUserModel::parseTeamData(const string& data)
{
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		return;
	}
	if (doc.IsObject()) {
		if (doc["team_id"].GetInt() != UserModel::getInstance()->getUserInfo().team_id) return;
		if (doc["unit"].Size() > 0) 
		{
			vector<TeamUnit> tempUnits = {};
			for (int i = 0; i < doc["unit"].Size(); i ++)
			{
				TeamUnit tempU;
				tempU.mst_unit_id = doc["unit"][rapidjson::SizeType(i)]["mst_unit_id"].GetInt();
				tempU.uuid = doc["unit"][rapidjson::SizeType(i)]["uuid"].GetString();
				tempUnits.push_back(tempU);
			}
			this->setTeamUnitList(tempUnits);
		}
		if (doc["skill"].Size() > 0)
		{
			vector<TeamSkill> tempSkills = {};
			for (int j = 0; j < doc["skill"].Size(); j ++)
			{
				TeamSkill tempS;
				tempS.mst_skill_id = doc["skill"][rapidjson::SizeType(j)]["mst_skill_id"].GetInt();
				tempS.user_id = doc["skill"][rapidjson::SizeType(j)]["user_id"].GetInt();
				tempS.uuid = doc["skill"][rapidjson::SizeType(j)]["uuid"].GetString();
				tempS.skill_index = doc["skill"][rapidjson::SizeType(j)]["skill_index"].GetInt();
				tempSkills.push_back(tempS);
			}
			this->setTeamSkillList(tempSkills);
		}
		NotificationCenter::getInstance()->postNotification(TEAM_DATA_UPDATE_MSG, this);
	}
}

RoomUserModel::RoomUserModel()
{
	_roomId = 0;
	_roomUserList = {};
	_redTeamUserList = {};
	_blueTeamUserList = {};
	_teamSkillList = {};
	_teamUnitList = {};
}

RoomUserModel::~RoomUserModel()
{

}
