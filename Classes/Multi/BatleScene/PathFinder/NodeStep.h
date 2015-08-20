#ifndef NODESTEP_H__
#define NODESTEP_H__

#include "base/Define.h"
/************************************************************************/
/* CLASS USING FOR DETECT SHORTEST PATH BY A* ALGORITHM                 */
/************************************************************************/
class NodeStep : public cocos2d::Object
{
public:
	NodeStep();
	~NodeStep();

	static NodeStep* pooledNode(Vec2 pos, vector<NodeStep*> *_poolAvaiable, vector<NodeStep*> *_pool);

	static NodeStep *createWithPosition(const Vec2 pos);
	bool initWithPosition(const Vec2 pos);

	int getFScore() const;
	float distanceTo(NodeStep* other);
	bool isEqual(const NodeStep *other) const;
	std::string getDescription() const;
	CC_SYNTHESIZE(Vec2, _position, Position);
	CC_SYNTHESIZE(int, _x, X);
	CC_SYNTHESIZE(int, _y, Y);
	CC_SYNTHESIZE(int, _gScore, GScore);
	CC_SYNTHESIZE(int, _hScore, HScore);
	CC_SYNTHESIZE(NodeStep*, _parent, Parent);

private:

};

#endif