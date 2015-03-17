#ifndef _LABEL_SHOW_H_
#define _LABEL_SHOW_H_

#include "cocos2d.h"

USING_NS_CC;


class LabelShow : public cocos2d::Sprite
{
public:
	static LabelShow* createLabel(int value, int typeFont);
	bool init(int value, int typeFont);
	void onEnter();

private:

	

protected:
	enum typeFont
	{
		F_Yellow = 1,
		F_Green = 2
	};

	int _value;
	int _typeFont;
};

#endif /* _LABEL_SHOW_H_ */