#include "Effect.h"

USING_NS_CC;

/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT HELP
/////////////////////////////////////////////////////////////////////////////////////////

void Effect::createEffectHelp(Sprite* spriteUnit, std::string plistPathHorizontal, std::string plistPathVertical, std::string eclipseImagePath)
{
	// Eclipse create and rotate
	_sorcery = createSorceryEclipse(spriteUnit, eclipseImagePath);

	// 4‚Â–Ê‚Ì–hŒä
	createHelpWallTop(spriteUnit, plistPathHorizontal);
	createHelpWallRight(spriteUnit, plistPathVertical);
	createHelpWallBottom(spriteUnit, plistPathHorizontal);
	createHelpWallLeft(spriteUnit, plistPathVertical);

	// Create effect and remove eclipse
	Sequence *runEffect = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE), // Thgian cho 4 effect kia thuc hien xong
		CallFuncN::create(CC_CALLBACK_0(Effect::removeSorceryEclipse, this)),
		NULL
		);
	spriteUnit->runAction(runEffect);

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
void Effect::createEffectHeal(Sprite* spriteUnit, std::string plistEffectPath, std::string eclipseFilePath)
{
	_sorcery = createSorceryEclipse(spriteUnit, eclipseFilePath);

	ParticleSystem* skillHealEffect = ParticleSystemQuad::create(plistEffectPath); //_1s
	spriteUnit->addChild(skillHealEffect);
	skillHealEffect->setPosition(Vec2(spriteUnit->getContentSize().width / 2, spriteUnit->getContentSize().height / 2));

	//removeSorceryEclipse
	auto sequence = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE),
		CallFuncN::create(CC_CALLBACK_0(Effect::removeSorceryEclipse, this)),
		NULL
		);
	spriteUnit->runAction(sequence);
}

/////////////////////////////////////////////////////////////////////////////////////////
// SKILL EFFECT ATTACK
/////////////////////////////////////////////////////////////////////////////////////////
void Effect::createEffectAttackFire(Sprite* spriteUnit, std::string plistEffectPath, std::string eclipseFilePath , Vec2 attackPositon)
{
	_sorcery = createSorceryEclipse(spriteUnit, eclipseFilePath);

	// Thuc hien effect 1s
	ParticleSystem* skillAttackEffect = ParticleSystemQuad::create(plistEffectPath); //_1s
	spriteUnit->addChild(skillAttackEffect);
	skillAttackEffect->setDuration(3.0f);
	skillAttackEffect->setPosition(Vec2(spriteUnit->getContentSize().width / 2, spriteUnit->getContentSize().height / 2));
	
	skillAttackEffect->runAction(MoveTo::create(DELAY_ATTACK_MOVE, attackPositon - spriteUnit->getPosition()));


	//removeSorceryEclipse
	auto sequenceRemoveEclipse = Sequence::create(
		DelayTime::create(DELAY_REMOVE_ECLIPSE), // Delay remove eclipse >= effect run time
		CallFuncN::create(CC_CALLBACK_0(Effect::removeSorceryEclipse, this)),
		NULL
		);

	spriteUnit->runAction(sequenceRemoveEclipse);

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
	sorcery->setScaleX(0.075f);
	sorcery->setScaleY(0.075f);
	sorcery->setPosition(Vec2(0, 0));

	auto actionRotate = RotateBy::create(1.0f, Vec3(0, 0, 150));
	auto repeat = RepeatForever::create(actionRotate);
	sorcery->runAction(repeat);

	nodeFather->addChild(sorcery, -10);
	nodeFather->setScaleY(0.5f);

	return nodeFather;
}


void Effect::removeSorceryEclipse()
{
	log("Remove eclipse");

	if (_sorcery != nullptr)
	{
		_sorcery->removeFromParentAndCleanup(true);
	}
	else
	{
		return;
	}
}
