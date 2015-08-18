#ifndef SKILLSHOWBAR_H__
#define SKILLSHOWBAR_H__
#include "cocos2d.h"
#include "base/Define.h"
#include "Model/data/UserUnitModel.h"

#define SHOWTIME 3.0f

enum ItemType
{
	SKILL_INFO = 1,
	KILL_INFO = 2,
	NEUTRAL_INFO = 3
};

enum NEUTRAL_TYPE
{
	CANNON = 1,
	TOWER = 2,
	UNIT = 3
};
USING_NS_CC;
using namespace std;

class InfoPanelItem : public Node
{
public:
	InfoPanelItem();
	~InfoPanelItem();
	/*Create Infor panel item with param:
	*
	
	*/
	static InfoPanelItem* createItem(int teamId);
	bool init(int teamId);
	void onEnter();
	void selfRemove(bool cleanFlg);

	void setItemType(int type);

	void setPrimaryUnitIcon(string path);
	void setSecondUnitIcon(string path);

	void setNeutralType(int type);

	void setLabelContent(string content);
	Label* getLabel() {
		return _mainLabel;
	};
protected:
private:
	Label* _mainLabel;
};

#endif