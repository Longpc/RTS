#ifndef UNIT_DATA_HELPER
#define UNIT_DATA_HELPER

#include "base/Define.h"
#include "base/LayerBase.h"

class UnitData
{
public:
	static UnitInforNew getUnitDataById(int unitId);
	static vector<UnitInforNew> getAllUnitData();
	static UnitInforNew convertInfo(vector<string> data);

};
#endif