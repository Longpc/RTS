#ifndef PATH_FINDER_H__
#define PATH_FINDER_H__

#include "base/Define.h"
#include "NodeStep.h"
#include "Multi/BatleScene/Map.h"

USING_NS_CC;
using namespace std;
class PathFinder
{
public:
	PathFinder();
	~PathFinder();
	static PathFinder* getInstance();
	static void destroyInstance();
	void init();
	vector<Vec2> findPath(Vec2 beginPos, Vec2 endPos);
	void setMap(MyMap* map) {
		_myMap = map;
	};

protected:
private:
	static PathFinder* mPathFinder;
	MyMap* _myMap;
	Vector<NodeStep*> _spOpenSteps;
	Vector<NodeStep*> _spClosedSteps;
	Vector<NodeStep*> _shortestPath;

	vector<NodeStep*> _pool;
	vector<NodeStep*> _poolAvaiable;
	int _jump;
	vector<Vec2> sides;
	
	virtual vector<NodeStep*> nodeNeighbours(NodeStep *curentNode);
	virtual NodeStep* jumpNode(int currentX, int currentY, int directionX, int directionY, NodeStep* start, NodeStep* end);
	virtual vector<NodeStep*> indentifySuccessors(NodeStep* current, NodeStep* start, NodeStep* end);
	vector<Vec2> constructPathAndStartAnimationFromStep(NodeStep *step);
	int nodeHeuristic(const Vec2 &fromCoord, const Vec2 &toCoord);
	virtual bool isBlocked(Vec2 titleCoord);
	bool checkContain(const Vector<NodeStep*> &steps, const NodeStep *step);
	
	void dumpPool() {
		_poolAvaiable = {};
		_pool = {};
	}

};

#endif