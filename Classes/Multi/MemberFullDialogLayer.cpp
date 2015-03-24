#include "MemberFullDialogLayer.h"

bool MemberFullDialog::init()
{
	if (!LayerBase::init())
	{
		return false;
	}

	_menu->setVisible(false);
	auto background = LayerColor::create(Color4B(0, 0, 0, 150));
	addChild(background, -1);

	auto foreground = Sprite::create("image/navigator/dialog_background.png");
	foreground->setPosition(_visibleSize / 2);
	background->addChild(foreground);

	auto label = Label::createWithSystemFont("Max member. Please try again late", "fonts/Marker Felt.ttf", 30);
	label->setColor(Color3B::BLACK);
	label->setPosition(Vec2(foreground->getContentSize().width/2,foreground->getContentSize().height/2));
	foreground->addChild(label);

	auto button = MenuItemImage::create("image/button/button_ok_n.png", "image/button/button_ok_s.png", CC_CALLBACK_1(LayerBase::onBackButtonClick, this));
	button->addChild(Label::createWithSystemFont("OK", "", 25));
	button->setPosition(Vec2(_visibleSize.width/2, _visibleSize.height/2 - 100));
	
	auto menu = Menu::create(button, nullptr);
	menu->setPosition(Vec2::ZERO);

	background->addChild(menu);

	return true;
}

