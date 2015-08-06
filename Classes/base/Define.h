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

#define DESIGN_RESOLUTION_W 960
#define DESIGN_RESOLUTION_H 640

struct Room_User_Unit_Model
{
	string _id = "";
	int status = 0;
	int mp = 0;
	int hp = 0;
	float position_x = 0;
	float position_y = 0;
	int direction = 0;
	int team_id = 0;
	int room_id = 0;
	int user_id = 0;
	int mst_unit_id = 0;
	string uuid = "";
	int angle = 0;
	bool moving = false;
};

//Double bellow
struct Room_User_Model
{
	string _id = "";
	int npc = 0;
	int state = 0;
	int ready = 0;
	int team_id = 0;
	int user_id = 0;
	int room_id = 0;
	string uuid = "";
};
struct RoomUser
{
	string _uuid;
	int room_id = 0;
	int user_id = 0;
	int team_id = 0;
	int _ready = 0;
	int state = 0;
	int npc = 0;
	string name;
};

struct TeamUnit
{
	string uuid;
	int mst_unit_id;
};

struct TeamSkill
{
	string uuid;
	int user_id;
	int mst_skill_id;
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
	int attack_range;
	int move_speed;
	int attr;
	int type;
	string image;
	int attack_delay;
	bool isStun = false;
};

struct SkillInfoNew {
	int mst_skill_id;
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
#define ATTACK_AOE 110.0f

//skill.effect_type values range. 
#define TYPE_BUFF 1
#define TYPE_RESTORE 2
#define TYPE_ATTACK 3
#define TYPE_POISON 4
#define TYPE_STUN 5
#define TYPE_TRAP 6
#define TYPE_SUMMON 7
#define TYPE_PET 8

#define TARGET_ALL 1
#define TARGET_ONE 0

#define DAME_TYPE_PERCENT 2
#define DAME_TYPE_PURE 1

#define TEAM_FLG_RED 2
#define TEAM_FLG_BLUE 1

#define ATTACK_ANIMATION_DELAY 1

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

enum SKILL_RANGE_TYPE
{
	CIRCLE = 1,
	RECTANGLE = 2,
	TRIANGLE = 3,
	STAR = 4,
	CAKE = 5,
	FADE_RECTANGLE = 6
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

////////////////////////////////////////////////////////////
// MOVE KEY VALUE
////////////////////////////////////////////////////////////
#define SCREEN_HORIZONTAL 888
#define SCREEN_VERTICAL 999

#define MOVE_AUTO 1
#define MOVE_MANUAL 2
#define MOVE_CIRCLE 3

#define MOVE_CIRCLE_LEFT 1
#define MOVE_CIRCLE_RIGHT 2

#define MOVE_CIRCLE_TIME 1
#define MOVE_CIRCLE_DISTANCE 2

#define AUTO_MOVE_ACTION_TAG 9876
#define MOVE_DISTANCE 540

#define SPEED_MULTIPLE 2

#define SCR_ORIEN_K "THEKEY"
#define MOVE_KEY "MOVE_MODE"
#define MOVE_CIRCLE_TYPE "MOVE_CIRCLE_TYPE"
#define NOVE_CIRCLE_PROPERTY "MOVE_CIRCLE_PROPERTY"

/*Notification messages name*/
#define TEAM_DATA_UPDATE_MSG "team_data_update"
#define SELECT_UNIT_END_MSG "Select_unit"
#define SELECT_SKILL_END_MSG "select_skill"
#define READY_FOR_BATTLE "Ready"

#define CONNECTED_MSG "Connected"
#define DISCONNECT_MSG "Disconnect"
#define MOVESV_CONNECTED_MSG "MV_CONNECTED"
#define MOVESV_DISCONNECTED_MSG "MV_DISCONNECTED"
#define MAPSV_CONNECTED_MSG "MAP_CONNECTED"
#define MAPSV_DISCONNECTED_MSG "MAP_DISCONNECTED"

#define RECONNECT_DELAY 0.5f

#define MINU_CIRCLE_SCALE 2/5


#define MOVE_SPEED 250
#define IMAGE_SCALE 5.325f
#define ANIMETE_DELAY 0.15f
#define ATTACK_ANIMATION_DELAY 1

#define UI_SCENE_CREATE_FUNC(UIScene) \
public: \
static Scene* createScene() \
		{ \
    Scene* pScene = Scene::create(); \
    UIScene* uiLayer = new UIScene(); \
    if (uiLayer && uiLayer->init()) \
		    { \
        uiLayer->autorelease(); \
        pScene->addChild(uiLayer); \
		    } \
			    else \
		    { \
        CC_SAFE_DELETE(uiLayer); \
		    } \
    return pScene; \
		}





#endif