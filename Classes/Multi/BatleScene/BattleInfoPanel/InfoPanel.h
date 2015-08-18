#ifndef SKILLPANEL_H__
#define SKILLPANEL_H__

#include "base/Define.h"
#include "InfoPanelItem.h"


struct DisplayItemInfo
{
	int unitId;
	string skillName;
	int teamFlg;
};
/************************************************************************/
/* THIS CLASS IS TEST VERSION. IF HAVE TIME> PLEASE USE RICKTEXT AS ITEM*/
/************************************************************************/
class InfoPanel : public Node
{
public:
	InfoPanel();
	~InfoPanel();
	static InfoPanel* createPanel();
	bool init();
	CC_SYNTHESIZE(int, _maxItemNum, MaxItemNum);
	/*Add new skill info to display in panel
	@param: unitId
	@param: skillName
	@param: teamFlg
	*/
	virtual void addSkillInfoItem(int unitId, string skillName, int teamFlg);
	/*
	*Add kill info item in panel
	@param: int killer_mst_unit_id
	@param: int dead_mst_unit_id
	@param: int teamFlg
	*/
	virtual void addKillInfoItem(int killer_mst_unit_id, int dead_mst_unit_id, int teamFlg);

	/*
	Add neutral unit change status 
	@param: int teamId
	@param: int neutralType: 1-Cannon, 2-Tower, 3-Unit
	*/
	virtual void addNeutralInfoItem(int teamID, int neutralType);

protected:
private:
	void addItem(InfoPanelItem* item);
	void updateContent();
	vector<InfoPanelItem*> _allItem;
};


#endif