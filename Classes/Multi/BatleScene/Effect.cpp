//////////////////////////////////////////////////////////////////////////////////////
// CLASS EFFECT EXTENDS SPRITE
//////////////////////////////////////////////////////////////////////////////////////
#include "Effect.h"

USING_NS_CC;

/////////////////////////////////////////////////////////////////////////////////////////
// CONTRUCTER AND DETRUCTOR
/////////////////////////////////////////////////////////////////////////////////////////
Effect::Effect() : _spriteAttack(nullptr)
{

}

Effect::~Effect()
{
	_spriteAttack = nullptr;
	unschedule(schedule_selector(Effect::updateAttackMove));
}



/////////////////////////////////////////////////////////////////////////////////////////
// CREATE EFFECT WITH VOID AND PARTICLE
/////////////////////////////////////////////////////////////////////////////////////////
Effect* Effect::create()
{
	auto effect = new Effect();
	if (effect && effect->init())
	{
		effect->autorelease();
		return effect;
	}
	CC_SAFE_DELETE(effect);
	return nullptr;
}

bool Effect::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}


/*
	Create Effect using Particle File
	@param plistEffectPath: ParticleFile Path
	@param effectColor: int effectColor(blue=1 , red=2 , green=3 , yellow=4 , other = 0)
	@param skillType: int skillType(restore=1 , help=2 , attack=3)
*/
Effect* Effect::createWithParticle(std::string plistEffectPath, int effectColor , int skillType)
{
	auto effect = new Effect();
	if (effect && effect->initWithParticle(plistEffectPath, effectColor , skillType))
	{
		effect->autorelease();
		return effect;
	}
	CC_SAFE_DELETE(effect);

	return nullptr;
}


/*
	Init Effect using Particle File
	@param plistEffectPath: ParticleFile Path
	@param effectColor: int effectColor(blue=1 , red=2 , green=3 , yellow=4 , other = 0)
	@param skillType: int skillType(restore=1 , help=2 , attack=3)
*/
bool Effect::initWithParticle(std::string plistEffectPath, int effectColor , int skillType)
{
	if (!Sprite::init())
	{
		return false;
	}

	ParticleSystemQuad* skillEffect;

	switch (skillType)
	{
	case SkillType::Restore:
		createEffectHeal(plistEffectPath, effectColor);
		break;
	case SkillType::Help:
		createEffectHelp(plistEffectPath, effectColor);
		break;
	case SkillType::Attack:
		createEffectAttackFire(plistEffectPath, effectColor);
		break;
	case SkillType::Help_Defence:
		createEffectHelpDefence(plistEffectPath, effectColor);
		break;
	default:
		break;
	}

	return true;
}


void Effect::onEnter()
{
	Sprite::onEnter();

	if (_spriteAttack != nullptr)
	{
		schedule(schedule_selector(Effect::updateAttackMove), MOVE_TIME_INTERVAL);
	}
	else
	{
		return;
	}
}

void Effect::updateAttackMove(float dt)
{
	//_indexOfBeAttackEnemy
	Vec2 pos1 = _spriteAttack->getPosition();
	Vec2 pos2 = this->getPosition();

	auto move = MoveTo::create(dt , pos1);
	this->runAction(move);
}

void Effect::setTargetAttack(Sprite* attackSprite)
{
	_spriteAttack = attackSprite;
}


/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT HEAL
/////////////////////////////////////////////////////////////////////////////////////////

void Effect::createEffectHeal(std::string plistEffectPath, int effectColor)
{
	ParticleSystemQuad* skillHealEffect = ParticleSystemQuad::create(plistEffectPath);
	skillHealEffect->setPosition(Vec2::ZERO);
	setColorEffect(skillHealEffect, effectColor);
	this->addChild(skillHealEffect);
}


/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT HELP RESTORE
/////////////////////////////////////////////////////////////////////////////////////////

void Effect::createEffectHelp(std::string plistEffectPath, int effectColor)
{
	ParticleSystemQuad* skillHelpEffect = ParticleSystemQuad::create(plistEffectPath);
	skillHelpEffect->setDuration(DELAY_HELP);
	skillHelpEffect->setPosition(Vec2::ZERO);
	setColorEffect(skillHelpEffect, effectColor);

	this->addChild(skillHelpEffect);
}


void Effect::createEffectHelpDefence(std::string plistEffectPath, int effectColor)
{
	// Unit size (70x120)

	// 4‚Â–Ê‚Ì–hŒä
	////////////////////TOP
	Vec2 posTop = Vec2(
		this->getPositionX(),
		this->getPositionY() + 20
		);

	ParticleSystemQuad* defenceTop = ParticleSystemQuad::create(plistEffectPath);
	defenceTop->setDuration(DELAY_HELP);
	setColorEffect(defenceTop, effectColor);
	this->addChild(defenceTop);
	defenceTop->setPosition(posTop);


	////////////////////RIGHT
	Vec2 posRight = Vec2(
		this->getPositionX() + 70 ,
		this->getPositionY()
		);

	ParticleSystemQuad* defenceRight = ParticleSystemQuad::create(plistEffectPath);
	defenceRight->setScaleX(0.2f);
	defenceRight->setDuration(DELAY_HELP);
	setColorEffect(defenceRight, effectColor);
	this->addChild(defenceRight);
	defenceRight->setPosition(posRight);


	////////////////////BOTTOM
	Vec2 posBottom = Vec2(
		this->getPositionX(),
		this->getPositionY() - 20
		);

	ParticleSystemQuad* defenceBottom = ParticleSystemQuad::create(plistEffectPath);
	defenceBottom->setDuration(DELAY_HELP);
	setColorEffect(defenceBottom, effectColor);
	this->addChild(defenceBottom);
	defenceBottom->setPosition(posBottom);


	////////////////////LEFT
	Vec2 posLeft = Vec2(
		this->getPositionX() - 70,
		this->getPositionY()
		);

	ParticleSystemQuad* defenceLeft = ParticleSystemQuad::create(plistEffectPath);
	defenceLeft->setScaleX(0.2f);
	defenceLeft->setDuration(DELAY_HELP);
	setColorEffect(defenceLeft, effectColor);
	this->addChild(defenceLeft);
	defenceLeft->setPosition(posLeft);


}


/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT ATTACK
/////////////////////////////////////////////////////////////////////////////////////////

void Effect::createEffectAttackFire(std::string plistEffectPath, int effectColor)
{
	ParticleSystemQuad* skillAttackFire = ParticleSystemQuad::create(plistEffectPath); //_1s
	skillAttackFire->setDuration(20);
	skillAttackFire->setScale(0.6f);
	setColorEffect(skillAttackFire , effectColor);

	this->addChild(skillAttackFire, 100);
	skillAttackFire->setPosition(Vec2::ZERO);
	//skillAttackFire->setDuration(DELAY_ATTACK_FIRE_LIFE);
	

}

void Effect::createEffectAttackThunder(std::string plistEffectPath, int effectColor , Vec2 attackPositon)
{
	ParticleSystemQuad* skillAttackThunder = ParticleSystemQuad::create(plistEffectPath);
	
	setColorEffect(skillAttackThunder , effectColor);
	this->addChild(skillAttackThunder , 100);
	skillAttackThunder->setPosition(attackPositon);
}


/////////////////////////////////////////////////////////////////////////////////////////
// EFFECT COLOR
/////////////////////////////////////////////////////////////////////////////////////////
void Effect::setColorEffect(ParticleSystemQuad* effect, int effectColor)
{
	switch (effectColor)
	{
	case EffectColor::EC_Other:
		break;
	case EffectColor::EC_Blue:
		effect->setStartColor(Color4F::BLUE);
		effect->setEndColor(Color4F::BLUE);
		break;
	case EffectColor::EC_Green:
		effect->setStartColor(Color4F::GREEN);
		effect->setEndColor(Color4F::GREEN);
		break;
	case EffectColor::EC_Red:
		effect->setStartColor(Color4F::RED);
		effect->setEndColor(Color4F::RED);
		break;
	case EffectColor::EC_Yellow:
		effect->setStartColor(Color4F::YELLOW);
		effect->setEndColor(Color4F::YELLOW);
		break;
	default:
		break;
	}
}