#pragma execution_character_set("utf-8")
#include "RoomModel.h"

RoomModel* RoomModel::s_RoomModel = nullptr;

RoomModel* RoomModel::getInstance() {
	if (!s_RoomModel) {
		s_RoomModel = new (std::nothrow)RoomModel();
		s_RoomModel->init();
	}
	return s_RoomModel;
}

bool RoomModel::init()
{
	setRoomUserList({});
	setRedTeamUserList({});
	setBlueTeamUserList({});

	auto sv = NodeServer::getInstance()->getClient();

	//handler for another player join room event
	sv->on("room_public_connect", [&](SIOClient* client, const std::string& data) {
		initDataWhenJoinRoom(data);
	});
	//handler for another player disconnect event
	sv->on("room_public_disconnect", [&](SIOClient* client, const std::string& data) {
		log("room_public_disconnect with data: %s", data.c_str());
		initDataWhenJoinRoom(data);
	});

	//handler for another player select team event
	sv->on("room_public_select_team_end", [&](SIOClient * client, const string& data) {
		log("team select public data: %s", data.c_str());
		rapidjson::Document doc;
		doc.Parse<0>(data.c_str());
		if (doc.HasParseError()) {
			log("error parser Json in room_public_connect");
			return;
		}
		if (doc.IsObject() && doc.HasMember("args")) {
			rapidjson::Value &val = doc["args"];
			int team_id = val[rapidjson::SizeType(0)]["team_id"].GetInt();
			int room_id = val[rapidjson::SizeType(0)]["room_id"].GetInt();
			int user_id = val[rapidjson::SizeType(0)]["user_id"].GetInt();

			setTeamForUserByUserId(room_id, user_id, team_id);
			log("data updated");
		}


	});

	//handler for another player selected unit event
	sv->on("room_public_select_unit_end", [&](SIOClient *client, const string& data) {
		log("unit select public data: %s", data.c_str());

	});

	//handler for another player select skill event
	sv->on("room_public_select_skill_end", [&](SIOClient *client, const string& data) {
		log("room_public_select_skill_end: %s", data.c_str());

	});
	return true;
}

void RoomModel::addUserInfoToUserList(RoomUser data)
{
	auto a = getRoomUserList();
	a.push_back(data);
	setRoomUserList(a);
}

void RoomModel::setTeamForUserByUserId(int room_id, int user_id, int team_id)
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
}

RoomUser RoomModel::parseJsonToRoomUserData(string data)
{
	RoomUser temp;
	rapidjson::Document doc;
	doc.Parse<0>(data.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		 return temp;
	}
	if (doc.IsObject() && doc.HasMember("args")) {
		rapidjson::Value &val = doc["args"];
		temp.room_id = val[rapidjson::SizeType(0)]["room_id"].GetInt();
		temp.user_id = val[rapidjson::SizeType(0)]["user_id"].GetInt();
		temp._uuid = val[rapidjson::SizeType(0)]["uuid"].GetString();
	}
	return temp;
}

void RoomModel::initDataWhenJoinRoom(string jsonData)
{
	rapidjson::Document doc;
	doc.Parse<0>(jsonData.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		return;
	}
	if (doc.IsObject() && doc.HasMember("args")) {
		rapidjson::Value &val = doc["args"];
		vector<RoomUser> tempList = {};
		log("Data size: %d", val[rapidjson::SizeType(0)].Size());
		for (int i = 0; i < val[rapidjson::SizeType(0)].Size(); i++)
		{
			RoomUser temp; 
			temp.room_id = val[rapidjson::SizeType(0)][i]["room_id"].GetInt();
			temp.state = val[rapidjson::SizeType(0)][i]["state"].GetInt();
			temp.team_id = val[rapidjson::SizeType(0)][i]["team_id"].GetInt();
			temp.user_id = val[rapidjson::SizeType(0)][i]["user_id"].GetInt();
			temp._uuid = val[rapidjson::SizeType(0)][i]["room_user_id"].GetString();
			tempList.push_back(temp);
		}
		setRoomUserList(tempList);
		log("init data when connected completed");
	}
}
