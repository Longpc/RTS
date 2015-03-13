#ifndef ANIMATION_FROM_IMAGE
#define ANIMATION_FROM_IMAGE

#include "cocos2d.h"

using namespace std;
USING_NS_CC;

class AnimationFromImage : public Animate
{
public:
	static AnimationFromImage* createAnimation(int imageId, string path);
	void init(int imageId, string path);
	virtual Animation* getAnimation();

private:
	Vector<SpriteFrame*> _animaFrame;
	Animation* _mAnimation;
	string _imageFilePath;
	int _imageId;
	float _delay = 0.2f;
};


#endif