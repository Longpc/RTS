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
#include "Effect.h"

#include "cocostudio/CocoStudio.h"
#include <time.h>
#include "Server/Server.h"
#include <string.h>
#include "Server/API/BattleAPI.h"

#define MOVE_SPEED 250
#define IMAGE_SCALE 0.6f
#define ANIMETE_DELAY 0.25f
#define ATTACK_ANIMATION_DELAY 1

#define LOW 1
#define MID 2
#define HIGH 3

#define ICON 10

#define RESPAWN_DELAY 0.6f

#define ENEMY_NUM 3
#define RESTORE_MULTI 5

#define TAG_SKILL_1 1
#define TAG_SKILL_2 2
#define TAG_SKILL_3 3
#define TAG_SKILL_4 4

#define ENEMY_FLAG 1
#define ALLIED_FLAG 2

#define TAG_MP_LABEL 1
#define TAG_SKILL_AOE 111
#define SKILL_TOUCH_DELAY 1
#define SKILL_AOE_Y_SCALE 0.75f

#define DRAW_UNIT 999

#define STATUS_CHANGE_TIME 0.5f
#define STATUS_DELAY_TIME 0.2f

#define POISON 1
#define STUN 2
#define BUFF_ATTACK 3
#define BUFF_DEFENCE 4
#define DEBUFF_ATTACK 5

#define POISON_STEP_TIME 2.0f

#define FOUNTAIN_ACTION 555

#define ENEMY_RESPAW_ACTION_TAG 101
#define BUFF_STATUS_TAG 102

#define ENEMY_CONTACT_CATEGORY_BITMAP 0x00000101
#define ENEMY_CONTACT_COLLISION_BITMAP 0x00000010
#define ALLIED_CONTACT_CATEGORY_BITMAP 0x00000110
#define ALLIED_CONTACT_COLLISION_BITMAP 0x00000001

using namespace cocostudio;
class BatleScene : public LayerBase
{

public:
	BatleScene();
	~BatleScene();
	static Scene* createScene(int selectedUnitId, vector<SkillInfoNew> playerSkills);
	static BatleScene* create(int unitId, vector<SkillInfoNew> playerSkills);
	bool init(int unitId, vector<SkillInfoNew> playerSkills);
private:
	///VARIABLES///////////////////////////////////////////////////////////////////////
	struct tm day;
	time_t timer;
	struct tm * timeinfo;
	int _oldSecond = 0;
	///UI///
	Button *_menuButton;

	Button *_skill1Button;
	Button *_skill2Button;
	Button *_skill3Button;
	Button *_skill4Button;

	Label *_timeViewLabel;
	Sprite *_miniMap;
	Sprite *_selectRect;
	Sprite *_mainCharacterIconInMiniMap;

	Node *_battleBackround;

	ClippingNode *_characterImageViewNode;

	///TOUCH EVENT AND DETECT MOVE DIRECTION///
	Vec2 _touchStartPoint;
	Sprite *_touchMoveBeginSprite;
	Sprite *_touchMoveEndSprite;

	int _moveMode;
	////PHYSICAL///
	PhysicsWorld *_myWorld;

	vector<Sprite*> _allStone;

	string _moveImagePath;
	string _attackImagePath;
	Sprite *_selectTargetSprite;

	///ATTACK LOGIC///
	Sprite *_autoAttackArea;
	Sprite *_skillAOEShowSprite;
	//Sprite *_testAttackTarget;
	vector<Sprite*> _allEnemyUnitSprite;
	vector<Sprite*> _allEnemyIconInMinimap;
	vector<Slider*> _allEnemyHpBar;
	vector<UnitInforNew> _allEnemyUnitData;
	vector<int> _allEnemuUnitMaxHp;

	vector<bool> _allEnemyAttachDelay;


	int _currentAttackActionTag;
	int _currentMoveActionTag;

	int _indexOfBeAttackEnemy = -1;

	vector<SkillInfoNew> _playerSkills;
	///CHARACTER///

	int _selectedUnitId;
	UnitInforNew _mainCharacterData;
	UnitInforNew _saveMainStatusData;
	vector<SkillInfoNew> _mainCharacterSkillData;
	Sprite *testObject;
	Node *_statusContainer;

	int _mainCharacerMaxMp;

	Slider *_mainCharacterMiniHpBar;
	bool _onRespwanFlg = false;

	bool _moveDisableFlg = false;
	bool _onDelayAttackFlg = false;

	Slider *_mainCharacterHpBar;
	Label *_hpViewLabel;
	Slider *_mainCharacterMpBar;
	Label *_mpViewlabel;
	///ALLIED UNIT
	vector<UnitInforNew> _allAlliedUnitData;
	vector<Slider*> _allAlliedUnitHpBar;
	vector<Sprite*> _allAlliedUnitSprite;
	vector<int> _allAlliedUnitMaxHp;

	float _helpAttackValue = 1.0f;
	float _helpDefenceValue = 1.0f;
	float _helpHpValue = 1.0f;

	UnitInforNew _redTeamTowerData;
	UnitInforNew _blueTeamTowerData;

	int _currentPlayerTeamFlg = TEAM_FLG_BLUE;
	int _currentEnemyTeamFlg = TEAM_FLG_RED;

	//For battle result
	vector<UserBattleInfo> _blueTeamInfo;
	vector<UserBattleInfo> _redTeamInfo;

	/*All battle variables*/
	int _alliedTeamTotalDead = 0;
	int _enemyTeamTotalDead = 0;

	/*For skill area moved logic*/
	SkillInfoNew _onSelectSkillData;
	bool _showSkillTargetFlag = false;

	/*For skill status show*/
	vector<string> _skillStatusImageList;
	
	vector<vector<string>> _enemyStatusImagePath;

	/*Titled Map for Path finding and simple background*/
	TMXTiledMap* _myMap;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////

	///DATABASE///
	virtual UnitInforNew getUnitDataFromDataBase(int unitId);
	virtual vector<SkillInfoNew> getUnitSkillFromDataBase(int unitId);
	virtual vector<UnitInforNew> getEnemyUnitsData(vector<int> enemyIdList);
	///LAYOUT BASE///
	virtual void onEnter();

	///CREATE UI CONTENT + PHYSIC WORLD///
	virtual void createContent();
	virtual void displaySkillMpInButton(Button *parent, int mp);
	virtual void createPhysicBolder();
	virtual Node* createHBolder();
	virtual Node* createVBolder();
	virtual void createRandomRock();
	virtual bool onPhysicContactBegin(const PhysicsContact &contact);
	virtual bool onPhysicContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	///BUTTON CALLBACK///
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void menuButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void debugPhysicButtonCallback(Ref *pSEnder, Widget::TouchEventType type);
	virtual void changeImageButtonCallback(Ref *pSender, Widget::TouchEventType type);

	///MAIN TOUCH EVENT///
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	
	///CHARACTER MOVE LOGIC///
	virtual bool caculAvgAngle(int avg, float angle);
	virtual int detectDirectionBaseOnTouchAngle(float angle);
	
	///CHARACTER MOVE ACTION///
	virtual void actionCharacter(int directionId, Sprite *characterSprite);
	virtual void actionCharacterCopy(int directionId, Sprite *sprite);
	//virtual void selectAttackTarget();
	//virtual void runAttackAnimation();
	virtual Animation* createMoveAnimationWithDefine(int imageId, string path);
	virtual Animation* createAttackAnimationWithDefine(int imageId, string path);
	virtual void rotateCharacter(Sprite *target, int direc);

	//virtual void removeMoveDisableFlg();
	virtual void removeceAttackDelayFlg();

	/*Remove enemy attack delay flag. Default attack delay time is 1s*/
	virtual void removeEnemyAttackDelayFlg(Ref *pSender, int index);

	/*This function will be call when main character attack animation finished ( 0.5s)*/
	virtual void characterAttackCallback();
	/*This function will be called when enemy (as pSender) attack animation was finished*/
	virtual void enemyAttackCallback(Ref *pSEnder, int index);
	
	virtual void showAttackDame(int dameValue, Vec2 pos, int colorType);

	virtual void enemyDieAction(int id);

	virtual void runRespawnAction(int killerId);
	virtual void removeReSpawnFlg();

	///OPTION DIALOG CALLBACK///
	virtual void optionDecideCallback(Ref *pSEnder, Widget::TouchEventType type);
	virtual void optionCancelCallback(Ref *pSEnder, Widget::TouchEventType type);

	///MINIMAP LOGIC///
	virtual void updateMiniMap();
	virtual void checkForAutoAttack();
	void update(float delta);
	///FAKE  Z Order///
	void fakeZOrder();
	///BATLE TIME///
	void updateTime();
	string makeTimeString(int second);

	virtual void savePhysicWorld(PhysicsWorld *world);

	virtual void changeAnimationImagePathByUnitId(int unitId);
	virtual void autoRestoreHpAndMp();
	virtual void updateSlider();

	virtual void updateHpAndMpViewLabel();

	virtual void removeSkillDisableFlg(int skillnum);

	virtual void showCoolDown(Button *parentButton, int cooldownTime);

	virtual void playSkill(SkillInfoNew skillData);
	virtual vector<int> detectUnitInAoe(SkillInfoNew skill, int unitFlg, bool drawFlg = true);

	virtual void skillRestoreAction(SkillInfoNew skillInfo);
	virtual void skillRestoreAll(SkillInfoNew skillInfo);
	virtual void skillRestoreOne(SkillInfoNew skillInfo);

	virtual void skillHelpAction(SkillInfoNew skillInfo);
	virtual void skillHelpAll(SkillInfoNew skillInfo);
	virtual void skillHelpOne(SkillInfoNew skillInfo);

	virtual void skillAttackAction(SkillInfoNew skillInfo);
	virtual void skillAttackAll(SkillInfoNew skillInfo);
	virtual void skillAttackOne(SkillInfoNew skillInfo);

	virtual void skillPoisonAction(SkillInfoNew skillInfo);
	virtual void skillStunAction(SkillInfoNew skillInfo);

	virtual void poisonEffectAction(SkillInfoNew skill, int index);

	virtual void endBattle();
	virtual float caculDameRate(int mainC, int enemy);

	virtual void longPressAction(Button *pSender, SkillInfoNew skill);


	virtual void getBattleInformationFromSocketIO(int sID);
	virtual void sendInformationToServer(int sID, string data);

	virtual void demoCallbackNotUserInlineFunction(Ref *pSender, vector<int> a);
	virtual bool detectPointInTriangle(Vec2 point, vector<Vec2> points);
	virtual float makeDot(Vec2 v1, Vec2 v2);
	virtual Vec2 makePoint(Vec2 v1, Vec2 v2);

	virtual void saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg);
	virtual void saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg);

	virtual void displayUnitStatus(Sprite *parent, int statusType, SkillInfoNew skillInfo, int spIndex = 0);
	virtual Animation*  createStatusAnimation(string imagePath);

	virtual void enemyUnitAutoMoveTest();
	/*define the function to play effect and logic when player move on fountain area*/
	virtual void fountainRestoreEffect();
	virtual void enemyRespawAction(int index);


	/////////ECLIPSE ROTATE EFFECT

	virtual void createSorceryEffect(Sprite* spriteUnit, std::string eclipseFilePath);
	virtual void removeSorceryEclipse(Ref* pSender);
	virtual void removeEffect(Ref* pSender);

	virtual void pushStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void removeStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void displayStatusInTime(Sprite* parent, vector<string> allImages);

	virtual int findIndexOfString(vector<string> v, string element);

	virtual Vec2 getTitlePosForPosition(Vec2 location);
	virtual Vec2 getPositionForTitleCoord(Vec2 titleCoord);
	virtual vector<Vec2> AStarPathFindingAlgorithm(Vec2 curentPos, Vec2 destinationPos);
	virtual bool isValidTileCoord(Vec2 &titleCoord);
	virtual bool isWallAtTileCoord(Vec2 &titleCoord);
	virtual PointArray *allWalkableTitlesCoordForTitleCoord(Vec2 titleCoord);

	class ShortestPathStep : public cocos2d::Object
	{
	public:
		ShortestPathStep();
		~ShortestPathStep();

		static ShortestPathStep *createWithPosition(const Vec2 pos);
		bool initWithPosition(const Vec2 pos);

		int getFScore() const;
		bool isEqual(const ShortestPathStep *other) const;
		std::string getDescription() const;

		CC_SYNTHESIZE(Vec2, _position, Position);
		CC_SYNTHESIZE(int, _gScore, GScore);
		CC_SYNTHESIZE(int, _hScore, HScore);
		CC_SYNTHESIZE(ShortestPathStep*, _parent, Parent);
	};

	Vector<ShortestPathStep*> _spOpenSteps;
	Vector<ShortestPathStep*> _spClosedSteps;
	Vector<ShortestPathStep*> _shortestPath;

	void insertInOpenSteps(ShortestPathStep *step);
	int computeHScoreFromCoordToCoord(const Vec2 &fromCoord, const Vec2 &toCoord);
	int costToMoveFromStepToAdjacentStep(const ShortestPathStep *fromStep, const ShortestPathStep *toStep);
	ssize_t getStepIndex(const Vector<ShortestPathStep*> &steps, const ShortestPathStep *step);
	void constructPathAndStartAnimationFromStep(ShortestPathStep *step);
	void moveStepAction();

	void countTime(float dt);
	float calCulTime = 0.0f;
};


#endif