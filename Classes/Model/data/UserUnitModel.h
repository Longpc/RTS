#ifndef USER_UNIT_MODEL_H__
#define USER_UNIT_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "base/database/DataUtils.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

struct UserUnitInfo
{
	int mst_unit_id = 0;
	string name = "default name";
	int hp = 0;
	int hp_heal = 0;
	int mp = 0;
	int mp_heal = 0;
	int attack = 0;
	int attack_range = 0;
	int attack_speed = 0;
	int defence = 0;
	int move_speed = 0;
	int element = 0;
	int skill1_id = 0;
	int skill2_id = 0;
	bool isStun = false;
};
USING_NS_CC;
using namespace std;
using namespace rapidjson;

class UserUnitModel : public Ref
{
public:
	static UserUnitModel *getInstance();

	vector<UserUnitInfo> createUserUnitDataFromJson(rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& value);
	CC_SYNTHESIZE(vector<UserUnitInfo>, _userUnits, UserUnitList);
	UserUnitInfo getUnitInfoById(int mst_unit_id);
	string getUnitImageById(int id);
	Document::GenericValue* convertFromUserUnitInfoToJson(UserUnitInfo unitInfo, Document::AllocatorType& allo);


private:
	static UserUnitModel *s_UserUnit;

};


#endif