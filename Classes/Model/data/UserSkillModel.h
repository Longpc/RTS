#ifndef USER_SKILL_MODEL_H__
#define USER_SKILL_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "base/database/DataUtils.h"

#include "json/rapidjson.h"
#include "json/document.h"

struct UserSkillInfo 
{
	int mst_skill_id = 0;
	string name = "default name";
	int skill_type = 0;
	int mp_cost = 0;
	int cooldown_time = 0;
	int range_type = 0;
	int range_distance = 0;
	int multi_effect = 0;
	int target_type = 1;
	int effect_type = 0;
	int buff_effect_type = 0;
	int duration = 0;
	int correct_type = 0;
	int corrett_value = 0;
	string skill_des = "this is skill";
	string image_path = "default.png";
	string skill_icon_path = "default.png";
};

USING_NS_CC;
using namespace std;
using namespace rapidjson;
class UserSkillModel :public Ref
{
public:
	static UserSkillModel *getInstance();

	vector<UserSkillInfo> createUserSkillDataFromJson(rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& value);

	UserSkillInfo getSkillInfoById(int mst_skill_id);

	CC_SYNTHESIZE(vector<UserSkillInfo>, _userSkills, UserSkillList);
	CC_SYNTHESIZE(vector<UserSkillInfo>, _playerSkills, PlayerSkillsList);
protected:
private:
	static UserSkillModel *s_UserSkill;
};
#endif