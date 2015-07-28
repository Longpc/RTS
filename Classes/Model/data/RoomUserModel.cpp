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
			log("error parser Json in room_public_select_team_end");
			return;
		}
		if (doc.IsObject()) {
			int team_id = doc["team_id"].GetInt();
			int room_id = doc["room_id"].GetInt();
			int user_id = doc["user_id"].GetInt();

			setTeamForUserByUserId(room_id, user_id, team_id);
			log("data updated");
			NotificationCenter::getInstance()->postNotification("update_team", (Ref*)(intptr_t)user_id);
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

void RoomUserModel::addUserInfoToUserList(RoomUser data)
{
	auto a = getRoomUserList();
	a.push_back(data);
	setRoomUserList(a);
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
			tempList.push_back(temp);
		}
		setRoomUserList(tempList);
		log("init data when connected completed");
		NotificationCenter::getInstance()->postNotification("switchUserButton", (Ref*)(intptr_t)doc["selectedId"].GetInt());
	}
	else {
		log("Data type: %d", doc.GetType());
	}
}
