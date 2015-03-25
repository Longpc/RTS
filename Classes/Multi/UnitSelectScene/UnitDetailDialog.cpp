#pragma execution_character_set("utf-8")
#include "UnitDetailDialog.h"

UnitDetailDialog* UnitDetailDialog::create(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	UnitDetailDialog *p = new UnitDetailDialog();
	if (p && p->init(unit, decideCallback, ccelCallback)) {
		p->autorelease();
		return p;
	}
	CC_SAFE_DELETE(p);
	return NULL;
}

bool UnitDetailDialog::init(UnitInforNew unit, MyTouchEvent decideCallback, MyTouchEvent ccelCallback)
{
	if (!LayerBase::init()) {
		return false;
	}
	_unitInfo = unit;
	getUnitSkillDataFromDatabase();
	_menu->setVisible(false);
	_bgImage->setVisible(false);
	_pageTitleSprite->setVisible(false);
	_usernameBg->setVisible(false);
	_decideCallback = decideCallback;
	_ccCallback = ccelCallback;
	

	//auto background = LayerColor::create(Color4B(0, 0, 0, 150));
	//addChild(background, -1);
	_backGround->setOpacity(150);
	auto foreground = Sprite::create("image/dialog/unitDetail/select_scene_detail.png");
	foreground->setPosition(_visibleSize / 2);
	addChild(foreground);

	auto closeButton = Button::create();
	closeButton->loadTextureNormal("image/button/new/button_close.png");
	closeButton->setPosition(Vec2(foreground->getContentSize().width - closeButton->getContentSize().width + 20, foreground->getContentSize().height - closeButton->getContentSize().height));
	closeButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::closeButtonCallback, this));
	foreground->addChild(closeButton,11);

	auto decideButton = Button::create();
	decideButton->loadTextureNormal("image/button/new/button_decide.png");
	decideButton->setPosition(Vec2(foreground->getContentSize().width - decideButton->getContentSize().width/2 - 15, decideButton->getContentSize().height));
	decideButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::decideButtonCallback, this));
	foreground->addChild(decideButton,11);

	displayUnitInfo(foreground);

	return true;
}

void UnitDetailDialog::decideButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_decideCallback != nullptr) {
		_decideCallback(pSender, type);
	}

}

void UnitDetailDialog::closeButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	closeDialog();
	if (_ccCallback != nullptr) {
		_ccCallback(pSender, type);
	}
}

void UnitDetailDialog::closeDialog()
{
	this->runAction(RemoveSelf::create(false));
}

void UnitDetailDialog::displayUnitInfo(Sprite *parent)

{

	auto image = Sprite::create(_unitInfo.image);
	image->setPosition(Vec2(150, parent->getContentSize().height / 2));
	parent->addChild(image, 10);
	image->setScale(2);

	string dameType = "無";
	switch (_unitInfo.attr)
	{
	case 1:
		dameType = "火";
		break;
	case 2:
		dameType = "草";
		break;
	case 3:
		dameType = "水";
	default:
		break;
	}


	std::stringstream info;
	info << "名前:   " << _unitInfo.name << "\nHP:   " << _unitInfo.hp << "\nHP 回復:   " << _unitInfo.hp_restore << "\nMP:   " << _unitInfo.mp_restore << "\n攻撃力:   " << _unitInfo.attack_dame << "\n防御力:   " << _unitInfo.defence;
	info << "\n攻撃範囲:   " << _unitInfo.attack_sight << "\n移動スピード: " << _unitInfo.move_speed << "\n属性: " << dameType << "\nタイプ: " << _unitInfo.type;
	statusLabel = Label::create(info.str().c_str(), JAPANESE_FONT_1_BOLD, 25);
	statusLabel->setColor(Color3B::BLACK);
	statusLabel->setHorizontalAlignment(TextHAlignment::LEFT);
	
	skillLabel = Node::create();
	skillLabel->setVisible(false);

	auto backGroundSize = parent->getContentSize();

	for (int i = 0; i < _allUnitSkill.size(); i++)
	{
		auto sp = Sprite::create(_allUnitSkill[i].icon);
		sp->setScale(0.7);
		Vec2 pos = Vec2(0, backGroundSize.height/2 - 100 * i - 250);
		sp->setPosition(pos);
		skillLabel->addChild(sp);
		string content = _allUnitSkill[i].name.append("\n").append(_allUnitSkill[i].effect);
		auto lb = Label::create(content.c_str(), JAPANESE_FONT_1_BOLD, 25,Size(400,100));
		lb->setColor(Color3B::BLACK);
		lb->setPosition(pos + Vec2(50, -10));
		lb->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
		skillLabel->addChild(lb);
	}

	

	if (statusLabel->getContentSize().height > backGroundSize.height-150)
	{

		auto scroll = extension::ScrollView::create();
		scroll->setViewSize(Size(backGroundSize.width / 2, 300));
		scroll->setPosition(Vec2(backGroundSize.width / 2 - 70, 30));
		scroll->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
		scroll->setDirection(extension::ScrollView::Direction::VERTICAL);
		scroll->updateInset();
		scroll->setVisible(true);

		parent->addChild(scroll);
		auto layer = Layer::create();
		layer->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		layer->setContentSize(Size(backGroundSize.width / 2, statusLabel->getContentSize().height + 50));
		layer->setPosition(Vec2(0, -layer->getContentSize().height / 2));

		auto height = layer->getContentSize().height;

		scroll->setContainer(layer);
		scroll->setContentOffset(scroll->minContainerOffset());
		statusLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		statusLabel->setPosition(Vec2(0, height));
		layer->addChild(statusLabel);
		/*skillLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
		
		
		skillLabel->setPosition(Vec2(2, height));
		layer->addChild(skillLabel); */
	}
	else {
		statusLabel->setPosition(Vec2(parent->getContentSize().width / 2 - 70, parent->getContentSize().height - 100));
		statusLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);

		parent->addChild(statusLabel);
		
	}
	

	skillLabel->setPosition(Vec2(parent->getContentSize().width / 2 - 70, parent->getContentSize().height - 100));
	skillLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
	parent->addChild(skillLabel);

	
	statusButton = Button::create();
	statusButton->loadTextureNormal("image/dialog/unitDetail/status_s.png");
	statusButton->setPosition(Vec2(parent->getContentSize().width/2 - 10,parent->getContentSize().height - 50));
	statusButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::statusButtonCallback, this));
	parent->addChild(statusButton,10);

	skillButton = Button::create();
	skillButton->loadTextureNormal("image/dialog/unitDetail/skill.png");
	skillButton->setPosition(statusButton->getPosition() + Vec2(skillButton->getContentSize().width,0));
	skillButton->addTouchEventListener(CC_CALLBACK_2(UnitDetailDialog::skillButonCallback, this));
	parent->addChild(skillButton,10);
// 	RichText *text1 = RichText::create();
// 	text1->ignoreContentAdaptWithSize(false);
// 	text1->setContentSize(Size(100, 100));
// 	RichElementText *ele1 = RichElementText::create(1, Color3B::RED, 255, "名前: \n", JAPANESE_FONT_1_BOLD, 25);
// 	RichElementText *ele2 = RichElementText::create(2, Color3B::BLACK, 255, _unitInfo.name.c_str(), JAPANESE_FONT_1_BOLD, 25);
// 
// 	text1->pushBackElement(ele1);
// 	text1->pushBackElement(ele2);
// 	parent->addChild(text1);
}

void UnitDetailDialog::statusButtonCallback(Ref *pSender, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusLabel->setVisible(true);
		skillLabel->setVisible(false);
		statusButton->loadTextureNormal("image/dialog/unitDetail/status_s.png");
		skillButton->loadTextureNormal("image/dialog/unitDetail/skill.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void UnitDetailDialog::skillButonCallback(Ref *pSEnder, Widget::TouchEventType type)
{
	switch (type)
	{
	case cocos2d::ui::Widget::TouchEventType::BEGAN:
		break;
	case cocos2d::ui::Widget::TouchEventType::MOVED:
		break;
	case cocos2d::ui::Widget::TouchEventType::ENDED:
	{
		statusLabel->setVisible(false);
		skillLabel->setVisible(true);
		statusButton->loadTextureNormal("image/dialog/unitDetail/status.png");
		skillButton->loadTextureNormal("image/dialog/unitDetail/skill_s.png");
		break;
	}
	case cocos2d::ui::Widget::TouchEventType::CANCELED:
		break;
	default:
		break;
	}
}

void UnitDetailDialog::getUnitSkillDataFromDatabase()
{
	/*string sql = "SELECT skill.* FROM unit_skill JOIN unit ON unit.id = unit_skill.unitId JOIN skill ON skill.id = unit_skill.skillId WHERE unit.id = ";
	sql.append(DataUtils::numberToString(_unitInfo.id));

#define DATAFILE "database.db3"
	sqlite3 *data = SqlUtil::openData(DATAFILE);

	vector<vector<string>> a = SqlUtil::runQuery(data, sql.c_str());
	for (auto &item : a)
	{
		SkillInfoNew temp;
		temp.id = DataUtils::stringToFloat(item[0].c_str());
		temp.name = item[1];
		temp.aoe = DataUtils::stringToFloat(item[2].c_str());
		temp.target_type = DataUtils::stringToFloat(item[3].c_str());
		temp.mp_cost = DataUtils::stringToFloat(item[4].c_str());
		temp.cooldown = DataUtils::stringToFloat(item[5].c_str());
		temp.skill_type = DataUtils::stringToFloat(item[6].c_str());
		temp.dame_type = DataUtils::stringToFloat(item[7].c_str());
		temp.dame_value = DataUtils::stringToFloat(item[8].c_str());
		temp.duration = DataUtils::stringToFloat(item[9].c_str());
		temp.effect = (item[10].c_str());
		temp.plistpath = (item[11].c_str());
		temp.icon = item[12].c_str();
		_allUnitSkill.push_back(temp);
	}
	*/
	_allUnitSkill =  SkillData::getUnitSkillsByUnitId(_unitInfo.id);
}
