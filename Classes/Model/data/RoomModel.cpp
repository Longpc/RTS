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
		if (doc.IsObject()) {
			int team_id = doc["team_id"].GetInt();
			int room_id = doc["room_id"].GetInt();
			int user_id = doc["user_id"].GetInt();

			setTeamForUserByUserId(room_id, user_id, team_id);
			log("data updated");
		}
		else {
			log("something wrong: %d", doc.GetType());
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

void RoomModel::initDataWhenJoinRoom(string jsonData)
{
	rapidjson::Document doc;
	doc.Parse<0>(jsonData.c_str());
	if (doc.HasParseError()) {
		log("error parser Json");
		return;
	}
	if (doc.IsArray()) {
		vector<RoomUser> tempList = {};
		log("Data size: %d", doc.Size());
		for (int i = 0; i < doc.Size(); i++)
		{
			RoomUser temp; 
			temp.room_id = doc[i]["room_id"].GetInt();
			temp.state = doc[i]["state"].GetInt();
			temp.team_id = doc[i]["team_id"].GetInt();
			temp.user_id = doc[i]["user_id"].GetInt();
			temp._uuid = doc[i]["room_user_id"].GetString();
			tempList.push_back(temp);
		}
		setRoomUserList(tempList);
		log("init data when connected completed");
	}
	else {
		log("Data type: %d", doc.GetType());
	}
}
