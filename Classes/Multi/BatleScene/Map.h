#ifndef MY_TITLED_MAP
#define MY_TITLED_MAP
#include "base/Define.h"

using namespace std;

class MyMap : public TMXTiledMap
{
public:
	static MyMap* createMap(string tmxFile);
	bool init();
	Vec2 getPostionForTitleCoor(Vec2 titleCoor);
	Vec2 getTitleCoorForPosition(Vec2 pos);
	Sprite* getTitleAt(Vec2 titleCoor);
	bool checkPosInsizeMap(Vec2 pos);
private:

};


#endif