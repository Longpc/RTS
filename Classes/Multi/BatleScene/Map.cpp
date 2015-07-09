#include "Map.h"

MyMap* MyMap::createMap(string tmxFile)
{
	auto map = new MyMap();
	map->init();
	if (map->initWithTMXFile(tmxFile))
	{
		map->autorelease();
		return map;
	}
	CC_SAFE_DELETE(map);
	return nullptr;
}

bool MyMap::init()
{
	if (!TMXTiledMap::init()) {
		return false;
	}

	return true;
}

cocos2d::Vec2 MyMap::getPostionForTitleCoor(Vec2 titleCoor)
{
	return this->getLayer("main_layer")->getTileAt(titleCoor)->getPosition();
}



cocos2d::Vec2 MyMap::getTitleCoorForPosition(Vec2 location)
{
	int x = location.x / this->getTileSize().width;
	int y = ((this->getMapSize().height*this->getTileSize().height - location.y) / this->getTileSize().height);
	return Vec2(x, y);
}

bool MyMap::checkPosInsizeMap(Vec2 pos)
{
	if (pos.x < 0 || pos.y < 0 || pos.x > this->getContentSize().width || pos.y > this->getContentSize().height) return false;

	return true;
}

Sprite* MyMap::getTitleAt(Vec2 titleCoor)
{
	return this->getLayer("main_layer")->getTileAt(titleCoor);
}
