#ifndef DEFINE_H
#define DEFINE_H
#include "cocos2d.h"
using namespace std;
USING_NS_CC;
/*
User default information
_name: User name
_onRoom: Current room where user is playing. Default 0 is idle user
*/
struct UserInfo
{
	string _name;
	int _onTeam = 0;
};
struct UnitInfo
{
	string _name;
	int _unitId;
	string _imagePath;
	int _hp;
	int _attack;
	int _defence;
};
struct SkillInfo
{
	string _name;
	int _skillId;
	string _imagePath;
	string _description;
};

#define BASE_SPACE_X 120
#define BOLDER_TAG 77
#define SCREEN_TRANSI_DELAY 0.5
#define MULTI_MODE 2
#define SOLO_MODE 1

#define BUTON_MARGIN 200
#endif