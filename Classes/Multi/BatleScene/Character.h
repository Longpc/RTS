#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "cocos2d.h"
#include "base/Define.h"


USING_NS_CC;
using namespace std;


typedef std::function<void()> AttackCallback;
class Character : public Sprite
{
public :
	Character();
	~Character();
	/*Character id: 2: Red, 3: Purple, 4: Green, 5: Blue, 6: Black*/
	static Character* createCharacter(int characterId);
	virtual bool init(int characterId);
	// get/set function
	CC_SYNTHESIZE(string, _moveImagePath, MoveImagePath);
	CC_SYNTHESIZE(string, _attackImagePath, AttackImagePath);
	CC_SYNTHESIZE(int, _moveMode, MoveMode);
	CC_SYNTHESIZE(float, _characterMoveSpeed, CharacterMoveSpeed);
	CC_SYNTHESIZE(Vec2, _miniCircle, MiniCirclePosition);
	CC_SYNTHESIZE(int, _currentMoveActionTag, CurrentMoveActionTag);
	CC_SYNTHESIZE(int, _currentAttackActionTag, CurrentAttackActionTag);
	CC_SYNTHESIZE(float, _moveOneTapTime, MoveOneTapTime);
	CC_SYNTHESIZE(Vec2, _destinationalOneTap, DestinationalOneTap);
	CC_SYNTHESIZE(AttackCallback, _attackCallback , AttackCallbackFunction);
	CC_SYNTHESIZE(bool, _attackDelayFlg, AttackDelayFlag);
	CC_SYNTHESIZE(bool, _onMoving, OnMovingFlg);
	/*for save neutral unit data in solo mode*/
	CC_SYNTHESIZE(int, _saveAttackTime, SaveAtttackTime);
	void moveActionByVector(Vec2 destination);
	void attackActionByUnitPosition(int direction, int attackTime, AttackCallback oneSecondCallback = nullptr,  AttackCallback attackCallback = nullptr);
	void respawAction();
	void stopMoveAction();
	void stopAttackAction();

	virtual void actionMoveCharacter(int directionId);
	virtual void rotateCharacter(int direc);
	virtual void changeAnimationImagePathByUnitId(int unitId);
	CC_SYNTHESIZE(bool, _birdMode, BirdMode);
	int _birdModeIndex = 0;
	virtual void birdMode(int index);

	/*
	 * Character id: 2: Red, 3: Purple, 4: Green, 5: Blue, 6: Black
	 */
	virtual void changeUnitType(int unitId);

private :
	Sprite *testObject;
	int _characterId;
	float _direc;
	float _characterSpeed;


	
	///CHARACTER MOVE LOGIC///
	virtual bool caculAvgAngle(int avg, float angle);
	virtual int detectDirectionBaseOnTouchAngle(float angle);

	///CHARACTER MOVE ACTION///
	
	virtual Animation* createMoveAnimationWithDefine(int imageId);
	virtual Animation* createAttackAnimationWithDefine(int imageId);
	
	

};




#endif /* _CHARACTERMOVERMANAGER_H_ */
