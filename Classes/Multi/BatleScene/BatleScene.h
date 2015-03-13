#ifndef BATLE_SCENE
#define BATLE_SCENE
#include "base/LayerBase.h"
#include "base/Define.h"
#include "Multi/BatleResultScene/BatleResultScene.h"


#include <time.h>
class BatleScene : public LayerBase
{

public:
	static Scene* createScene();
	CREATE_FUNC(BatleScene);
	bool init();
private:
	///LAYOUT BASE///////////////////////////////////////////////////////////////////////
	virtual void onEnter();

	///CREATE UI CONTENT + PHYSIC WORLD///////////////////////////////////////////////////////////////////////
	virtual void createContent();
	virtual Sprite* createBackground(Vec2 pos);
	virtual void createPhysicBolder();
	virtual Node* createHBolder();
	virtual Node* createVBolder();

	///BUTTON CALLBACK///////////////////////////////////////////////////////////////////////
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void menuButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	///MAIN TOUCH EVENT///////////////////////////////////////////////////////////////////////
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	
	///CHARACTER MOVE LOGIC///////////////////////////////////////////////////////////////////////
	virtual bool caculAvgAngle(int avg, float angle);
	virtual int detectDirectionBaseOnTouchAngle(float angle);
	
	///CHARACTER MOVE ACTION///////////////////////////////////////////////////////////////////////
	virtual void actionCharacter(int directionId);
	virtual Animation* createAnimationWithDefine(int imageId, string path);

	///MINIMAP LOGIC///////////////////////////////////////////////////////////////////////
	virtual void updateMiniMap();

	void update(float delta);

	///BATLE TIME///
	void updateTime();

	struct tm day;
	time_t timer;

	///UI///////////////////////////////////////////////////////////////////////
	Button *_menuButton;

	Button *_skill1Button;
	Button *_skill2Button;
	Button *_skill3Button;
	Button *_skill4Button;

	Slider *_hpSlider;
	Slider *_manaSlider;

	Label *_timeViewLabel;
	Sprite *_miniMap;
	Sprite *_selectRect;
	Sprite *_mini_Icon;

	Node *_battleBackround;

	ClippingNode *_characterImageViewNode;

	///TOUCH EVENT AND DETECT MOVE DIRECTION///////////////////////////////////////////////////////////////////////
	Vec2 _touchStartPoint;

	////PHYSICAL//////////////////////////////////////////////////////////////////////
	PhysicsWorld *_myWorld;
	virtual void savePhysicWorld(PhysicsWorld *world);

	///CHARACTER///////////////////////////////////////////////////////////////////////
	Sprite *testObject;


};


#endif