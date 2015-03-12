#ifndef BATLE_SCENE
#define BATLE_SCENE
#include "base/LayerBase.h"
#include "base/Define.h"
#include "Multi/BatleResultScene/BatleResultScene.h"

class BatleScene : public LayerBase
{

public:
	static Scene* createScene();
	CREATE_FUNC(BatleScene);
	bool init();
private:
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
};


#endif