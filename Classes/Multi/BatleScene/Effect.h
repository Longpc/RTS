#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "cocos2d.h"


#define DELAY_HELP 1.0
#define DELAY_RESTORE 1.0
#define DELAY_REMOVE_ECLIPSE 1.5
#define DELAY_ATTACK_THUNDER_LIFE 1.0
#define DELAY_ATTACK_MOVE 2.0
#define DELAY_ATTACK_FIRE_LIFE 3.0 // Life = move + remove_delay

USING_NS_CC;

class Effect
{
public:

	////////////////////////////////////////////////////////////
	// EFFECT HELP
	////////////////////////////////////////////////////////////
	virtual void createEffectHelp(Sprite* spriteUnit, std::string plistPathHorizontal, std::string plistPathVertical, std::string eclipseImagePath);
	virtual void createHelpWallTop(Sprite* spriteUnit, const std::string plistPathHorizontal);
	virtual void createHelpWallBottom(Sprite* spriteUnit, const std::string plistPathHorizontal);
	virtual void createHelpWallLeft(Sprite* spriteUnit, const std::string plistPathVertical);
	virtual void createHelpWallRight(Sprite* spriteUnit, const std::string plistPathVertical);

	////////////////////////////////////////////////////////////
	// EFFECT HEAL RESTORE WITH COLOR
	////////////////////////////////////////////////////////////
	ParticleSystemQuad* createEffectHeal(std::string plistEffectPath, int effectColor);
	virtual void runEffectHeal(Sprite* spriteUnit, ParticleSystemQuad* skillHealEffect, std::string sorceryImagePath);

	////////////////////////////////////////////////////////////
	// EFFECT ATTACK FIRE
	////////////////////////////////////////////////////////////
	ParticleSystemQuad* createEffectAttackFire(std::string plistEffectPath);
	virtual void runEffectAttackFire(Sprite* spriteUnit, ParticleSystemQuad* skillAttackFireEffect, std::string eclipseFilePath, Vec2 attackPositon);

	////////////////////////////////////////////////////////////
	// EFFECT ATTACK THUNDER
	////////////////////////////////////////////////////////////
	ParticleSystemQuad* createEffectAttackThunder(std::string plistEffectPath);
	virtual void runEffectAttackThunder(Sprite* spriteUnit, ParticleSystemQuad* skillAttackThunderEffect, std::string eclipseFilePath, Vec2 attackPositon);


	////////////////////////////////////////////////////////////
	// SORCERY ECLIPSE
	////////////////////////////////////////////////////////////
	Sprite* createSorceryEclipse(Sprite* spriteUnit, std::string eclipseFilePath);
	void removeSorceryEclipse(Ref* pSender);

	////////////////////////////////////////////////////////////
	// Effect Color
	////////////////////////////////////////////////////////////
	enum EffectColor
	{
		EC_Other = 0,
		EC_Blue = 1,
		EC_Red = 2,
		EC_Green = 3,
		EC_Yellow = 4
	};

protected:
private:

};


#endif // _EFFECT_H_
