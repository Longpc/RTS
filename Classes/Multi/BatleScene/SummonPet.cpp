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
		sprintf(szName, "image/new_unit/unit_0%d_8.png", chatacterId);
		pet->initWithFile(szName);
		pet->setMoveFlg(moveEnable);
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

	}), DelayTime::create(1.0f), nullptr)));

}
void SummonPet::update(float dt)
{
	auto pos = this->getPosition();
	if (getBattleMap()->checkPosInsizeMap(pos) == false) return;
	
	auto titleC = getBattleMap()->getTitleCoorForPosition(pos);
	//need check nearly object title

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
				return;
			}
			else {
				title->setColor(Color3B::GREEN);
				auto mTittle = getBattleMiniMap()->getTitleAt(titleC);
				mTittle->setColor(Color3B::GREEN);
				return;
			}
		}
	}
	else {
		if (title->getColor() != Color3B::ORANGE)
		{
			if (_gameMode == MULTI_MODE) {
				if (title->getName() == "sending") return;
				log("send test move event");
				title->setName("sending");
				//sendingFlg->push_back(true);
				BattleAPI::getInstance()->sendTestMoveLogic(titleC);
				return;
			}
			else {
				title->setColor(Color3B::ORANGE);
				auto mTittle = getBattleMiniMap()->getTitleAt(titleC);
				mTittle->setColor(Color3B::ORANGE);
				return;
			}
		}
	}

}

SummonPet::~SummonPet()
{
	this->stopAllActions();
}
