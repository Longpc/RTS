#include "UserSkill.h"

UserSkill* UserSkill::s_UserSkill = nullptr;

UserSkill * UserSkill::getInstance()
{
	if (!s_UserSkill) {
		s_UserSkill = new (std::nothrow)UserSkill();
		s_UserSkill->setUserSkillList({});
		s_UserSkill->setPlayerSkillsList({});
	}
	return s_UserSkill;
}

vector<UserSkillInfo> UserSkill::createUserSkillDataFromJson(rapidjson::GenericValue<rapidjson::UTF8<char>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>>& value)
{
	vector<UserSkillInfo> vInfo;
	vector<UserSkillInfo> pInfo;
	UserSkillInfo info;
	for (int i = 0; i < value.Size(); i++)
	{
		info.mst_skill_id = DataUtils::stringToFloat(value[i]["mst_skill_id"].GetString());
		info.name = value[i]["name"].GetString();
		info.skill_type = DataUtils::stringToFloat(value[i]["skill_type"].GetString());
		info.mp_cost = DataUtils::stringToFloat(value[i]["mp_cost"].GetString());
		info.cooldown_time = DataUtils::stringToFloat(value[i]["cooldown_time"].GetString());
		info.range_type = DataUtils::stringToFloat(value[i]["range_type"].GetString());
		info.range_distance = DataUtils::stringToFloat(value[i]["range_distance"].GetString());
		info.multi_effect = DataUtils::stringToFloat(value[i]["multi_effect"].GetString());
		info.target_type = DataUtils::stringToFloat(value[i]["target_type"].GetString());
		info.effect_type = DataUtils::stringToFloat(value[i]["effect_type"].GetString());
		info.buff_effect_type = DataUtils::stringToFloat(value[i]["buff_effect_type"].GetString());
		info.duration = DataUtils::stringToFloat(value[i]["effect_duration"].GetString());
		info.correct_type = DataUtils::stringToFloat(value[i]["correct_type"].GetString());
		info.corrett_value = DataUtils::stringToFloat(value[i]["corrett_value"].GetString());
		info.skill_des = value[i]["description"].GetString();
		info.image_path = value[i]["image_path"].GetString();
		info.skill_icon_path = value[i]["skill_icon_path"].GetString();

		vInfo.push_back(info);
		if (info.skill_type == 1) {
			pInfo.push_back(info);
		}
		log("skill %d name: %s", i+1, value[i]["name"].GetString());
	}
	setUserSkillList(vInfo);
	setPlayerSkillsList(pInfo);
	return getUserSkillList();
}

UserSkillInfo UserSkill::getSkillInfoById(int mst_skill_id)
{
	for (auto & skill : getUserSkillList())
	{
		if (skill.mst_skill_id == mst_skill_id)
		{
			return skill;
		}
	}
}
