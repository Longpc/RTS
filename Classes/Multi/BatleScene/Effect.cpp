#include "Effect.h"

USING_NS_CC;


/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT HELP
/////////////////////////////////////////////////////////////////////////////////////////

void Effect::createEffectHelp(Sprite* spriteUnit, std::string plistPathHorizontal, std::string plistPathVertical, std::string eclipseImagePath)
{
	// Eclipse create and rotate
	auto sorcery = createSorceryEclipse(spriteUnit, eclipseImagePath);

	// 4‚Â–Ê‚Ì–hŒä
	createHelpWallTop(spriteUnit, plistPathHorizontal);
	createHelpWallRight(spriteUnit, plistPathVertical);
	createHelpWallBottom(spriteUnit, plistPathHorizontal);
	createHelpWallLeft(spriteUnit, plistPathVertical);

	// Create effect and remove eclipse
	Sequence *runEffect = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE), // Thgian cho 4 effect kia thuc hien xong
		CallFuncN::create(CC_CALLBACK_1(Effect::removeSorceryEclipse, this)),
		NULL
		);
	sorcery->runAction(runEffect);

}

void Effect::createHelpWallTop(Sprite* spriteUnit, const std::string plistPathHorizontal)
{
	Vec2 pos = Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2 + spriteUnit->getContentSize().height / 2
		);

	//"skillEffect/particle_defence_05s_h.plist"
	ParticleSystem* helpEffect = ParticleSystemQuad::create(plistPathHorizontal);
	helpEffect->setDuration(DELAY_HELP);
	spriteUnit->addChild(helpEffect);
	helpEffect->setPosition(pos);
}

void Effect::createHelpWallBottom(Sprite* spriteUnit, const std::string plistPathHorizontal)
{
	Vec2 pos = Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2 - spriteUnit->getContentSize().height / 8
		);

	//"skillEffect/particle_defence_05s_h.plist"
	ParticleSystem* helpEffect = ParticleSystemQuad::create(plistPathHorizontal);
	helpEffect->setDuration(DELAY_HELP);
	spriteUnit->addChild(helpEffect);
	helpEffect->setPosition(pos);
}

void Effect::createHelpWallLeft(Sprite* spriteUnit, const std::string plistPathVertical)
{
	Vec2 pos = Vec2(
		spriteUnit->getContentSize().width / 2 - spriteUnit->getContentSize().width / 2 + 50,
		spriteUnit->getContentSize().height / 2
		);

	//"skillEffect/particle_defence_05s_v.plist"
	ParticleSystem* helpEffect = ParticleSystemQuad::create(plistPathVertical);
	helpEffect->setDuration(DELAY_HELP);
	spriteUnit->addChild(helpEffect);
	helpEffect->setPosition(pos);
}

void Effect::createHelpWallRight(Sprite* spriteUnit, const std::string plistPathVertical)
{
	Vec2 pos = Vec2(
		spriteUnit->getContentSize().width / 2 + spriteUnit->getContentSize().width / 2 - 50,
		spriteUnit->getContentSize().height / 2
		);

	//"skillEffect/particle_defence_05s_h.plist"
	ParticleSystem* helpEffect = ParticleSystemQuad::create(plistPathVertical);
	helpEffect->setDuration(DELAY_HELP);
	spriteUnit->addChild(helpEffect);
	helpEffect->setPosition(pos);
}

/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT HEAL
/////////////////////////////////////////////////////////////////////////////////////////

ParticleSystemQuad* Effect::createEffectHeal(std::string plistEffectPath, int effectColor)
{
	ParticleSystemQuad* skillHealEffect = ParticleSystemQuad::create(plistEffectPath); //_1s
	switch (effectColor)
	{
	case EffectColor::EC_Other:
		break;
	case EffectColor::EC_Blue:
		skillHealEffect->setStartColor(Color4F::BLUE);
		skillHealEffect->setEndColor(Color4F::BLUE);
		break;
	case EffectColor::EC_Green:
		skillHealEffect->setStartColor(Color4F::GREEN);
		skillHealEffect->setEndColor(Color4F::GREEN);
		break;
	case EffectColor::EC_Red:
		skillHealEffect->setStartColor(Color4F::RED);
		skillHealEffect->setEndColor(Color4F::RED);
		break;
	case EffectColor::EC_Yellow:
		skillHealEffect->setStartColor(Color4F::YELLOW);
		skillHealEffect->setEndColor(Color4F::YELLOW);
		break;
	default:
		break;
	}

	return skillHealEffect;

}

void Effect::runEffectHeal(Sprite* spriteUnit, ParticleSystemQuad* skillHealEffect, std::string eclipseFilePath)
{
	log("Create Effect Heal Restore");
	auto sorcery = createSorceryEclipse(spriteUnit, eclipseFilePath);


	spriteUnit->addChild(skillHealEffect);
	skillHealEffect->setPosition(Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2
		));

	//removeSorceryEclipse
	auto sequence = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE),
		CallFuncN::create(CC_CALLBACK_1(Effect::removeSorceryEclipse, this)),
		NULL
		);
	sorcery->runAction(sequence);
}

/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT ATTACK
/////////////////////////////////////////////////////////////////////////////////////////
ParticleSystemQuad* Effect::createEffectAttackFire(std::string plistEffectPath)
{
	ParticleSystemQuad* skillAttackFire = ParticleSystemQuad::create(plistEffectPath); //_1s
	skillAttackFire->setScale(0.6f);

	return skillAttackFire;

}

void Effect::runEffectAttackFire(Sprite* spriteUnit, ParticleSystemQuad* skillAttackFireEffect, std::string eclipseFilePath, Vec2 attackPositon)
{
	auto sorcery = createSorceryEclipse(spriteUnit, eclipseFilePath);

	// Thuc hien effect 1s
	spriteUnit->getParent()->addChild(skillAttackFireEffect, 100);

	skillAttackFireEffect->setDuration(DELAY_ATTACK_FIRE_LIFE);
	skillAttackFireEffect->setPosition(spriteUnit->getPosition());

	skillAttackFireEffect->runAction(MoveTo::create(DELAY_ATTACK_MOVE, attackPositon));

	//removeSorceryEclipse
	auto sequenceRemoveEclipse = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE), // Delay remove eclipse >= effect run time
		CallFuncN::create(CC_CALLBACK_1(Effect::removeSorceryEclipse, this)),
		NULL
		);

	sorcery->runAction(sequenceRemoveEclipse);
}

ParticleSystemQuad* Effect::createEffectAttackThunder(std::string plistEffectPath)
{
	ParticleSystemQuad* skillAttackThunder = ParticleSystemQuad::create(plistEffectPath);

	return skillAttackThunder;
}

void Effect::runEffectAttackThunder(Sprite* spriteUnit, ParticleSystemQuad* skillAttackThunderEffect, std::string eclipseFilePath, Vec2 attackPositon)
{
	auto sorcery = createSorceryEclipse(spriteUnit, eclipseFilePath);

	// Thuc hien effect 1s
	spriteUnit->getParent()->addChild(skillAttackThunderEffect, 100);

	skillAttackThunderEffect->setDuration(DELAY_ATTACK_THUNDER_LIFE);
	skillAttackThunderEffect->setPosition(attackPositon);

	//removeSorceryEclipse
	auto sequenceRemoveEclipse = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE), // Delay remove eclipse >= effect run time
		CallFuncN::create(CC_CALLBACK_1(Effect::removeSorceryEclipse, this)),
		NULL
		);

	sorcery->runAction(sequenceRemoveEclipse);
}

/////////////////////////////////////////////////////////////////////////////////////////
// SORCERY CREATE AND EFFECT
/////////////////////////////////////////////////////////////////////////////////////////

Sprite* Effect::createSorceryEclipse(Sprite* spriteUnit, std::string eclipseFilePath)
{

	log("Create Eclipse");

	auto nodeFather = Sprite::create();
	spriteUnit->addChild(nodeFather, -10);
	nodeFather->setPosition(Vec2(
		spriteUnit->getContentSize().width / 2,
		spriteUnit->getContentSize().height / 2 - spriteUnit->getContentSize().height / 4
		));

	// Create Eclipse
	auto sorcery = Sprite::create(eclipseFilePath);
	sorcery->setScale(1.5f);
	sorcery->setPosition(Vec2(0, 0));

	nodeFather->addChild(sorcery, -10);

	auto actionRotate = RotateBy::create(1.0f, Vec3(0, 0, 150));
	auto repeat = RepeatForever::create(actionRotate);
	sorcery->runAction(repeat);

	nodeFather->setScaleY(0.5f);

	return nodeFather;
}


void Effect::removeSorceryEclipse(Ref* pSender)
{
	log("Remove eclipse");

	Sprite* sorcery = dynamic_cast<Sprite*>(pSender);

	if (sorcery != nullptr)
	{
		sorcery->removeFromParentAndCleanup(true);
	}
	else
	{
		return;
	}
}
