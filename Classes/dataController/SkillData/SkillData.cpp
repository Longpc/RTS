#include "SkillData.h"


vector<SkillInfoNew> SkillData::getUnitSkillsByUnitId(int unitId)
{
	vector<SkillInfoNew> returnData;
	string sql = "SELECT skill.* FROM unit_skill JOIN unit ON unit.id = unit_skill.unitId JOIN skill ON skill.id = unit_skill.skillId WHERE unit.id = ";
	sql.append(DataUtils::numberToString(unitId));
	sqlite3 *data = SqlUtil::openData(DATAFILE);

	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	for (auto &item : a)
	{
		returnData.push_back(convertData(item));
	}

	return returnData;
}

SkillInfoNew SkillData::getSkillInfoBySkillId(int skillId)
{
	SkillInfoNew a;
	return a;
}
vector<SkillInfoNew> SkillData::getPlayerSkillDataByPlayerId(int playerId)
{
	vector<SkillInfoNew> temp;
	return temp;
}

vector<SkillInfoNew> SkillData::getAllPlayerSkillData()
{
	vector<SkillInfoNew> allSkill;
	sqlite3 *data = SqlUtil::openData(DATAFILE);
	string sql = "select * from player_skill";
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	for (auto &item : a)
	{
		allSkill.push_back(convertData(item));
	}
	return allSkill;
}

SkillInfoNew SkillData::getPlayerSkillDataBySkillId(int skillId)
{
	sqlite3 *data = SqlUtil::openData(DATAFILE);
	string sql = "select * from player_skill where player_skill.id =";
	sql.append(DataUtils::numberToString(skillId));
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	if (a.size() > 0)
	{
		return convertData(a[0]);
	}
	SkillInfoNew b;
	return b;
}
SkillInfoNew SkillData::convertData(vector<string> item)
{
	SkillInfoNew temp;
	temp.mst_skill_id = DataUtils::stringToFloat(item[0].c_str());
	temp.name = item[1];
	temp.aoe = DataUtils::stringToFloat(item[2].c_str());
	temp.target_type = DataUtils::stringToFloat(item[3].c_str());
	temp.mp_cost = DataUtils::stringToFloat(item[4].c_str());
	temp.cooldown = DataUtils::stringToFloat(item[5].c_str());
	temp.skill_type = DataUtils::stringToFloat(item[6].c_str());
	temp.dame_type = DataUtils::stringToFloat(item[7].c_str());
	temp.dame_value = DataUtils::stringToFloat(item[8].c_str());
	temp.duration = DataUtils::stringToFloat(item[9].c_str());
	temp.effect = (item[10].c_str());
	temp.plistpath = (item[11].c_str());
	temp.icon = item[12].c_str();
	temp.skill_help_type = DataUtils::stringToFloat(item[13].c_str());
	temp.area_type = DataUtils::stringToFloat(item[14].c_str());

	return temp;
}





