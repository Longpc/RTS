#ifndef ANIMATION_SPRITE
#define ANIMATION_SPRITE

#include "cocos2d.h"

USING_NS_CC;
using namespace std;

class AnimationFromPlist :public Animate
{
public:
	static AnimationFromPlist* createAnimation();
	virtual bool init();
	virtual void setPlistFilePath(string path);
	virtual void setDelayTime(float delay);
	virtual void stopAction();

protected:
	SpriteFrameCache *_mSpriteFrame;
	Animation *_mAnimation;
	float _delayTime;
	string _plistFilepath;
private:
};

#endif