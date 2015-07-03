#include "LoadingSprite.h"

LoadingSprite * LoadingSprite::loading = nullptr;

bool LoadingSprite::init()
{
	if (!LayerColor::init()) {
		return false;
	}
	this->initWithColor(Color4B::BLACK);
	this->setSwallowsTouches(false);
	log("create loading sprite");
	auto sp = Sprite::create("loading.png");
	sp->setScale(1.5f);
	sp->setPosition(getContentSize()/2);
	addChild(sp);
	auto rotate = RotateBy::create(1.0f, 90);
	sp->runAction(RepeatForever::create(rotate));

	return true;
}

LoadingSprite* LoadingSprite::getInstance()
{
	if (!loading)
	{
		loading = new LoadingSprite();
		loading->init();
		loading->autorelease();
	}
	return loading;
}

void LoadingSprite::showLoadingFail()
{
	
	MenuItemFont::setFontName(JAPANESE_FONT_1_HEAVY);
	MenuItemFont::setFontSize(80);
	auto closeItem = MenuItemFont::create("CLOSE", [&](Ref *p) {
		Director::getInstance()->end();
	});
	auto menu = Menu::create(closeItem, NULL);
	menu->alignItemsVertically();
	this->addChild(menu);
	menu->setPosition(this->getContentSize() / 2);
}

void LoadingSprite::destroyInstance()
{
	this->removeFromParentAndCleanup(true);
	loading = nullptr;
}


