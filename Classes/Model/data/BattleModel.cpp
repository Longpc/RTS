#include "BattleModel.h"

BattleModel* BattleModel::s_BattleModel = nullptr;
BattleModel* BattleModel::getInstance()
{
	if (!s_BattleModel) {
		s_BattleModel = new (std::nothrow)BattleModel();
		s_BattleModel->init();
	}
	return s_BattleModel;
}

bool BattleModel::init()
{
	setPlayerSkills({});
	return true;
}

//received data from server and parser for initial battle information (selected units, skills, player information....)
void BattleModel::parserJsonToInitData(string jsonData)
{
	vector<int> skillIds = {};
	rapidjson::Document doc;
	doc.Parse<0>(jsonData.c_str());
	if (doc.HasParseError()) {
		log("Battle Model: error in parser Json data");
		return;
	}
	if (doc.IsObject() && doc.HasMember("args")) {
		log("HERE");
		auto a = UserModel::getInstance()->getUuId();
		log("Args Size: %d", doc["args"].Size());
		log("Size 2: %d",doc["args"][rapidjson::SizeType(0)].Size());
		for (int i = 0; i  < doc["args"][rapidjson::SizeType(0)].Size(); i++)
		{
			log("%s",doc["args"][rapidjson::SizeType(0)][i]["room_user_id"].GetString());
		}
	}
	else {
		log("Battle Model: wrong data");
		return;
	}
}
