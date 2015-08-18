#pragma execution_character_set("utf-8")
#include "InfoPanel.h"

InfoPanel::InfoPanel()
{

}

InfoPanel::~InfoPanel()
{

}

InfoPanel* InfoPanel::createPanel()
{
	auto panel = new InfoPanel();
	if (panel) {
		panel->init();
		panel->autorelease();
		return panel;
	}
	CC_SAFE_DELETE(panel);
	return panel;
}

bool InfoPanel::init()
{
	if (!Node::init())
	{
		return false;
	}
	setMaxItemNum(5);

	return true;
}

void InfoPanel::addSkillInfoItem(int unitId, string skillName, int teamFlg)
{
	auto item = InfoPanelItem::createItem(teamFlg);
	item->setItemType(ItemType::SKILL_INFO);
	item->setPrimaryUnitIcon(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(unitId));
	item->setLabelContent(skillName);
	addItem(item);
}

void InfoPanel::addKillInfoItem(int killer_mst_unit_id, int dead_mst_unit_id, int teamFlg) 
{
	auto item = InfoPanelItem::createItem(teamFlg);
	item->setItemType(ItemType::KILL_INFO);
	item->setPrimaryUnitIcon(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(killer_mst_unit_id));
	item->setSecondUnitIcon(UserUnitModel::getInstance()->getUnitImageByMstUnitItD(dead_mst_unit_id));
	item->setLabelContent(" が　　 をキルした");
	//item->getLabel()->setString(" が　　 をキルした");
	addItem(item);
}

void InfoPanel::addNeutralInfoItem(int teamID, int neutralType) 
{
	auto item = InfoPanelItem::createItem(teamID);
	item->setItemType(ItemType::NEUTRAL_INFO);
	item->setNeutralType(neutralType);
	if (teamID == TEAM_FLG_BLUE)
	{
		item->setLabelContent("青チームが　　を獲得した");
		//item->getLabel()->setString("青チームが　　を獲得した");
	}
	else
	{
		item->setLabelContent("赤チームが　　を獲得した");
		//item->getLabel()->setString("赤チームが　　を獲得した");
	}
	addItem(item);
}

void InfoPanel::addItem(InfoPanelItem* item) {
	this->addChild(item);
	item->setTag(123);
	item->runAction(Sequence::create(DelayTime::create(SHOWTIME), CallFuncN::create([&](Ref *p) {
		auto i = (InfoPanelItem*)p;
		i->selfRemove(true);
		_allItem.clear();
		auto list = this->getChildren();
		for (auto &item : list) {
			if (item->getTag() == 123) {
				auto a = (InfoPanelItem*)item;
				_allItem.push_back(a);
			}
		}
		updateContent();
	}), nullptr));
	_allItem.push_back(item);
	updateContent();
}

void InfoPanel::updateContent()
{
	if (_allItem.size() > getMaxItemNum()) {
		auto shiftItem = _allItem.front();
		_allItem.erase(_allItem.begin());
		shiftItem->selfRemove(true);
	}

	for (int i = 1; i <= _allItem.size(); i ++)
	{
		_allItem[_allItem.size() -i]->setPosition(Vec2(0, (1-i) * 55));
	}
}
