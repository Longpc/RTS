#include "Multi/BatleScene/SummonPet.h"



SummonPet* SummonPet::createPet(int chatacterId, MyMap* map, MyMap* miniMap,int gameMode, bool moveEnable)
{
	auto pet = new SummonPet();
	if (pet)
	{
		
		char szName[100] = { 0 };
		pet->setGameMode(gameMode);
		pet->init(chatacterId);
		pet->setBattleMap(map);
		pet->setBattleMiniMap(miniMap);
		pet->setMoveFlg(moveEnable);
		sprintf(szName, "image/new_unit/unit_0%d_8.png", chatacterId);
		pet->initWithFile(szName);	
		pet->autorelease();
		
		return pet;
	}
	CC_SAFE_DELETE(pet);
	return nullptr;
}

bool SummonPet::init(int characerId)
{
	if (!Sprite::init()) {
		return false;
	}
	setCharacterId(characerId);
	scheduleUpdate();
	return true;
}

void SummonPet::onEnterTransitionDidFinish()
{
	Sprite::onEnterTransitionDidFinish();
	if (!getMoveFlg()) return;
	this->runAction(RepeatForever::create(Sequence::create(CallFuncN::create([&](Ref* p){
		auto sp = (Sprite*)p;
		if (sp->getPhysicsBody() == nullptr) return;
		float x = random(-1.0f, 1.0f) * 1000;
		float y = random(-1.0f, 1.0f) * 1000;
		Vec2 vec = Vec2(x, y);
		sp->getPhysicsBody()->setVelocity(Vec2::ZERO);
		Vec2 force = Vec2(150 * cos(vec.getAngle()), 150 * sin(vec.getAngle()));
		sp->getPhysicsBody()->setVelocity(force);
		rotatePetWithMoveVector(vec);
		if (!getMoveFlg()) return;

		if (getGameMode() == MULTI_MODE)
		{
			BattleAPI::getInstance()->sendMiniOnMoveEvent(getTeamFlag(), getIndexOffset(), getParentUuid().c_str(), getPosition(), force);
		}

	}), DelayTime::create(1.0f), nullptr)));

}
void SummonPet::update(float dt)
{
	if (!getMoveFlg()) return;

	if (getTeamFlag() < 1 || getTeamFlag() > 2) return;
	auto pos = this->getPosition();
	if (getBattleMap()->checkPosInsideMap(pos) == false) return;
	
	auto titleC = getBattleMap()->getTitleCoorForPosition(pos);
	//need check nearly object title

	if (getBattleMap()->checkTileCantGet(titleC)) return;

	auto title = getBattleMap()->getTitleAt(titleC);
	if (title == nullptr) {
		return;
	}
	if (title->getName() == "disable") return;
	if (getTeamFlag() == TEAM_FLG_BLUE) {
		if (title->getColor() != Color3B::GREEN)
		{
			if (_gameMode == MULTI_MODE)
			{
				if (title->getName() == "sending") return;
				//log("send test move event");
				//sendingFlg->push_back(true);
				title->setName("sending");
				BattleAPI::getInstance()->sendTestMoveLogic(titleC);
				//return;
			}
			//else {
				title->setColor(Color3B::GREEN);
				auto mTittle = getBattleMiniMap()->getTitleAt(titleC);
				mTittle->setColor(Color3B::GREEN);
				return;
			//}
		}
	}
	else {
		if (title->getColor() != Color3B::ORANGE)
		{
			if (_gameMode == MULTI_MODE) {
				if (title->getName() == "sending") return;
				//log("send test move event");
				title->setName("sending");
				//sendingFlg->push_back(true);
				BattleAPI::getInstance()->sendTestMoveLogic(titleC);
				//return;
			}
			//else {
				title->setColor(Color3B::ORANGE);
				auto mTittle = getBattleMiniMap()->getTitleAt(titleC);
				mTittle->setColor(Color3B::ORANGE);
				return;
			//}
		}
	}
}

SummonPet::~SummonPet()
{
	this->stopAllActions();
}

void SummonPet::rotatePetWithMoveVector(Vec2 vec)
{
	auto angle = -vec.getAngle() *RAD_DEG + 90;
	int direc = 1;
	if (caculAvgAngle(0, angle)) direc = 1;
	if (caculAvgAngle(90, angle)) direc =  4;
	if (caculAvgAngle(180, angle)) direc = 7;
	if (caculAvgAngle(270, angle) || caculAvgAngle(-90, angle)) direc = 10;
	char szName[100] = { 0 };
	sprintf(szName, "image/new_unit/unit_0%d_%d.png", getCharacterId(), direc);
	auto text = Director::getInstance()->getTextureCache()->addImage(szName);
	if (text) this->setTexture(text);
}
bool SummonPet::caculAvgAngle(int avg, float angle) {
	if (angle > avg - 44.9f && angle < avg + 44.9f)
		return true;
	return false;
}