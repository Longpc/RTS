#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "cocos2d.h"


#define DELAY_HELP 1.0f // Time life of help effect
#define DELAY_RESTORE 1.0f // Time life of restore effect
#define DELAY_REMOVE_ECLIPSE 1.2f //1.2
#define DELAY_ATTACK_THUNDER_LIFE 1.0f
#define DELAY_ATTACK_MOVE 1.0f //2.0
#define DELAY_ATTACK_FIRE_LIFE 1.0f // Life = move + remove_delay
#define MOVE_TIME_INTERVAL 0.2f


USING_NS_CC;

class Effect : public Sprite
{
public:

	Effect();
	~Effect();

	////////////////////////////////////////////////////////////
	// EFFECT INIT
	////////////////////////////////////////////////////////////
	static Effect* create();
	virtual bool init();
	static Effect* createWithParticle(std::string plistEffectPath, int effectColor , int skillType);
	virtual bool initWithParticle(std::string plistEffectPath, int effectColor , int skillType);

	virtual void onEnter();
	virtual void updateAttackMove(float dt);
	virtual void setTargetAttack(Sprite* attackSprite);
	////////////////////////////////////////////////////////////
	// EFFECT COLOR
	////////////////////////////////////////////////////////////
	virtual void setColorEffect(ParticleSystemQuad* effect, int effectColor);
	////////////////////////////////////////////////////////////
	// EFFECT HELP
	////////////////////////////////////////////////////////////
	virtual void createEffectHelp(std::string plistEffectPath, int effectColor);

	virtual void createEffectHelpDefence(std::string plistEffectPath , int effectColor);
	

	////////////////////////////////////////////////////////////
	// EFFECT HEAL RESTORE WITH COLOR
	////////////////////////////////////////////////////////////
	virtual void createEffectHeal(std::string plistEffectPath, int effectColor);

	////////////////////////////////////////////////////////////
	// EFFECT ATTACK FIRE
	////////////////////////////////////////////////////////////
	virtual void createEffectAttackFire(std::string plistEffectPath, int effectColor);

	////////////////////////////////////////////////////////////
	// EFFECT ATTACK THUNDER
	////////////////////////////////////////////////////////////
	virtual void createEffectAttackThunder(std::string plistEffectPath, int effectColor , Vec2 attackPositon);


	////////////////////////////////////////////////////////////
	// Effect Color
	////////////////////////////////////////////////////////////
	enum EffectColor
	{
		EC_Other = 0, // Begin Effect Color
		EC_Blue = 1,
		EC_Red = 2,
		EC_Green = 3,
		EC_Yellow = 4
	};

	////////////////////////////////////////////////////////////
	// Skill Type
	////////////////////////////////////////////////////////////
	enum SkillType
	{
		Restore = 1 ,
		Help = 2 ,
		Attack = 3 ,
		Help_Defence = 4
	};

protected:
private:

	Sprite* _spriteAttack;

};


#endif // _EFFECT_H_
