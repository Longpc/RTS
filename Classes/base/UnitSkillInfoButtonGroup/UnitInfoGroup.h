#ifndef __RTS_BUTTONGROUP__
#define __RTS_BUTTONGROUP__

#include "base/Define.h"
#include "base/baseButton/ClippingButtonBase.h"

typedef std::function<void()> MyCallback;
class UnitInfoGroup : public Node
{
public:
	static UnitInfoGroup* create();
	bool init();
	ClipingButtonBase* getUnitButton();
	ClipingButtonBase* getSkillButon(int index);
	Button* getUnitClickableButton();
	Button* getSkillClickableButton(int index);
	void setSelected(bool selectedFlg);
	void addUnitButtonClickCallback(MyCallback cb) {
		_unitTouchCallback = cb;
	};
	void addSkill1ButtonCLickCallback(MyCallback cb) {
		_skill1TouchCallback = cb;
	};
	void addSkill2ButonClickCallback(MyCallback cb) {
		_skill2TouchCallback = cb;
	};
	void setPlayerNameLabel(string name);
	void setUnitIcon(string imgPath);
	void setSkillIcon(int buttonIndex, string imgPath);
	void showReadyForBattle(bool showFlg);

	void resetDefaultStatus();
protected:
private:
	ClipingButtonBase *_unitButton;
	ClipingButtonBase *_playerSkill1Button;
	ClipingButtonBase *_playerSkill2Button;
	Label *_playerNameLabel;

	virtual void onTouchUnitButton(Ref *p, Widget::TouchEventType type);
	virtual void onTouchSkill1Button(Ref *p, Widget::TouchEventType type);
	virtual void onTouchSkill2Button(Ref *p, Widget::TouchEventType type);
	MyCallback _unitTouchCallback;
	MyCallback _skill1TouchCallback;
	MyCallback _skill2TouchCallback;
};



#endif