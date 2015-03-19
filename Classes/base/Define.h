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

struct UnitInforNew 
{
	int id;
	string name;
	int hp;
	int hp_restore;
	int mp;
	int mp_restore;
	int attack_dame;
	int defence;
	int attack_sight;
	int move_speed;
	int attr;
	int type;
	string image;
};

struct SkillInfoNew {
	int id;
	string name;
	int aoe;
	int target_type;
	int mp_cost;
	int cooldown;
	int skill_type;
	int dame_type;
	int dame_value;
	int duration;
	string effect;
	string plistpath;
	string icon;

};
#define TYPE_HELP 1
#define TYPE_RESTORE 2
#define TYPE_ATTACK 3

#define TARGET_ALL 0
#define TARGET_ONE 1

#define DAME_TYPE_PERCENT 1
#define DAME_TYPE_PURE 2


#define BASE_SPACE_X 120
#define BOLDER_TAG 77
#define SCREEN_TRANSI_DELAY 0.5
#define MULTI_MODE 2
#define SOLO_MODE 1

#define BUTON_MARGIN 200

#define PI 3.1415926535897932385f
#define DEG_RAD (PI / 180)
#define RAD_DEG (180 / PI)
#endif