#ifndef SKILL_DATA_H__
#define SKILL_DATA_H__

#include "base/LayerBase.h"
#include "base/Define.h"

#define DATAFILE "database.db3"
class SkillData
{
public:

	static vector<SkillInfoNew> getUnitSkillsByUnitId(int unitId);
	static SkillInfoNew getSkillInfoBySkillId(int skillId);
	static SkillInfoNew convertData(vector<string> item);

private:

};

#endif