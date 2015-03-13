#include "AnimationFromImage.h"

AnimationFromImage* AnimationFromImage::createAnimation(int imageId, string path)
{
	auto mAni = new AnimationFromImage();
	mAni->init(imageId,path);
	mAni->create(mAni->getAnimation());
	mAni->autorelease();
	return mAni;
}

void AnimationFromImage::init(int imageId, string path)
{
	_imageId = imageId;
	_imageFilePath = path;

}

Animation* AnimationFromImage::getAnimation()
{
	auto animation = Animation::create();
	for (int i = 1; i < 3; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "unit_00_0%d_%d.png", _imageId, i);
		string p = _imageFilePath;
		p.append(szName);
		animation->addSpriteFrameWithFile(p.c_str());
	}
	// should last 2.8 seconds. And there are 14 frames.
	animation->setDelayPerUnit(0.2f);
	animation->setRestoreOriginalFrame(true);
	animation->setLoops(true);
	return animation;
}


