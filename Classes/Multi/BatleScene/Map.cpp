#include "Map.h"

MyMap* MyMap::createMap(string tmxFile)
{
	auto map = new MyMap();
	if (map->initWithTMXFile(tmxFile))
	{
		map->init();
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
	_towerCoor = {
		Vec2(23, 41),
		//Vec2(46, 41),
		//Vec2(23, 8),
		Vec2(46, 8),
		Vec2(35, 26)
	};
	_cannonCoor = {
		Vec2(35, 40),
		Vec2(35, 10)
	};
	for (auto &pos : _towerCoor)
	{
		drawRectInMap(pos - Vec2(4, -4), 8);
	}
	for (auto &pos2 : _cannonCoor)
	{
		drawRectInMap(pos2 - Vec2(2, -2), 4);
	}
	return true;
}

cocos2d::Vec2 MyMap::getPostionForTitleCoor(Vec2 titleCoor)
{
	return this->getLayer("main_layer")->getTileAt(titleCoor)->getPosition() + this->getTileSize()/2;
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

bool MyMap::checkTitleCantGet(Vec2 pos)
{
	return (checkTitleNearObject(_towerCoor, 8, pos +Vec2(0, -1)) || checkTitleNearObject(_cannonCoor, 4, pos + Vec2(0, -1)));
}

bool MyMap::checkTitleNearObject(vector<Vec2> vec, int offset, Vec2 pos)
{
	for (auto &ele : vec)
	{
		if (pos.x >= ele.x - offset / 2 && pos.x < ele.x + offset / 2 && pos.y >= ele.y - offset / 2 && pos.y < ele.y + offset / 2)
		{
			return true;
		}
	}
	return false;
}

void MyMap::drawRectInMap(Vec2 beginTitleCoor, int offset)
{
	Size titleSize = this->getTileSize();
	auto pos = this->getTitleAt(beginTitleCoor)->getPosition();
	Vec2 endPos = pos + offset* titleSize;
	DrawNode * draw = DrawNode::create();
	draw->drawRect(pos, endPos, Color4F::GRAY);
	this->addChild(draw, 1);
}
