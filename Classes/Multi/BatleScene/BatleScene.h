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
	virtual void onEnter();
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void createContent();
	virtual void menuButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);

	void update(float delta);
	void updateTime();

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

	ClippingNode *_characterImageViewNode;





};


#endif