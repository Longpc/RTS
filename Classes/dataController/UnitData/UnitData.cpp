#include "dataController/UnitData/UnitData.h"

#define DATABASE_FILE "database.db3"

UnitInforNew UnitData::getUnitDataById(int unitId)
{
	sqlite3 *data = SqlUtil::openData(DATABASE_FILE);
	string sql = "select * from unit where unit.id =";
	sql.append(DataUtils::numberToString(unitId));
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	if (a.size() > 0)
	{
		return convertInfo(a[0]);
	}
	UnitInforNew b;
	return b;
}

vector<UnitInforNew> UnitData::getAllUnitData()
{
	vector<UnitInforNew> allUnit;
	sqlite3 *data = SqlUtil::openData(DATABASE_FILE);
	string sql = "select * from unit where unit.move_speed > 0 ";
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	for (auto &item : a)
	{	
		allUnit.push_back(convertInfo(item));
		
	}
	return allUnit;
}

UnitInforNew UnitData::convertInfo(vector<string> item)
{
	UnitInforNew temp;
	temp.id = DataUtils::stringToFloat(item[0].c_str());
	temp.name = item[1];
	temp.hp = DataUtils::stringToFloat(item[2].c_str());
	temp.hp_restore = DataUtils::stringToFloat(item[3].c_str());
	temp.mp = DataUtils::stringToFloat(item[4].c_str());
	temp.mp_restore = DataUtils::stringToFloat(item[5].c_str());
	temp.attack_dame = DataUtils::stringToFloat(item[6].c_str());
	temp.defence = DataUtils::stringToFloat(item[7].c_str());
	temp.attack_sight = DataUtils::stringToFloat(item[8].c_str());
	temp.move_speed = DataUtils::stringToFloat(item[9].c_str());
	temp.attr = DataUtils::stringToFloat(item[10].c_str());
	temp.type = DataUtils::stringToFloat(item[11].c_str());
	temp.image = item[12].c_str();
	temp.attack_delay = DataUtils::stringToFloat(item[13].c_str());

	return temp;
}

UnitInforNew UnitData::getTowerDataByTeamFlg(int teamFlg)
{
	sqlite3 *data = SqlUtil::openData(DATABASE_FILE);
	string sql = "select * from unit where unit.move_speed = 0 AND unit.mp = ";
	sql.append(DataUtils::numberToString(teamFlg));
	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	if (a.size() > 0)
	{
		return convertInfo(a[0]);
	}
	UnitInforNew b;
	return b;
}
