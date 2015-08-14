#ifndef SUMMON_PET
#define SUMMON_PET
#include "base/Define.h"
#include "Map.h"
#include "Server/API/BattleAPI.h"
using namespace std;

class SummonPet : public Sprite
{
public:
	~SummonPet();
	static SummonPet* createPet(int chatacterId, MyMap* map,MyMap* miniMap,int gameMode, bool moveEnable = false);
	virtual bool init(int characerId);
	CC_SYNTHESIZE(int, _characterId, CharacterId);
	CC_SYNTHESIZE(bool, _moveEnableFlg, MoveFlg);
	CC_SYNTHESIZE(int, _teamFlg, TeamFlag);
	CC_SYNTHESIZE(string, _petName, PetName);
	CC_SYNTHESIZE(string, _parentUuid, ParentUuid);
	CC_SYNTHESIZE(int, _index, IndexOffset);
	CC_SYNTHESIZE(int, _petHP, petHP);
	CC_SYNTHESIZE(MyMap*, _map, BattleMap);
	CC_SYNTHESIZE(MyMap*, _miniMap, BattleMiniMap);
	CC_SYNTHESIZE(int, _gameMode, GameMode);
	void onEnterTransitionDidFinish();
	float _checkTIme = 0;
	void update(float delta);
private:
	void rotatePetWithMoveVector(Vec2 vec);
	bool caculAvgAngle(int avg, float angle);
};

#endif