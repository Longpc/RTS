#include "Character.h"

USING_NS_CC;
using namespace std;

Character::Character()
	: _unitUUid(""),
	_characterId(0),
	_moveMode(0),
	_characterMoveSpeed(0),
	_currentAttackActionTag(0),
	_currentMoveActionTag(0)
{
	//Contructor
}
Character::~Character()
{
	//Detructor
}

Character* Character::createCharacter(int characterId)
{
	auto character = new Character();
	if (character)
	{
		character->init(characterId);
		char szName[100] = { 0 };
		sprintf(szName, "image/new_unit/unit_0%d_8.png", characterId);
		character->initWithFile(szName);
		character->autorelease();
		return character;
	}
	CC_SAFE_DELETE(character);
	return nullptr;
}


bool Character::init(int characterId)
{
	if (!Sprite::init())
	{
		return false;
	}

	_characterId = characterId;
	setAttackDelayFlag(false);
	setOnMovingFlg(false);
	setOnCannonLunchFlg(false);
	//changeAnimationImagePathByUnitId(_characterId);

	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);

	return true;
}

/*void Character::changeAnimationImagePathByUnitId(int unitId) {

	switch (unitId - 1) {
	case 1:
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	case 3:
		_moveImagePath = "image/unit_new/move/green/";
		_attackImagePath = "image/unit_new/attack/green/";
		break;
	case 4:
		_moveImagePath = "image/unit_new/move/blue/";
		_attackImagePath = "image/unit_new/attack/blue/";
		break;
	case 5:
		_moveImagePath = "image/unit_new/move/black/";
		_attackImagePath = "image/unit_new/attack/black/";
		break;
	case 2:
		_moveImagePath = "image/unit_new/move/purple/";
		_attackImagePath = "image/unit_new/attack/purple/";
		break;
	default:
		_moveImagePath = "image/unit_new/move/red/";
		_attackImagePath = "image/unit_new/attack/red/";
		break;
	}
}
*/
void Character::changeUnitType(int unitId)
{
	//changeAnimationImagePathByUnitId(unitId);
	_characterId = unitId;
	string path = UserUnitModel::getInstance()->getUnitImageByMstUnitItD(unitId);
	auto text = Director::getInstance()->getTextureCache()->addImage(path);
	this->setTexture(text);

}

void Character::moveActionByVector(Vec2 destination)
{
	int direct = detectDirectionBaseOnTouchAngle(-(destination.getAngle() * RAD_DEG) + 90);
	auto distan = destination - this->getMiniCirclePosition();
	switch (this->getMoveMode())
	{
	case 1: // MOVE_AUTO
		actionMoveCharacter(direct);
		if (this->getPhysicsBody() != nullptr)
		{
			this->getPhysicsBody()->setVelocity(Vect(this->getCharacterMoveSpeed()*SPEED_MULTIPLE * cos(destination.getAngle()), this->getCharacterMoveSpeed()*SPEED_MULTIPLE * sin(destination.getAngle())));
		}
		break;
	case 2: // MOVE_MANUAL
	case 3: // MOVE_CIRCLE_MANUAL
	case 4: // MOVE_CIRCLE_LONGTAP
		this->getPhysicsBody()->setVelocity(Vect(this->getCharacterMoveSpeed() * cos(destination.getAngle()), this->getCharacterMoveSpeed() * sin(destination.getAngle())));
		actionMoveCharacter(direct);
		break;
	case 5: // MOVE_CIRCLE_ONETAP	
		
		this->stopActionByTag(101);
		/*auto actionOneTap = Sequence::create(DelayTime::create(this->getMoveOneTapTime()), CallFuncN::create([&](Ref* pSender){
			this->stopMoveAction();
			this->getPhysicsBody()->setVelocity(Vec2::ZERO);
		}), nullptr);
		actionOneTap->setTag(101);
		this->runAction(actionOneTap);*/
		if (this->getPhysicsBody() != nullptr)
		{
			this->getPhysicsBody()->setVelocity(Vect(this->getCharacterMoveSpeed()*SPEED_MULTIPLE * cos(destination.getAngle()), this->getCharacterMoveSpeed()*SPEED_MULTIPLE * sin(destination.getAngle())));
		}
		if (getBirdMode() == false)
		{
			actionMoveCharacter(direct);
		}
		else {
			//getPhysicsBody()->setRotationEnable(true);
			setRotation(destination.getAngle());
			//getPhysicsBody()->setRotationEnable(false);
		}

		break;
	}
	/*if (BattleModel::getInstance()->getGameMode() == MULTI_MODE && this->getPhysicsBody() != nullptr)
	{
		BattleAPI::getInstance()->sendMoveEvent(this->getPosition(), this->getPhysicsBody()->getVelocity());
	}*/

}

void Character::attackActionByTargetPosition(Vec2 direcVector , int attackTime, AttackCallback oneSecondCb, AttackCallback attackCallback)
{
	if (getAttackDelayFlag() == false) {
		setAttackDelayFlag(true);		
		auto call1 = CallFuncN::create([& , oneSecondCb](Ref* pSender){
			setAttackDelayFlag(false);
			if (oneSecondCb != nullptr)
			{
				oneSecondCb();
			}
			
		});

		auto call2 = CallFuncN::create([&, attackCallback](Ref* pSender){
			if (attackCallback != nullptr)
			{
				attackCallback();
			}

		});

		Sequence *cb2Sequence;
		if (getBirdMode()) {
			cb2Sequence = Sequence::create(Blink::create(0.5f, 4), call2, nullptr);
		}
		else {
			/*auto ani = Animate::create(createAttackAnimationWithDefine(direction));
			cb2Sequence = Sequence::create(ani, call2, nullptr);*/
			cb2Sequence = Sequence::create(Blink::create(0.5f, 4), call2, nullptr);
			/*auto move = MoveBy::create(0.15f, Vec2(direcVector * 100 / direcVector.length()));
			auto rvMove = move->reverse();
			cb2Sequence = Sequence::create(move, rvMove,call2, nullptr);*/
		}

		this->runAction(Spawn::create(Sequence::create(DelayTime::create(attackTime), call1, nullptr), cb2Sequence, nullptr));
	}
	else {
		log("Unit on attack delay");
	}
}

void Character::respawAction()
{

}

void Character::stopMoveAction()
{
	this->stopAllActionsByTag(this->getCurrentMoveActionTag());
	setOnMovingFlg(false);
	/*if (this->getPhysicsBody() == nullptr)
	{
		return;
	}
	else
	{
		this->getPhysicsBody()->setVelocity(Vect(0,0));
	}*/
	
}

void Character::stopAttackAction()
{

}

Animation* Character::createMoveAnimationWithDefine(int direc) {
	auto animation = Animation::create();
	for (int i = 0; i < 3; i++) 
	{
		char szName[100] = { 0 };
		sprintf(szName, "image/new_unit/unit_0%d_%d.png", _characterId, direc+i);
		animation->addSpriteFrameWithFile(szName);
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}
/*Animation* Character::createAttackAnimationWithDefine(int direc) 
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++) {
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_attack_%d.png", imageId, i);
		string p = getAttackImagePath();
		p.append(szName);
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}*/

void Character::attackActionWithDirec(int direc)
{

}

void Character::actionMoveCharacter(int directionId) {
	if (getBirdMode()) {
		birdMode(_birdModeIndex);
		return;
	}
	if (getOnMovingFlg()) {
		if (_currentMoveActionTag == directionId) {
			//log("same with previous action");
			return;
		}
	}
	this->stopAllActionsByTag(this->getCurrentMoveActionTag());

	auto action = Animate::create(createMoveAnimationWithDefine(directionId));
	auto repeat = RepeatForever::create(action);
	repeat->setTag(directionId);
	//_currentMoveActionTag = directionId;
	this->setCurrentMoveActionTag(directionId);
	this->runAction(repeat);
	setOnMovingFlg(true);
	setCharacterCurrentDirec(directionId);
}

void Character::rotateCharacter(int direc) {
	if (getBirdMode()) return;
	int imgId = direc + 1;
	char szName[100] = { 0 };
	sprintf(szName, "image/new_unit/unit_0%d_%d.png", _characterId, imgId);
	Texture2D *text = Director::getInstance()->getTextureCache()->addImage(szName);
	this->setTexture(text);
	setCharacterCurrentDirec(direc);

}
int Character::detectDirectionBaseOnTouchAngle(float angle) {
	/*if (caculAvgAngle(0, angle))
		return 8;
	if (caculAvgAngle(45, angle))
		return 9;
	if (caculAvgAngle(90, angle))
		return 6;
	if (caculAvgAngle(135, angle))
		return 3;
	if (caculAvgAngle(180, angle))
		return 2;
	if (caculAvgAngle(225, angle))
		return 1;
	if (caculAvgAngle(-90, angle) || caculAvgAngle(270, angle))
		return 4;
	if (caculAvgAngle(-45, angle)) {
		return 7;
	}
	return 8;*/
	if (caculAvgAngle(0, angle)) return 1;
	if (caculAvgAngle(90, angle)) return 4;
	if (caculAvgAngle(180, angle)) return 7;
	if (caculAvgAngle(270, angle) || caculAvgAngle(-90, angle)) return 10;

	//default
	return 7;
}

bool Character::caculAvgAngle(int avg, float angle) {
	if (angle > avg - 44.9f && angle < avg + 44.9f)
		return true;
	return false;
}

void Character::birdMode(int index)
{
	stopMoveAction();
	setBirdModeIndex(index);
	char szName[100] = { 0 };
	sprintf(szName, "bird_0%d.png", index);
	string p = "image/bird/";
	p.append(szName);

	Texture2D* text = TextureCache::getInstance()->addImage(p);

	this->setTexture(text);
	this->setBirdMode(true);
	
	/*Rect = text->getContentSize();
	testObject->setTextureRect()*/
}

void Character::runLunchingAction()
{
	this->setColor(Color3B::WHITE);
	/*enable cannon lunch flag. It will be remove after 5 seconds*/
	this->setOnCannonLunchFlg(true);
	/*action to disable cannon lunch flag*/
	auto action = Sequence::create(DelayTime::create(5.0f), CallFuncN::create([&](Ref* p) {
		Character *cha = (Character*)p;
		cha->stopAllActionsByTag(1991);
		cha->setOnCannonLunchFlg(false);
	}), nullptr);
	action->setTag(1991);
	this->runAction(action);
}
