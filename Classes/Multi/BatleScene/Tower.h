#ifndef TOWER_
#define TOWER_



#define NEUTRAL_IMG_PATH "neutral_tower_big.png"

#define BLUE_IMG_PATH "tower_blue.png"
#define BLUE_ATTACK_IMG_PATH "image/unit_new/attack/tower/tower_blue_attack_"
#define BLUE_ATTACK_ANIMATION_PLIST_PATH "Effect/blueTower_Attack.plist"

#define RED_IMG_PATH "tower_red.png"
#define RED_ATTACK_IMG_PATH "image/unit_new/attack/tower/tower_red_attack_"
#define RED_ATTACK_ANIMATION_PLIST_PATH "Effect/redTower_Attack.plist"




#include "base/Define.h"
typedef std::function<void(Ref *p)> Callback;
class Tower : public Sprite
{
public:
	/*Create tower as sprite with type: 0 is neutral tower, 1 is blue tower and 2 for red tower*/
	static Tower* createTower(int type);
	virtual bool init(int type);
	virtual void runAttackAnimationWithTarget(Sprite* target, Callback cb);

	virtual void changeTowerType(int type, bool isInit = false);
	CC_SYNTHESIZE(int, _towerType, TowerType);
	CC_SYNTHESIZE(int, _attackStatus, AttackStatus);



protected:
private:
	virtual Animation* createTowerAttackAnimation();
	virtual void createEffectObject(Sprite* target);
	virtual void updatePosition(float dt);

	Texture2D* _currentTexture;
	string _attackImgPath = "";
	string _plistPath = "";
	ParticleSystemQuad* ef;
	Sprite* _target;

	Texture2D* _neutralTexture;
	Texture2D* _blueTexture;
	Texture2D* _redTexture;
};

#endif