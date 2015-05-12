#ifndef BATTLE_MODEL_H__
#define BATTLE_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserSkill.h"
#include "Model/data/UserUnit.h"
#include "Model/data/UserModel.h"

#include "base/database/DataUtils.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"

USING_NS_CC;
using namespace rapidjson;
using namespace std;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define BUILDING_MYDLL 12
#ifdef BUILDING_MYDLL
#define MYCLASS_DECLSPEC __declspec(dllexport)
#else
#define MYCLASS_DECLSPEC __declspec(dllimport)
#endif
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
# define MYCLASS_DECLSPEC CC_DLL
#endif

class MYCLASS_DECLSPEC BattleModel :public Ref
{
public:
	static BattleModel* getInstance();
	bool init();
	void parserJsonToInitData(string jsonData);

	CC_SYNTHESIZE(vector<int>, _playerSkills, PlayerSkills);

private:
	static BattleModel * s_BattleModel;
};
#endif