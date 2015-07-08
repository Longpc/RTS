#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserUnitModel.h"

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

	//CC_SYNTHESIZE(string, _moveImagePath, MoveImagePath);
	//CC_SYNTHESIZE(string, _attackImagePath, AttackImagePath);
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
	CC_SYNTHESIZE(int, _currentDirec, CharacterCurrentDirec);

	/*for minion skill logic*/
	CC_SYNTHESIZE(int, _minionTotalHp, MinionTotalHp);

	void moveActionByVector(Vec2 destination);
	void attackActionByTargetPosition(Vec2 direcVector, int attackTime, AttackCallback oneSecondCallback = nullptr,  AttackCallback attackCallback = nullptr);
	void respawAction();
	void stopMoveAction();
	void stopAttackAction();

	virtual void actionMoveCharacter(int directionId);
	virtual void rotateCharacter(int direc);
	//virtual void changeAnimationImagePathByUnitId(int unitId);

	/*get/set the flag to decide that this character in bird mode or not*/
	CC_SYNTHESIZE(bool, _birdMode, BirdMode);
	CC_SYNTHESIZE(int, _birdModeIndex, BirdModeIndex);
	virtual void birdMode(int index);

	virtual void runLunchingAction();


	CC_SYNTHESIZE(bool, _onCannonLunch, OnCannonLunchFlg);

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
	//virtual Animation* createAttackAnimationWithDefine(int imageId);
	virtual void attackActionWithDirec(int direc);
	

};




#endif /* _CHARACTERMOVERMANAGER_H_ */
