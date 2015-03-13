#include "base/animationBase/AnimationSpriteFromPlist.h"

AnimationFromPlist* AnimationFromPlist::createAnimation()
{
	auto mAni = new AnimationFromPlist();
	mAni->init();
	mAni->autorelease();
	return mAni;
}

void AnimationFromPlist::setPlistFilePath(string path)
{
	_plistFilepath = path;
}

void AnimationFromPlist::setDelayTime(float delay)
{
	_delayTime = delay;
}

void AnimationFromPlist::stopAction()
{
	Animate::stop();
}

bool AnimationFromPlist::init()
{
	return true;
}
