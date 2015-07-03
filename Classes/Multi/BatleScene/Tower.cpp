#include "Tower.h"


Tower* Tower::createTower(int type)
{
	auto tw = new Tower();
	tw->init(type);
	tw->autorelease();
	return tw;
}

bool Tower::init(int type)
{
	if (!Sprite::init()) {
		return false;
	}
	_neutralTexture = Director::getInstance()->getTextureCache()->addImage(NEUTRAL_IMG_PATH);
	_blueTexture = Director::getInstance()->getTextureCache()->addImage(BLUE_IMG_PATH);
	_redTexture = Director::getInstance()->getTextureCache()->addImage(RED_IMG_PATH);
	changeTowerType(type, true);
	return true;
}

void Tower::runAttackAnimationWithTarget(Sprite* target, Callback callback)
{
	if (getTowerType() == 0) return;

	auto atAnimate = Animate::create(createTowerAttackAnimation());
	createEffectObject(target);
	this->runAction(Sequence::create(atAnimate, CallFuncN::create(callback), nullptr));
}

Animation* Tower::createTowerAttackAnimation()
{
	auto animation = Animation::create();
	for (int i = 0; i < 2; i++)
	{
		char fName[30] = { 0 };
		sprintf(fName, "%d.png", i);
		string path = _attackImgPath;
		path.append(fName);
		animation->addSpriteFrameWithFile(path.c_str());
	}

	animation->setDelayPerUnit(ANIMETE_DELAY);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);

	return animation;
}

void Tower::createEffectObject(Sprite* target)
{
	_target = target;
	ef = ParticleSystemQuad::create(_plistPath);
	this->getParent()->addChild(ef,100);
	ef->setPosition(this->getPosition());
	schedule(SEL_SCHEDULE(&Tower::updatePosition), 0.15f);
}

void Tower::updatePosition(float dt)
{
	//log("updater %f", dt);
	Vec2 pos1 = _target->getPosition();
	Vec2 pos2 = ef->getPosition();
	if ((pos1 - pos2).length() < 50) {
		ef->runAction(RemoveSelf::create(true));
		unschedule(SEL_SCHEDULE(&Tower::updatePosition));
	}

	auto move = MoveTo::create(dt, pos1);
	ef->runAction(move);
}

void Tower::changeTowerType(int type, bool isInit)
{
	switch (type)
	{
	case 0:
		_currentTexture = _neutralTexture;
		break;
	case 1:
		_currentTexture = _blueTexture;
		_attackImgPath = BLUE_ATTACK_IMG_PATH;
		_plistPath = BLUE_ATTACK_ANIMATION_PLIST_PATH;
		break;
	case 2:
		_currentTexture = _redTexture;
		_attackImgPath = RED_ATTACK_IMG_PATH;
		_plistPath = RED_ATTACK_ANIMATION_PLIST_PATH;
		break;

	}
	setTowerType(type);
	//log("Tower type: %d", type);
	if (isInit)
	{
		Rect rect = Rect::ZERO;
		rect.size = _currentTexture->getContentSize();
		initWithTexture(_currentTexture, rect);
	}
	else {
		setTexture(_currentTexture);
	}
	
}
