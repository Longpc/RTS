#ifndef BATLE_SCENE
#define BATLE_SCENE
#include "base/LayerBase.h"
#include "base/Define.h"
#include "base/MyBodyParser.h"
#include "Multi/BatleResultScene/BatleResultScene.h"
#include "base/font/LabelShow.h"
#include "OptionDialog.h"
#include "dataController/UnitData/UnitData.h"
#include "dataController/SkillData/SkillData.h"

#include <time.h>

#define ENEMY_NUM 5
#define RESTORE_MULTI 5
class BatleScene : public LayerBase
{

public:
	static Scene* createScene(int selectedUnitId);
	static BatleScene* create(int unitId);
	bool init(int unitId);
private:
	///DATABASE///////////////////////////////////////////////////////////////////////
	virtual UnitInforNew getUnitDataFromDataBase(int unitId);
	virtual vector<SkillInfoNew> getUnitSkillFromDataBase(int unitId);
	virtual vector<UnitInforNew> getEnemyUnitsData(vector<int> enemyIdList);
	///LAYOUT BASE///////////////////////////////////////////////////////////////////////
	virtual void onEnter();

	///CREATE UI CONTENT + PHYSIC WORLD///////////////////////////////////////////////////////////////////////
	virtual void createContent();
	virtual Sprite* createBackground(Vec2 pos);
	virtual void createPhysicBolder();
	virtual Node* createHBolder();
	virtual Node* createVBolder();

	virtual void createRandomRock();
	vector<Sprite*> _allStone;

	virtual void onPhysicContactBegin(const PhysicsContact &contact);

	///BUTTON CALLBACK///////////////////////////////////////////////////////////////////////
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void menuButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill2ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill3ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill4ButtonCallback(Ref *pSender, Widget::TouchEventType type);
	
	virtual void debugPhysicButtonCallback(Ref *pSEnder, Widget::TouchEventType type);
	virtual void changeImageButtonCallback(Ref *pSender, Widget::TouchEventType type);

	///MAIN TOUCH EVENT///////////////////////////////////////////////////////////////////////
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	
	///CHARACTER MOVE LOGIC///////////////////////////////////////////////////////////////////////
	virtual bool caculAvgAngle(int avg, float angle);
	virtual int detectDirectionBaseOnTouchAngle(float angle);
	
	///CHARACTER MOVE ACTION///////////////////////////////////////////////////////////////////////
	virtual void actionCharacter(int directionId);
	
	virtual void selectAttackTarget();
	virtual void runAttackAnimation();
	virtual Animation* createMoveAnimationWithDefine(int imageId, string path);
	virtual Animation* createAttackAnimationWithDefine(int imageId, string path);
	virtual void rotateCharacter(Sprite *target, int direc);

	virtual void removeMoveDisableFlg();
	virtual void removeceAttackDelayFlg();

	/*Remove enemy attack delay flag. Default attack delay time is 1s*/
	virtual void removeEnemyAttackDelayFlg(Ref *pSender);

	/*This function will be call when main character attack animation finished ( 0.5s)*/
	virtual void characerAttackCallback();
	/*This function will be called when enemy (as pSender) attack animation was finished*/
	virtual void enemyAttackCallback(Ref *pSEnder);
	
	virtual void showAttackDame(int dameValue, Vec2 pos, int colorType);

	///OPTION DIALOG CALLBACK///////////////////////////////////////////////////////////////////////
	virtual void optionDecideCallback(Ref *pSEnder, Widget::TouchEventType type);
	virtual void optionCancelCallback(Ref *pSEnder, Widget::TouchEventType type);
	string _moveImagePath;
	string _attackImagePath;
	Sprite *_selectTargetSprite;

	///ATTACK LOGIC///////////////////////////////////////////////////////////////////////
	Sprite *_autoAttackArea;
	//Sprite *_testAttackTarget;
	vector<Sprite*> _alltargetUnit;
	vector<Sprite*> _allEnemyIconInMinimap;
	vector<Slider*> _allEnemyHpBar;
	vector<UnitInforNew> _allEnemyUnitData;

	int _currentAttackActionTag;
	int _currentMoveActionTag;

	int _indexOfBeAttackEnemy;

	bool _moveDisableFlg = false;
	bool _onDelayAttackFlg = false;
	vector<bool> _allEnemyAttachDelay;

	vector<int> _enemyCurentHp;
	vector<int> _enemyCurentMp;

	int _characterCurentHp;
	int _characterCurentMp;


	///MINIMAP LOGIC///////////////////////////////////////////////////////////////////////
	virtual void updateMiniMap();
	virtual void checkForAutoAttack();
	void update(float delta);
	///FAKE  Z Order///////////////////////////////////////////////////////////////////////
	void fakeZOrder();
	///BATLE TIME///
	void updateTime();
	string makeTimeString(int second);

	struct tm day;
	time_t timer;
	struct tm * timeinfo;

	///UI///////////////////////////////////////////////////////////////////////
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
	Sprite *_mini_Icon;

	Node *_battleBackround;

	ClippingNode *_characterImageViewNode;

	///TOUCH EVENT AND DETECT MOVE DIRECTION///////////////////////////////////////////////////////////////////////
	Vec2 _touchStartPoint;
	Sprite *_touchMoveBeginSprite;
	Sprite *_touchMoveEndSprite;

	int _moveMode;

	////PHYSICAL//////////////////////////////////////////////////////////////////////
	PhysicsWorld *_myWorld;
	virtual void savePhysicWorld(PhysicsWorld *world);
	///CHARACTER///////////////////////////////////////////////////////////////////////
	virtual void saveSelectedUnitId(int id);

	int _selectedUnitId;
	UnitInforNew _unitData;
	vector<SkillInfoNew> _allUnitSkillData;

	Sprite *testObject;
	Slider *_miniHpSlider;
	bool _onRespwanFlg = false;

	virtual void removeReSpawnFlg();



	virtual void changeImagePathforTest();
	virtual void autoRestoreHpAndMp();
	virtual void updateSlider();

	int _oldSecond = 0;

};


#endif