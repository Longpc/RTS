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

	bool checkPosInsideMap(Vec2 pos);
	bool checkCoodInsideMap(Vec2 coord);

	bool checkTileCantGet(Vec2 pos);
	bool checkWallInCoord(Vec2 coord);
private:
	bool checkTitleNearObject(vector<Vec2> vec, int offset, Vec2 pos);
	/*Draw rectangle in map*/
	virtual void drawRectInMap(Vec2 beginTitleCoor, int offset);
	vector<Vec2> _towerCoor;
	vector<Vec2> _cannonCoor;
	TMXLayer* _wallLayer;
};


#endif