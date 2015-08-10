#ifndef ROOM_MODEL_H__
#define ROOM_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserSkillModel.h"
#include "Model/data/UserUnitModel.h"
#include "Model/data/UserModel.h"

#include "Server/API/ListUserPHPAPI.h"
#include "Server/Server.h"
#include "base/database/DataUtils.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

USING_NS_CC;
using namespace rapidjson;
using namespace std;


class  RoomUserModel :public Ref
{
public:
	static RoomUserModel * getInstance();
	static void destroyInstance() {
		s_RoomModel = nullptr;
	};
	bool init();
	RoomUserModel();
	~RoomUserModel();

	CC_SYNTHESIZE(int, _roomId, RoomId);
	CC_SYNTHESIZE(vector<RoomUser>, _roomUserList,RoomUserList);
	CC_SYNTHESIZE(vector<RoomUser>, _redTeamUserList, RedTeamUserList);
	CC_SYNTHESIZE(vector<RoomUser>, _blueTeamUserList, BlueTeamUserList);
	CC_SYNTHESIZE(vector<TeamSkill>, _teamSkillList, TeamSkillList);
	CC_SYNTHESIZE(vector<TeamUnit>, _teamUnitList, TeamUnitList);
	/*
	 * Function to parser and init Team user unit list and team user skill list
	 */
	void parseTeamData(const string& data);

	/*this function will parse data received when select user was done. Parsed data will use to
	create room 's user list*/
	void initDataWhenJoinRoom(string jsonData);

	RoomUser parseJsonToRoomUserData(string data);

	void addUserInfoToUserList(RoomUser data);
	void setTeamForUserByUserId(int room_id, int user_id, int team_id);
	void updateTeamUserList();
	void updateTeamUnitSkillList(string uuid);

private:
	static RoomUserModel * s_RoomModel;
	CC_SYNTHESIZE(string, _tempData, TempData);
};

#endif