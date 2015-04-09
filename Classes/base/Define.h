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
	int _id;
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
	int attack_delay;
	bool isStun = false;
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
	int skill_help_type;
	int area_type;

};

struct UserBattleInfo
{
	string name = "Temp";
	int unitId = 2;
	string imagePath ="image/unit/1(3).png";
	int killNum = 0;
	int deadNum = 0;
	int assistNum = 0;
	int longestKillstreak = 0;
	int maxKillCombo = 0;
	int totalDealDame = 0;
	int totalReceivedDame = 0;
};


//////////////////////////////////////////////////////////////////////////
// EFFECT PLIST FILE PATH
//////////////////////////////////////////////////////////////////////////

#define PARTICLE_DEFENCE_H "Effect/particle_defence_05s_h.plist"
#define PARTICLE_DEFENCE_V "Effect/particle_defence_05s_v.plist"
#define PARTICLE_FIRE "Effect/particle_fire.plist"
#define PARTICLE_HEAL "Effect/particle_heal_1s.plist"
#define PARTICLE_MOVESPEED "Effect/particle_movespeed.plist"
#define PARTICLE_THUNDER "Effect/particle_thunder.plist"

//////////////////////////////////////////////////////////////////////////
// SORCERY IMAGE PATH
//////////////////////////////////////////////////////////////////////////
#define SORCERY_BLACK "image/screen/battle/magic/200x200/magic_black200x200.png"
#define SORCERY_BLUE "image/screen/battle/magic/200x200/magic_blue200x200.png"
#define SORCERY_ORANGE "image/screen/battle/magic/200x200/magic_orange200x200.png"
#define SORCERY_GREEN "image/screen/battle/magic/200x200/magic_green200x200.png"
#define SORCERY_PINK "image/screen/battle/magic/200x200/magic_pink200x200.png"
#define SORCERY_PURPLE "image/screen/battle/magic/200x200/magic_purple200x200.png"
#define SORCERY_RED "image/screen/battle/magic/200x200/magic_red200x200.png"

//////////////////////////////////////////////////////////////////////////
// VALUE
//////////////////////////////////////////////////////////////////////////
#define ATTACK_AOE 100.0f
#define TYPE_HELP 1
#define TYPE_RESTORE 2
#define TYPE_ATTACK 3
#define TYPE_POISON 4
#define TYPE_STUN 5

#define TARGET_ALL 0
#define TARGET_ONE 1

#define DAME_TYPE_PERCENT 1
#define DAME_TYPE_PURE 2

#define TEAM_FLG_RED 1
#define TEAM_FLG_BLUE 2
enum SKILL_HELP_TYPE
{
	HP = 1,
	HP_RESTORE = 2,
	MP = 3,
	MP_RESTORE = 4,
	ATTACK_DAME = 5,
	DEFENCE = 6,
	ATTACK_RANGE = 7,
	MOVESPEED = 8
};

#define JAPANESE_FONT_1_HEAVY "fonts/AozoraMinchoHeavy.ttf"
#define JAPANESE_FONT_1_BOLD "fonts/AozoraMincho-bold.ttf"
#define JAPANESE_FONT_1_REGULAR "fonts/AozoraMinchoRegular.ttf"

#define BASE_SPACE_X 120
#define BOLDER_TAG 77
#define SCREEN_TRANSI_DELAY 0.5
#define MULTI_MODE 2
#define SOLO_MODE 1

#define BUTON_MARGIN 200

#define PI 3.1415926535897932385f
#define DEG_RAD (PI / 180)
#define RAD_DEG (180 / PI)
#define APP_KEY "12345678"

#define MOVE_AUTO 1
#define MOVE_MANUAL 2
#define MOVE_KEY "MOVE_MODE"
#define AUTO_MOVE_ACTION_TAG 9876

#endif