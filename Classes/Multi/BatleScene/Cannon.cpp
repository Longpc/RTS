#include "Cannon.h"

Cannon::Cannon()
{

}

Cannon::~Cannon()
{

}

Cannon* Cannon::createCannon()
{
	auto cannon = new Cannon();
	if (cannon) {
		cannon->init();
		string path = cannon->getImagePath();
		cannon->initWithFile(path);
		cannon->autorelease();

		return cannon;
	}
	CC_SAFE_DELETE(cannon);
	return nullptr;
}

bool Cannon::init()
{
	if (!Sprite::init()) {
		return false;
	}
	changeTeamStatus(0,true);
	setAttackedTime(0);
	setDisableFlg(false);
	return true;
}

void Cannon::onEnter()
{
	Sprite::onEnter();
}

void Cannon::changeTeamStatus(int teamId, bool initFlg)
{
	switch (teamId)
	{
	case TEAM_FLG_BLUE:
		setImagePath("image/screen/battle/cannon_blue.png");
		setCurrentTeamId(TEAM_FLG_BLUE);
		break;
	case TEAM_FLG_RED:
		setImagePath("image/screen/battle/cannon_red.png");
		setCurrentTeamId(TEAM_FLG_RED);
		break;
	default:
		setImagePath("image/screen/battle/cannon.png");
		setCurrentTeamId(0);
		break;
	}
	if (!initFlg) {
		setTexture(Director::getInstance()->getTextureCache()->addImage(getImagePath()));
	}
}

void Cannon::onTouchEnded(Touch *touch, Event *unused_event)
{

}

void Cannon::disableCannon()
{
	this->setDisableFlg(true);
	auto disableAnimation = RepeatForever::create(Blink::create(1.0f, 2));
	disableAnimation->setTag(123456);
	this->runAction(disableAnimation);
	this->runAction(Sequence::create(DelayTime::create(10.0f), CallFuncN::create([&](Ref *p){
		auto cn = (Cannon*)p;
		cn->setVisible(true);
		cn->setDisableFlg(false);
		cn->stopAllActionsByTag(123456);
	}), nullptr));
}
