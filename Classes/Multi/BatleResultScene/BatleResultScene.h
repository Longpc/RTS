#ifndef BATLE_SCENE_RESULT
#define BATLE_SCENE_RESULT
#include "base/LayerBase.h"
#include "base/Define.h"
#include "ModeSelectScene/ModeSelectScene.h"
class BatleResultScene : public LayerBase
{

public:
	static Scene* createScene();
	CREATE_FUNC(BatleResultScene);
	bool init();
private:
	virtual void createContent();
	ClippingNode* createSlot(Vec2 position);
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void statusButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skillButonCallback(Ref *pSEnder, Widget::TouchEventType type);
	
	
	Sprite *statusTab;
	Sprite *skillTab;
	Button *statusButton;
	Button *skillButton;
	ClippingNode *_slot1;
	ClippingNode *_slot2;
	ClippingNode *_slot3;
};


#endif