#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "cocos2d.h"


#define DELAY_HELP 1.0
#define DELAY_RESTORE 1.0
#define DELAY_REMOVE_ECLIPSE 1.0
#define DELAY_ATTACK_MOVE 2.0
#define DELAY_ATTACK_LIVE 3.0

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
	// EFFECT HEAL RESTORE
	////////////////////////////////////////////////////////////
	virtual void createEffectHeal(Sprite* spriteUnit, std::string plistEffectPath, std::string sorceryImagePath);

	////////////////////////////////////////////////////////////
	// EFFECT ATTACK
	////////////////////////////////////////////////////////////
	virtual void createEffectAttackFire(Sprite* spriteUnit, std::string plistEffectPath, std::string sorceryImagePath, Vec2 attackPostion);
	
	// Sorcery

	Sprite *_sorcery;
	Sprite* createSorceryEclipse(Sprite* spriteUnit, std::string eclipseFilePath);
	void removeSorceryEclipse();

private:

};


#endif // _EFFECT_H_