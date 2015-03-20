#ifndef SKILL_DATA_H__
#define SKILL_DATA_H__

#include "base/LayerBase.h"
#include "base/Define.h"

#define DATAFILE "database.db3"
class SkillData
{
public:

	/*Get unit skill data by unit id*/
	static vector<SkillInfoNew> getUnitSkillsByUnitId(int unitId);
	/*get skill data by skill id*/
	static SkillInfoNew getSkillInfoBySkillId(int skillId);

	/*get player skill data by player id*/
	static vector<SkillInfoNew> getPlayerSkillDataByPlayerId(int playerId);

	/*get all player skill data in database*/
	static vector<SkillInfoNew> getAllPlayerSkillData();

	/*get Player skill data by skill id*/
	static SkillInfoNew getPlayerSkillDataBySkillId(int skillId);

	/**/
	static SkillInfoNew convertData(vector<string> item);

private:

};

#endif