#include "PathFinder.h"

PathFinder* PathFinder::mPathFinder = NULL;

PathFinder::PathFinder()
{

}

PathFinder::~PathFinder()
{
	_spClosedSteps.clear();
	_spOpenSteps.clear();
	_shortestPath.clear();
	_poolAvaiable.clear();
	_pool.clear();
	_myMap = nullptr;
}

PathFinder* PathFinder::getInstance()
{
	if (mPathFinder == NULL) {
		mPathFinder = new PathFinder();
		mPathFinder->init();
	}
	return mPathFinder;
}

void PathFinder::init()
{
	for (int a = -1; a < 2; a++)
	{
		for (int b = -1; b < 2; b++)
		{
			if (a == 0 && b == 0) continue;
			sides.push_back(Vec2(a, b));
		}
	}
}

vector<Vec2> PathFinder::findPath(Vec2 beginPos, Vec2 endPos)
{
	_jump = 0;
	dumpPool();
	vector<Vec2> result = {};

	Vec2 fromTitleCoord = _myMap->getTitleCoorForPosition(beginPos);
	Vec2 desTitleCoord = _myMap->getTitleCoorForPosition(endPos);
	if (desTitleCoord.x > _myMap->getMapSize().width) {
		desTitleCoord.x = _myMap->getMapSize().width;
	}
	if (desTitleCoord.y > _myMap->getMapSize().height) {
		desTitleCoord.y = _myMap->getMapSize().height;
	}
	if (desTitleCoord.x < 0) {
		desTitleCoord.x = 0;
	}
	if (desTitleCoord.y < 0) {
		desTitleCoord.y = 0;
	}
	if (fromTitleCoord == desTitleCoord)
	{
		log("You are already in the destination");
		return result;
	}
	//Rock, tree check
	log("From: %f, %f:", fromTitleCoord.x, fromTitleCoord.y);
	log("To: %f, %f", desTitleCoord.x, desTitleCoord.y);
	if (isBlocked(desTitleCoord))
	{
		log("you cannot move there");
		return result;
	}
	_spClosedSteps.clear();
	_spOpenSteps.clear();
	_shortestPath.clear();
	NodeStep* startNode = NodeStep::pooledNode(fromTitleCoord, &_poolAvaiable, &_pool);
	NodeStep* endNode = NodeStep::pooledNode(desTitleCoord, &_poolAvaiable, &_pool);

	startNode->setHScore(nodeHeuristic(startNode->getPosition(), endNode->getPosition()));
	_spOpenSteps.pushBack(startNode);

	do
	{
		NodeStep *curStep = _spOpenSteps.at(0);

		_spClosedSteps.pushBack(curStep);

		_spOpenSteps.erase(0);

		if (curStep->getPosition() == desTitleCoord)
		{
			log("Here the path");
			//
			result = constructPathAndStartAnimationFromStep(curStep);
			_spClosedSteps.clear();
			_spOpenSteps.clear();
			log("found");
			return result;
		}

		vector<NodeStep*> successors = indentifySuccessors(curStep, startNode, endNode);

		for (size_t i = 0; i < successors.size(); i++)
		{
			NodeStep *jumpNode = successors[i];

			/*if (_myMap->checkWallInCoord(successors[i]->getPosition())) {
			continue;
			}*/
			if (checkContain(_spClosedSteps, jumpNode))
			{
				continue;
			}
			/*if (getStepIndex(_spClosedSteps, jumpNode) != -1) {
			continue;
			}*/
			float moveCost = curStep->distanceTo(jumpNode);
			//ssize_t index = getStepIndex(_spOpenSteps, jumpNode);
			if (!checkContain(_spOpenSteps, jumpNode) || curStep->getGScore() + moveCost < jumpNode->getGScore())
			{
				jumpNode->setParent(curStep);
				jumpNode->setGScore(curStep->getGScore() + moveCost);
				jumpNode->setHScore(nodeHeuristic(jumpNode->getPosition(), endNode->getPosition()));
				_spOpenSteps.pushBack(jumpNode);
			}
		}
	} while (_spOpenSteps.size() > 0);

	log("Not Found");
	return result;
}

vector<NodeStep*> PathFinder::nodeNeighbours(NodeStep *curentNode)
{
	vector<NodeStep*> neighbours = {};
	if (!curentNode->getParent())
	{
		for (int i = 0; i < 8; i++)
		{
			int potentialX = curentNode->getX() + sides[i].x;
			int potentialY = curentNode->getY() + sides[i].y;
			if (!isBlocked(Vec2(potentialX, potentialY)))
			{
				neighbours.push_back(NodeStep::pooledNode(Vec2(potentialX, potentialY), &_poolAvaiable, &_pool));

			}
		}
		return neighbours;
	}

	int directionX = min(max(-1, curentNode->getX() - curentNode->getParent()->getX()), 1);
	int directionY = min(max(-1, curentNode->getY() - curentNode->getParent()->getY()), 1);
	if (directionX != 0 && directionY != 0)
	{
		if (!isBlocked(Vec2(curentNode->getX(), curentNode->getY() + directionY)))
		{
			neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX(), curentNode->getY() + directionY), &_poolAvaiable, &_pool));
		}

		if (!isBlocked(Vec2(curentNode->getX() + directionX, curentNode->getY())))
		{
			neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY()), &_poolAvaiable, &_pool));
		}

		if (!isBlocked(Vec2(curentNode->getX(), curentNode->getY() + directionY)) ||
			!isBlocked(Vec2(curentNode->getX() + directionX, curentNode->getY())))
		{
			neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY() + directionY), &_poolAvaiable, &_pool));
		}

		if (isBlocked(Vec2(curentNode->getX() - directionX, curentNode->getY())) &&
			!isBlocked(Vec2(curentNode->getX(), curentNode->getY() + directionY)))
		{
			neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() - directionX, curentNode->getY() + directionY), &_poolAvaiable, &_pool));
		}

		if (isBlocked(Vec2(curentNode->getX(), curentNode->getY() - directionY)) &&
			!isBlocked(Vec2(curentNode->getX() + directionX, curentNode->getY())))
		{
			neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY() - directionY), &_poolAvaiable, &_pool));
		}
	}
	else
	{
		if (directionX == 0)
		{
			if (!isBlocked(Vec2(curentNode->getX(), curentNode->getY() + directionY))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX(), curentNode->getY() + directionY), &_poolAvaiable, &_pool));
			}

			if (isBlocked(Vec2(curentNode->getX() + 1, curentNode->getY()))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + 1, curentNode->getY() + directionY), &_poolAvaiable, &_pool));
			}

			if (isBlocked(Vec2(curentNode->getX() - 1, curentNode->getY()))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() - 1, curentNode->getY() + directionY), &_poolAvaiable, &_pool));
			}
		}
		else
		{
			if (!isBlocked(Vec2(curentNode->getX() + directionX, curentNode->getY()))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY()), &_poolAvaiable, &_pool));
			}

			if (isBlocked(Vec2(curentNode->getX(), curentNode->getY() + 1))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY() + 1), &_poolAvaiable, &_pool));
			}

			if (isBlocked(Vec2(curentNode->getX(), curentNode->getY() - 1))) {
				neighbours.push_back(NodeStep::pooledNode(Vec2(curentNode->getX() + directionX, curentNode->getY() - 1), &_poolAvaiable, &_pool));
			}
		}
	}
	return neighbours;
}

NodeStep* PathFinder::jumpNode(int currentX, int currentY, int directionX, int directionY, NodeStep* start, NodeStep* end)
{
	_jump++;
	int nextX = currentX + directionX;
	int nextY = currentY + directionY;
	if (isBlocked(Vec2(nextX, nextY))) {
		return nullptr;
	}

	if (nextX == end->getX() && nextY == end->getY())
	{
		return NodeStep::pooledNode(Vec2(nextX, nextY), &_poolAvaiable, &_pool);
	}

	int offsetX = nextX;
	int offsetY = nextY;

	if (directionX != 0 && directionY != 0) {
		while (true)
		{
			if (!isBlocked(Vec2(offsetX - directionX, offsetY + directionY)) &&
				isBlocked(Vec2(offsetX - directionX, offsetY)) ||
				!isBlocked(Vec2(offsetX + directionX, offsetY - directionY)) &&
				isBlocked(Vec2(offsetX, offsetY - directionY)))
			{
				return NodeStep::pooledNode(Vec2(offsetX, offsetY), &_poolAvaiable, &_pool);
			}

			if (jumpNode(offsetX, offsetY, directionX, 0, start, end) != nullptr || jumpNode(offsetX, offsetY, 0, directionY, start, end) != nullptr)
			{
				return NodeStep::pooledNode(Vec2(offsetX, offsetY), &_poolAvaiable, &_pool);
			}

			offsetX += directionX;
			offsetY += directionY;
			if (isBlocked(Vec2(offsetX, offsetY))) {
				return nullptr;
			}

			if (offsetX == end->getX() && offsetY == end->getY()) {
				return NodeStep::pooledNode(Vec2(offsetX, offsetY), &_poolAvaiable, &_pool);
			}
		}
	}
	else
	{
		if (directionX != 0)
		{
			while (true)
			{
				if (!isBlocked(Vec2(offsetX + directionX, nextY + 1)) &&
					isBlocked(Vec2(offsetX, nextY + 1)) ||
					!isBlocked(Vec2(offsetX + directionX, nextY - 1)) &&
					isBlocked(Vec2(offsetX, nextY - 1)))
				{
					return NodeStep::pooledNode(Vec2(offsetX, nextY), &_poolAvaiable, &_pool);
				}

				offsetX += directionX;
				if (isBlocked(Vec2(offsetX, nextY))) {
					return nullptr;
				}
				if (offsetX == end->getX() && nextY == end->getY()) {
					return NodeStep::pooledNode(Vec2(offsetX, nextY), &_poolAvaiable, &_pool);
				}
			}
		}
		else {
			while (true)
			{
				if (!isBlocked(Vec2(nextX + 1, offsetY + directionY)) &&
					isBlocked(Vec2(nextX + 1, offsetY)) ||
					!isBlocked(Vec2(nextX - 1, offsetY + directionY)) &&
					isBlocked(Vec2(nextX - 1, offsetY)))
				{
					return NodeStep::pooledNode(Vec2(nextX, offsetY), &_poolAvaiable, &_pool);
				}

				offsetY += directionY;
				if (isBlocked(Vec2(nextX, offsetY))) {
					return nullptr;
				}
				if (nextX == end->getX() && offsetY == end->getY()) {
					return NodeStep::pooledNode(Vec2(nextX, offsetY), &_poolAvaiable, &_pool);
				}
			}
		}
	}
	return jumpNode(nextX, nextY, directionX, directionY, start, end);
}

vector<NodeStep*> PathFinder::indentifySuccessors(NodeStep* current, NodeStep* start, NodeStep* end)
{
	vector<NodeStep*> successors = {};
	vector<NodeStep*> neighbours = {};

	neighbours = nodeNeighbours(current);

	for (auto &neighbour : neighbours)
	{
		int directionX = min(max(-1, neighbour->getX() - current->getX()), 1);
		int directionY = min(max(-1, neighbour->getY() - current->getY()), 1);
		int dX = clampf(neighbour->getX(), -1, 1);
		int dy = clampf(neighbour->getY(), -1, 1);
		NodeStep* mjumpNode = jumpNode(current->getX(), current->getY(), directionX, directionY, start, end);
		if (mjumpNode) successors.push_back(mjumpNode);
	}

	return successors;
}
vector<Vec2> PathFinder::constructPathAndStartAnimationFromStep(NodeStep *step)
{
	_shortestPath.clear();
	do
	{
		if (step->getParent())
		{
			_shortestPath.insert(0, step);
		}
		step = step->getParent();
	} while (step);
	vector<Vec2> result = {};
	for (const NodeStep *s : _shortestPath)
	{
		log("%s", s->getDescription().c_str());
		result.push_back(s->getPosition());
	}
	return result;
}

int PathFinder::nodeHeuristic(const Vec2 &fromCoord, const Vec2 &toCoord)
{
	return abs(toCoord.x - fromCoord.x) + abs(toCoord.y - fromCoord.y);
}

bool PathFinder::isBlocked(Vec2 titleCoord)
{
	//if this title can not walk thought return true. Else return false
	if (!_myMap->checkCoodInsideMap(titleCoord)) return true;
	return _myMap->checkWallInCoord(titleCoord);
	//return false;
}

bool PathFinder::checkContain(const Vector<NodeStep*> &steps, const NodeStep *step)
{
	for (ssize_t i = 0; i < steps.size(); ++i)
	{
		if (steps.at(i)->getPosition() == step->getPosition())
		{
			return true;
		}
	}
	return false;
}

void PathFinder::destroyInstance()
{
	CC_SAFE_DELETE(mPathFinder);
}

