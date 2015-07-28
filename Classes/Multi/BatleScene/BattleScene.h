#ifndef BATLE_SCENE
#define BATLE_SCENE
#include "base/LayerBase.h"
#include "base/Define.h"
#include "base/MyBodyParser.h"
#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/BatleResultScene/BatleResultScene.h"
#include "base/font/LabelShow.h"
#include "OptionDialog.h"
#include "dataController/UnitData/UnitData.h"
#include "dataController/SkillData/SkillData.h"
#include "Effect.h"
#include "Character.h"
#include "Cannon.h"

#include "Model/data/UserModel.h"
#include "Model/data/UserSkillModel.h"
#include "Model/data/UserUnitModel.h"

#include "Model/data/BattleModel.h"

#include "cocostudio/CocoStudio.h"
#include <time.h>
#include "Server/Server.h"
#include <string.h>

#include "Server/API/BattleAPI.h"
#include "Tower.h"

#include "SummonPet.h"
#include "Map.h"

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

#define REDTEAM_CONTACT_CATEGORY_BITMAP 0x00000101
#define REDTEAM_CONTACT_COLLISION_BITMAP 0x00000010
#define BLUETEAM_CONTACT_CATEGORY_BITMAP 0x00000110
#define BLUETEAM_CONTACT_COLLISION_BITMAP 0x00000001

#define TOWER_TAG 222

#define HPBAR_TAG 1232

#define MINION_PARENT_TAG 21241
#define MINIONS_TAG 2176

#define BOUND_BORDER_TAG 1905
#define WORMHOLDROTATE 1245

#define NEUTRAL_MOVE_SPEED 150

#define FOLLOW_MARGIN 150

#define TRAP_CHECK_ACTION_TAG 2212
#define TRAP_DAME_ACTION_TAG 2122

#define MINION_PARENT_ACTION_TAG 2787
using namespace cocostudio;
class BattleScene : public LayerBase
{

public:
	BattleScene();
	~BattleScene();
	/*create battle scene with parameter:
	@selectedUnitId: selected unit id in unit selected scene
	@playerSkills: the vector store two player skill info
	*/
	static Scene* createScene();
	static BattleScene* create();
	bool init();
	int _gameMode;
private:
	/************************************************************************/
	/* VARIABLES                                                            */
	/************************************************************************/
	bool _onDestructCalled = false;
	bool _stopVelocFlg = false;
	Size _fakeVisibleSize;

	string _enemyTeamHpBarImg = "image/screen/battle/red_mini_hp_gauge.png";
	string _curentTeamHpBarImg = "image/screen/battle/mini_hp_gauge.png";


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
	Sprite *_selectRect;
	Sprite *_mainCharacterIconInMiniMap;

	Node *_battleBackground;

	ClippingNode *_characterImageViewNode;

	///TOUCH EVENT AND DETECT MOVE DIRECTION///
	Vec2 _touchStartPoint;
	Sprite *_touchMoveBeginSprite;
	Sprite *_touchMoveEndSprite;

	Sprite *_cannonTargetSprite;

	int _screenMode = 0;
	int _moveMode;
	////PHYSICAL///
	PhysicsWorld *_myWorld;

	vector<Sprite*> _allStone;

	string _moveImagePath;
	string _attackImagePath;
	Sprite *_selectTargetSprite;

	Sprite *_topMenuSprite;
	Node *_skillButtonParentnode;
	Label *_blueTeamTitleNumLabel;
	Label *_redTeamTitleNumLabel;

	///ATTACK LOGIC///
	//Sprite *_autoAttackArea;
	//Sprite *_skillAOEShowSprite;
	//Sprite *_testAttackTarget;
	vector<Sprite*> _allEnemyUnitSprite;
	vector<Sprite*> _allEnemyIconInMinimap;
	vector<Slider*> _allEnemyHpBar;
	vector<UserUnitInfo> _allEnemyUnitData;

	vector<int> _allEnemuUnitMaxHp;

	vector<bool> _allEnemyAttachDelay;


	int _currentAttackActionTag;
	int _currentMoveActionTag;

	int _indexOfBeAttackEnemy = -1;

	vector<UserSkillInfo> _playerSkills;
	///CHARACTER///

	int _selectedUnitId;
// 	UserUnitInfo _mainCharacterData;
	UserUnitInfo _saveMainStatusData;
	vector<UserSkillInfo> _mainCharacterSkillData;
	Character* testObject;

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
	vector<UserUnitInfo> _allAlliedUnitData;
	vector<Slider*> _allAlliedUnitHpBar;
	vector<Sprite*> _allAlliedUnitSprite;
	vector<int> _allAlliedUnitMaxHp;
	vector<Sprite*> _allAlliedUnitIconInMiniMap;

	float _helpAttackValue = 1.0f;
	float _helpDefenceValue = 1.0f;
	float _helpHpValue = 1.0f;

	UserUnitInfo _redTeamTowerData;
	UserUnitInfo _blueTeamTowerData;

	int _currentPlayerTeamFlg = TEAM_FLG_BLUE;
	int _currentEnemyTeamFlg = TEAM_FLG_RED;

	////For battle result
	//vector<UserBattleInfo> _saveBattleInfoAlliedTeam;
	//vector<UserBattleInfo> _saveBattleInfoEnemyTeam;

	/*All battle variables*/
	int _alliedTeamTotalDead = 0;
	int _enemyTeamTotalDead = 0;

	/*For skill area moved logic*/
	UserSkillInfo _onSelectSkillData;
	bool _showSkillTargetFlag = false;

	/*For skill status show*/
	vector<string> _skillStatusImageList;
	
	vector<vector<string>> _enemyStatusImagePath;
	vector<vector<string>> _alliedStatusImagePath;

	/*Titled Map for Path finding and simple background*/
	MyMap* _myMap;
	TMXLayer* _mapLayer;
	TMXLayer* _blockLayer;

	MyMap* _miniTMXMap;
	TMXLayer* _miniLayer;
	bool _onZoomMiniMap = false;

	Sprite* _oldTitle = nullptr;

	/*Test neutral tower*/
	vector<Tower*> _neutralTowerList;

	vector<Sprite*> _wormHoleList;


	vector<Character*> _neutralUnitList;

	vector<Cannon*> _cannonList;
	CC_SYNTHESIZE(bool, _cannonFlg, CannonFlg);

	vector<Sprite*> _neutralUnitIconInMiniMap;

	Node *redTowerParentNode;
	Node *blueTowerParentNode;
	/************************************************************************/
	/* FUNCTIONS                                                            */
	/************************************************************************/

	///DATABASE API
	virtual UserUnitInfo getUnitDataFromDataBase(int unitId);
	virtual vector<UserSkillInfo> getUnitSkillFromDataBase(UserUnitInfo unitData);
	virtual vector<UserUnitInfo> getUnitsDataListByUnitIdIdList(vector<Room_User_Unit_Model> enemyIdList);
	
	//Set Unit Status with value: 1:通常　2:死亡中　3:帰陣中　
	CC_SYNTHESIZE(int, _unitStatus, UnitStatus);
	virtual void sendMoveBeginEvent(float angle);
	virtual void sendMoveEndEvent();
	
	///LAYOUT BASE///
	virtual void onEnter();

	///CREATE UI CONTENT + PHYSIC WORLD///
	/*Create the content of battle scene: battle background, physical object, tree, rock, character
	 Initial data for battle logic
	*/
	virtual void createContent();

	virtual void changeScreenOrient();

	virtual void createNodeSVHandler();

	bool _onReconnect = false;
	virtual void reconnectToNodeServer(Ref *p);

	/*create wormhole UI*/
	virtual void createWormHole();

	/*Create map bounding wall*/
	virtual void createBoudingWall();

	/*Create neutral tower*/
	virtual void createNeutralTower();

	/*Create meutral unit*/
	virtual void createNeutralUnit();

	/*create Cannon */
	virtual void createCannon(int lisSize);


	/*Show skill mp cost value in the skill button (@parent);
	*/
	virtual void displaySkillMpInButton(Button *parent, int mp);
	/*create battle physic bolder*/
	virtual void createPhysicBolder();

	virtual Sprite* createCornerBolder(float angle);
	/*create rock and tree and put to battle background by random position*/
	virtual void createRandomRock();
	/*For physic contact event*/
	virtual bool onPhysicContactBegin(const PhysicsContact &contact);
	/*Pre-Solve of physic contact event*/
	virtual bool onPhysicContactPreSolve(PhysicsContact& contact, PhysicsContactPreSolve& solve);
	
	virtual void onPhysicContactPostSlove(PhysicsContact& contact, const PhysicsContactPostSolve& solve);
	
	virtual void contactWithWall();

	virtual void contactWithTower();

	///BUTTON CALLBACK///
	bool _touchEndButton = false;

	int _testBirdIndex = 0;
	bool _birdMode = false;
	virtual void birdButtonCallback(Ref *p, Widget::TouchEventType type);
	virtual void nextButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void menuButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void skill1ButtonCallback(Ref *pSender, Widget::TouchEventType type);

	virtual void debugPhysicButtonCallback(Ref *pSEnder, Widget::TouchEventType type);
	
	bool _isCheckMapCallback = true;
	virtual void checkMapTestButtonClick(Ref *pSender, Widget::TouchEventType type);

	///MAIN TOUCH EVENT///
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	
	///CHARACTER MOVE LOGIC///
	/* check the angle between the avg value  +- 22
	*/
	virtual bool caculAvgAngle(int avg, float angle);
	/*this function calculate the direction offset base on angle from -90 to 270
	@return: 
	7--8--9
	4-YOU-6
	1--2--3
	*/
	virtual int detectDirectionBaseOnTouchAngle(float angle);

	/*This function will be call when main character attack animation finished ( 0.5s)*/
	virtual void characterAttackCallback(int i, int dame);
	/*This function will be called when attack delay 1s ended*/
	virtual void oneSecondAttackCallback();
	
	/*This function will be called when enemy (as pSender) attack animation was finished*/
	virtual void enemyAttackCallback(Ref *pSEnder, int index);
	
	/*function to display the @dameValue as the image in the @position with color base on colorType ( 1: Yellow, 2: Blue */
	virtual void showAttackDame(int dameValue, Vec2 pos, int colorType);

	/**/
	virtual void unitDieAction(vector<Sprite*> unitSprite, vector<UserUnitInfo>* processUnitList, string uuid);

	/*Battle RT Unit respwan action*/
	virtual void unitRespwanAction(Ref* pSender, Sprite* unitSprite, vector<UserUnitInfo>* processUnitList, int index);

	/**/
	virtual void enemyDieAction(int id);
	/* display dead time and save kill/dead info
	*/
	virtual void runRespawnAction(string killerUuid);

	virtual void removeRespawnFlg();
	///OPTION DIALOG CALLBACK///
	virtual void optionDecideCallback(Ref *pSEnder, Widget::TouchEventType type);
	virtual void optionCancelCallback(Ref *pSEnder, Widget::TouchEventType type);

	///MINIMAP LOGIC///
	/*this function was defined for update the position of unit mini icon in the mini-map*/
	virtual void updateMiniMap();
	/* Function to check the condition and run attack animation by automatic
	*/
	virtual void checkForAutoAttack();
	virtual void attackLogicAndAnimation(Vec2 posDistan, int direc, int i, int dame);

	/*Checker for using team cannon*/
	virtual void checkForUsingCannon();

	virtual void readyForLunch(Cannon * cannon, int cannonIndex);
	
	virtual void lunchObject(Touch *touch);
	virtual void lunchObjectAction(Character* object, Vec2 lunchVector, int objectMoveSpeed);
	
	bool _onWarping = false;
	/*Checker for warp by move into wormhole*/
	virtual void checkForWarp();

	virtual void closeWormHole(int ingate, int outgate);
	virtual void openWormHole(int ingate, int outgate);
	
	virtual void characterAttackAnimationLogic(Vec2 distan, int direc, AttackCallback callback = nullptr);
	/*neutral type: 1 tower, 2 unit*/
	virtual void chatacterAttackNeutralCallback(int neutralIndex, int neutralType);

	/*character attack cannon calback*/
	virtual void characterAttackCannonCallback(int cannonIndex);

	virtual void neutralUnitStatusChange(Character* unit, int team, int index);

	int _onLunchCannonIndex;

	virtual void cannonStatusChange(Cannon* cannon, int teamId);

	/*check auto atack of tower*/
	bool _alliedTowerAttackdelay = false;
	bool _enemyTowerAttackdelay = false;
	virtual void checkAutoAttackOfTower();

	/*Tower attack animation*/
	virtual void towerAttackLogic(Sprite* towerSprite, UserUnitInfo towerData, vector<Sprite*> targetFindList, vector<UserUnitInfo>* unitDataList, string targetUuid, float randomNum);
	
	virtual void towerAttackCallback(Ref *p, UserUnitInfo towerData, Sprite* target, vector<UserUnitInfo>* unitDataList, int targetIndex, float randomNum);
	
	void update(float delta);
	float _checkTime = 0; //for test Battle sync
	Vec2 _checkPos = Vec2::ZERO;
	void neutralUnitMoveInSoloMod();

	void testMoveLogic(Sprite* object, int teamFLg);

	/*Change tower nearly title color with tower title*/
	virtual void changeTitlesNearObject(Sprite* object, int color, int offset);

	//TODO
	///FAKE  Z Order///
	void fakeZOrder();
	///BATLE TIME///
	/*calculate battle time and display*/
	void updateTime();
	/*create string with time format: hh:mm:ss base on @second. ex: 60s -> 00:01:00*/
	string makeTimeString(int second);

	/*function to save the physic world of the scene*/
	virtual void savePhysicWorld(PhysicsWorld *world);

	/*this function will change animation image path by unit id*/
	virtual void changeAnimationImagePathByUnitId(int unitId);

	/*
	************************************************************
	* REAL TIME FUNCTIONS BLOCK
	************************************************************
	*/
	virtual void rt_attackAnimationandLogic(Document& doc, vector<Sprite*> processUnitSprite, vector<UserUnitInfo>* processUnitData, vector<Sprite*> targetSpriteList, vector<UserUnitInfo>* targetDataList);

	/*this function will call every 5s to restore all unit hp and mp
	*/
	virtual void autoRestoreHpAndMp();
	/* update hp, mp status bar of all unit*/
	virtual void updateSlider();

	/*update the number of main character HP and MP show Label
	*/
	virtual void updateHpAndMpViewLabel();

	virtual void showCoolDownTime(Button *parentButton, int cooldownTime);

	virtual void playSkill(UserSkillInfo skillData);
	virtual void playSkillLogicAndAnimation(Sprite* object, UserSkillInfo skill, int team_id, float randNum, string uuid, int saveIndex, UserUnitInfo unit, float angle);
	/*this function will detect all unit in skill effect area base on @skill info (skill distance, skill area type, skill type...
	return value is the vector store all unit id which contained by effect area
	@drawFlg is flag to decide this function will show or not show the skill effect area
	*/
	virtual vector<int> detectUnitInAoe(Sprite* mainObj, UserSkillInfo skill, vector<Sprite*> targetList, float angle, bool drawFlg = true);
	
	virtual void drawCakePieSkillAOe(Character* object, UserSkillInfo skill, float angle);


	/* run logic and effect of heal skills
	*/
	virtual void skillRestoreAction(Sprite* object,  UserSkillInfo skillInfo, int targetUnitIndex, int teamId);

	virtual void skillRestoreAll(Sprite* object,  UserSkillInfo skillInfo, int teamId);
	/*restore all action logic*/
	virtual void restoreAllActionLogic(Sprite* object, UserSkillInfo skillInfo, vector<UserUnitInfo>& targetUnitDataList, vector<Sprite*> targetUnitList);
	
	virtual void skillRestoreOne(Sprite* object, UserUnitInfo* unitData, UserSkillInfo skillInfo, int teamId);
	/*Help effect with object*/
	virtual void healEffectWithObject(Sprite* obj);

	/*Run logic and effect of buff skills*/
	virtual void skillBuffAction(Sprite* object, UserUnitInfo* unit, UserSkillInfo skillInfo, int teamId, int saveIndex);
	virtual void skillHelpAll(Sprite* object, UserSkillInfo skillInfo, int teamId);
	virtual void skillHelpOne(Sprite* object, UserUnitInfo* unit, UserSkillInfo skillInfo, int teamId, int saveIndex);

	virtual void skillTrapAction(Sprite* object, UserSkillInfo skill, int teamId);

	virtual void trapSkillChecker(Ref* p,Sprite* object, Vec2 basePos, UserSkillInfo skill, vector<UserUnitInfo>* targetUnitList, vector<Sprite*> targetSpriteList);

	virtual void trapSkillCallback(Ref * p, int index, int dame, Sprite* object, Sprite* target, vector<UserUnitInfo>* effectUnitlist);

	virtual void skillSummonAction(Sprite* object, UserSkillInfo skill, UserUnitInfo* unitData);

	virtual void skillPetAction(Sprite* casterSprite, UserSkillInfo skill, UserUnitInfo* casterInfo, int teamFlg);


	/*Run logic and effect of attack skills*/
	virtual void skillAttackAction(Sprite* object, UserSkillInfo skillInfo, UserUnitInfo attacker, int teamId, float randNum, float angle);
	virtual void skillAttackAll(Sprite* object, UserSkillInfo skillInfo, UserUnitInfo attacker, int flg, float randNum, float angle, vector<Sprite*> &effectUnitSpriteList, vector<UserUnitInfo>* effectUnitlist);	
	virtual void showDameAndSkillLogic(Ref *p, int index, int dame, Sprite* object, Sprite* target, vector<UserUnitInfo>* effectUnitlist);
	
	virtual void skillAttackOne(Sprite* object, UserSkillInfo skillInfo, UserUnitInfo attacker, int teamId, float randNum);

	/*Run logic and effect of poison skills */
	virtual void skillPoisonAction(Sprite* object, UserSkillInfo skillInfo, int teamId, const string casterUuid);
		
	/*Run logic and effect of Stun skills*/
	virtual void skillStunAction(Sprite* object, UserSkillInfo skillInfo, int teamId);

	virtual void stunEffecAction(Sprite* object, UserSkillInfo skill, int index, vector<UserUnitInfo>* effectUnitDataList);


	/*Calculate logic and play effect for poison skill for defined unit base on @index as the index in _allEnemyUnitData*/
	virtual void poisonEffectAction(Sprite* object, UserSkillInfo skill, vector<UserUnitInfo>* unitList, vector<Sprite*> targetSprite, int teamID , const string casterUuid);


	/*End of battle logic*/
	virtual void endBattle(int winteamId);
	/*Calculate dame rate base on attacker and target attribute
	@Return: 1.5, 1, 0
	*/
	virtual float caculDameRate(int mainC, int enemy);
	/*run logic when use touch hand hold finger in skill button. It will show the skill effect area
	*/
	virtual void longPressAction(Button *pSender, UserSkillInfo skill);

	/*detect unit In fade rectangle. This mean unit in rectangle in front of skill caster*/
	virtual bool detectUnitInFadeRectangle(Character* object, Sprite* target, UserSkillInfo skill, float angle);

	/*detect unit in the round triangle*/
	virtual bool detectUnitInRoundTriangle(Character* object, Sprite* targetSprite, UserSkillInfo skill, float angle);
	/* this function will calculate for detect a @point is contained in the triangle or not
	@Return true if point inside the triangle and false for else*/
	virtual bool detectPointInTriangle(Vec2 point, vector<Vec2> points, Sprite* object);
	virtual float makeDot(Vec2 v1, Vec2 v2);
	virtual Vec2 makePoint(Vec2 v1, Vec2 v2);

	/*save dame deal from @attackUnit to @beAttackUnit as the attack target*/
	virtual void saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg);
	
	/*send dame deal information to server for sync battle information*/
	virtual void sendDameDeal(int dame, int targetID, SocketIOCallback callback);

	/*save kill and dead info*/
	virtual void saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg);

	/*Send kill dead information to server for sync*/
	virtual void sendKillDead(string killerUUid, string deadUnitUuid, Vec2 deadTitleCoord, SocketIOCallback callback);

	/*function to show status of unit by animation. (status: buff, poison, stun..*/
	virtual void displayUnitStatus(Sprite *parent, int statusType, UserSkillInfo skillInfo, int spIndex, vector<vector<string>>* targetImageStatus);
	
	/*for remove skill status image when skill effect duration end*/
	virtual void removeStatus(Ref *p, string imagepath, vector<vector<string>>* targetStatus, int index);
	
	virtual Animation*  createStatusAnimation(string imagePath);

	/*define the function to play effect and logic when player move on fountain area*/
	virtual void fountainRestoreEffect(Sprite *object, vector<UserUnitInfo>*  unitList, int index);
	virtual void enemyRespawAction(int index);


	/////////ECLIPSE ROTATE EFFECT

	virtual void createSorceryEffect(Sprite* spriteUnit, std::string eclipseFilePath);
	/*For calculate and display effect status*/
	virtual void pushStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void removeStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void displayStatusInTime(Sprite* parent, vector<string> allImages);

	/*function to return the index of string was element of he vector<string>*/
	virtual int findIndexOfString(vector<string> v, string element);

	/*get the tilecoord( title index in the titled map) by the title position*/
	//virtual Vec2 getTitleCoorForPosition(Vec2 location);
	
	/*get the position of the title cell*/
	//virtual Vec2 getPositionForTitleCoord(Vec2 titleCoord);
	/*Logic for A start path finding function*/
	virtual vector<Vec2> AStarPathFindingAlgorithm(Vec2 curentPos, Vec2 destinationPos);
	virtual bool isValidTileCoord(Vec2 &titleCoord);
	virtual bool isWallAtTileCoord(Vec2 &titleCoord);
	virtual PointArray *allWalkableTitlesCoordForTitleCoord(Vec2 titleCoord);

	/************************************************************************/
	/* CLASS USING FOR DETECT SHORTEST PATH BY A* ALGORITHM                 */
	/************************************************************************/
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

	///////////////////////////////////////////////////////////
	// CREATE MINI MOVE CONTROL
	///////////////////////////////////////////////////////////
	int _circleType;
	int _circleProperty;

	Sprite *_miniCircle;
	float circleX = 0.0f;
	float circleY = 0.0f;

	//Sprite *_miniUnit = nullptr;
	Character *_miniUnit = nullptr;

	bool _checkMiniCircleFlg = false;
	bool _checkOneTapMoveFlg = false;
	bool _checkLongTapMoveFlg = false;

	#define STOP_ACTION_ATTACK 11
	#define STOP_AUTO_MOVE 22
	#define STOP_LONG_MOVE 33

	Sprite* createMiniMoveCircle();
	void createMiniControlUnit(int circleType);
};


#endif