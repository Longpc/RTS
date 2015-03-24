#ifndef UNIT_DATA_HELPER
#define UNIT_DATA_HELPER

#include "base/Define.h"
#include "base/LayerBase.h"

class UnitData
{
public:
	/*Get unit data from data base by unit ID*/
	static UnitInforNew getUnitDataById(int unitId);
	/*get all unit data*/
	static vector<UnitInforNew> getAllUnitData();
	/*convert data form*/
	static UnitInforNew convertInfo(vector<string> data);
	/*get tower information by team Flg: TEAM_FLG_RED / TEAM_FLG_BLUE*/
	static UnitInforNew getTowerDataByTeamFlg(int teamFlg);

};
#endif