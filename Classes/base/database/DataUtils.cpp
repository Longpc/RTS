#include "DataUtils.h"


std::string DataUtils::numberToString(float number)
{
	std::stringstream text;
	text << number;
	return text.str().c_str();
}

float DataUtils::stringToFloat(const string str)
{
	stringstream ss(str);
	float result;
	return ss >> result ? result : 0;
}

char* DataUtils::MultiByteToUTF8(const char* src)
{
	/*unsigned int sizeWide = MultiByteToWideChar(0, 0, src, -1, nullptr, 0);
	wchar_t* bufferWide = new wchar_t[sizeWide + 1]();
	MultiByteToWideChar(0, 0x00000001, src, -1, bufferWide, sizeWide);
	unsigned int sizeUtf8 = WideCharToMultiByte(65001, 0, bufferWide, -1, nullptr, 0, nullptr, nullptr);
	char* bufferUtf8 = new char[sizeUtf8 + 1]();
	WideCharToMultiByte(65001, 0, bufferWide, -1, bufferUtf8, sizeUtf8, nullptr, nullptr);
	delete bufferWide;
	return bufferUtf8;*/
	return NULL;
}

rapidjson::Document::GenericValue& DataUtils::convertUnitDataToJsonObject(UnitInforNew unitData, rapidjson::Document::AllocatorType& allo)
{
	rapidjson::Value unitDataValue/*(kObjectType)*/;
	unitDataValue.SetObject();
	unitDataValue.AddMember("mst_unit_id", unitData.id, allo);
	unitDataValue.AddMember("name", unitData.name.c_str(), allo);
	unitDataValue.AddMember("hp", unitData.hp, allo);
	unitDataValue.AddMember("hp_heal", unitData.hp_restore, allo);
	unitDataValue.AddMember("mp", unitData.mp, allo);
	unitDataValue.AddMember("mp_heal", unitData.mp_restore, allo);
	unitDataValue.AddMember("attack", unitData.attack_dame, allo);
	unitDataValue.AddMember("attack_range", unitData.attack_range, allo);
	unitDataValue.AddMember("attack_speed", unitData.attack_delay, allo);
	unitDataValue.AddMember("defence", unitData.defence, allo);
	unitDataValue.AddMember("move_speed", unitData.move_speed, allo);
	unitDataValue.AddMember("element", unitData.type, allo);
	unitDataValue.AddMember("mst_skill_id_1", unitData.id, allo);
	unitDataValue.AddMember("mst_skill_id_2", unitData.id, allo);
	
	return unitDataValue;
}
