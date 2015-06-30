/*this class define to display cannon image and run cannon logic/effect*/
#ifndef MY_CANNON_CLASS
#define MY_CANNON_CLASS

#include "cocos2d.h"

#include "base/Define.h"

using namespace std;

class Cannon : public Sprite
{
public:
	Cannon();
	~Cannon();

	static Cannon* createCannon();
	virtual bool init();

	CC_SYNTHESIZE(string, _imagePath, ImagePath);
	CC_SYNTHESIZE(int, _beAttackedTime, AttackedTime);
	CC_SYNTHESIZE(int, _curTeamId, CurrentTeamId);
	CC_SYNTHESIZE(bool, _isDisable, DisableFlg);
	void onEnter();
	void changeTeamStatus(int teamId, bool initFlg = false);
	void disableCannon();
protected:
private:
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
};





#endif