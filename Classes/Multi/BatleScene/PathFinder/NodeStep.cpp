#include "NodeStep.h"

NodeStep::NodeStep() :
_position(Vec2::ZERO),
_gScore(0),
_hScore(0),
_parent(nullptr)
{

}

NodeStep::~NodeStep()
{

}

NodeStep* NodeStep::pooledNode(Vec2 pos, vector<NodeStep*> *_poolAvaiable, vector<NodeStep*> *_pool)
{
	if (_poolAvaiable->size() == 0) 
	{
		NodeStep *newNode = NodeStep::createWithPosition(pos);
		_pool->push_back(newNode);
		return newNode;
	}
	else
	{
		NodeStep* oldNode = _poolAvaiable->back();
		_poolAvaiable->pop_back();
		oldNode->setX(pos.x);
		oldNode->setY(pos.y);
		oldNode->setGScore(0);
		oldNode->setHScore(0);
		oldNode->setParent(nullptr);
		return oldNode;

	}
}

NodeStep *NodeStep::createWithPosition(const Vec2 pos)
{
	NodeStep *pRet = new NodeStep();
	if (pRet && pRet->initWithPosition(pos))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool NodeStep::initWithPosition(const Vec2 pos)
{
	setPosition(pos);
	setX(pos.x);
	setY(pos.y);
	return true;
}

float NodeStep::distanceTo(NodeStep* other)
{
	return (other->getPosition() - this->getPosition()).length();
}

int NodeStep::getFScore() const
{
	return this->getGScore() + this->getHScore();
}

bool NodeStep::isEqual(const NodeStep *other) const
{
	return this->getPosition() == other->getPosition();
}

std::string NodeStep::getDescription() const
{
	return StringUtils::format("pos=[%.0f;%.0f]  g=%d  h=%d  f=%d",
		this->getPosition().x, this->getPosition().y,
		this->getGScore(), this->getHScore(), this->getFScore());
}
