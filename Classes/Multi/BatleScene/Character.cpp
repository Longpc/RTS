#include "Character.h"

USING_NS_CC;
using namespace std;

Character::Character()
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
		string path = character->getMoveImagePath(); // character->getMoveImagePath = _moveImagePath;
		path.append("unit_00_08_1.png");
		character->initWithFile(path);
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
	changeAnimationImagePathByUnitId(_characterId);

	this->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);

	return true;
}

void Character::changeAnimationImagePathByUnitId(int unitId) {

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

void Character::moveActionByVector(Vec2 destination)
{
	int direct = detectDirectionBaseOnTouchAngle(-(destination.getAngle() * RAD_DEG) + 90);
	auto distan = destination - this->getMiniCirclePosition();
	switch (this->getMoveMode())
	{
	case 1: // MOVE_AUTO
		break;
	case 2: // MOVE_MANUAL
	case 3: // MOVE_CIRCLE_MANUAL
	case 4: // MOVE_CIRCLE_LONGTAP
		this->getPhysicsBody()->setVelocity(Vect(this->getCharacterMoveSpeed() * cos(destination.getAngle()), this->getCharacterMoveSpeed() * sin(destination.getAngle())));
		actionMoveCharacter(direct);
		break;
	case 5: // MOVE_CIRCLE_ONETAP	
		
		this->stopActionByTag(101);
		auto actionOneTap = Sequence::create(DelayTime::create(this->getMoveOneTapTime()), CallFuncN::create([&](Ref* pSender){
			this->stopMoveAction();
		}), nullptr);
		actionOneTap->setTag(101);
		this->runAction(actionOneTap);
		if (this->getPhysicsBody() != nullptr)
		{
			this->getPhysicsBody()->setVelocity(Vect(this->getCharacterMoveSpeed() * cos(destination.getAngle()), this->getCharacterMoveSpeed() * sin(destination.getAngle())));
		}
		actionMoveCharacter(direct);

		break;
	}
}

void Character::attackActionByUnitPosition(int direction , int attackTime ,AttackCallback attackCallback)
{
	if (getAttackDelayFlag() == false) {
		setAttackDelayFlag(true);
		rotateCharacter(direction);
		auto ani = Animate::create(createAttackAnimationWithDefine(direction));
		auto call1 = CallFuncN::create([& , attackCallback](Ref* pSender){
			setAttackDelayFlag(false);
			
		});
		this->runAction(Spawn::create(Sequence::create(DelayTime::create(attackTime), call1, nullptr), Sequence::create(ani, CallFuncN::create([&, attackCallback](Ref* pSender){
			if (attackCallback != nullptr)
			{
				attackCallback();
			}

		}),nullptr), nullptr));
	}
}

void Character::respawAction()
{

}

void Character::stopMoveAction()
{
	this->stopAllActionsByTag(this->getCurrentMoveActionTag());
	if (this->getPhysicsBody() == nullptr)
	{
		return;
	}
	else
	{
		this->getPhysicsBody()->setVelocity(Vect(0,0));
	}
	
}

void Character::stopAttackAction()
{

}

Animation* Character::createMoveAnimationWithDefine(int imageId) {
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++) 
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_%d.png", imageId, i);
		string p = getMoveImagePath();
		p.append(szName);
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}
Animation* Character::createAttackAnimationWithDefine(int imageId) 
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
}

void Character::actionMoveCharacter(int directionId) {
	if (this->getNumberOfRunningActions() > 0) {
		if (this->getActionByTag(directionId) != nullptr) {
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
}

void Character::rotateCharacter(int direc) {
	char szName[100] = { 0 };
	sprintf(szName, "unit_00_0%d_%d.png", direc, 1);
	string p = getMoveImagePath();
	p.append(szName);
	Texture2D *text = Director::getInstance()->getTextureCache()->addImage(p.c_str());
	this->setTexture(text);

}
int Character::detectDirectionBaseOnTouchAngle(float angle) {
	if (caculAvgAngle(0, angle))
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
	return 8;
}

bool Character::caculAvgAngle(int avg, float angle) {
	if (angle > avg - 22 && angle < avg + 22)
		return true;
	return false;
}
