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
#include "Character.h"

#include "Model/data/UserModel.h"
#include "Model/data/UserSkill.h"
#include "Model/data/UserUnit.h"


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
	/*create battle scene with parameter:
	@selectedUnitId: selected unit id in unit selected scene
	@playerSkills: the vector store two player skill info
	*/
	static Scene* createScene();
	static BatleScene* create();
	bool init();
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
	vector<UserUnitInfo> _allEnemyUnitData;
	vector<int> _allEnemuUnitMaxHp;

	vector<bool> _allEnemyAttachDelay;


	int _currentAttackActionTag;
	int _currentMoveActionTag;

	int _indexOfBeAttackEnemy = -1;

	vector<UserSkillInfo> _playerSkills;
	///CHARACTER///

	int _selectedUnitId;
	UserUnitInfo _mainCharacterData;
	UserUnitInfo _saveMainStatusData;
	vector<UserSkillInfo> _mainCharacterSkillData;
	Character* testObject;
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
	vector<UserUnitInfo> _allAlliedUnitData;
	vector<Slider*> _allAlliedUnitHpBar;
	vector<Sprite*> _allAlliedUnitSprite;
	vector<int> _allAlliedUnitMaxHp;

	float _helpAttackValue = 1.0f;
	float _helpDefenceValue = 1.0f;
	float _helpHpValue = 1.0f;

	UserUnitInfo _redTeamTowerData;
	UserUnitInfo _blueTeamTowerData;

	int _currentPlayerTeamFlg = TEAM_FLG_BLUE;
	int _currentEnemyTeamFlg = TEAM_FLG_RED;

	//For battle result
	vector<UserBattleInfo> _blueTeamInfo;
	vector<UserBattleInfo> _redTeamInfo;

	/*All battle variables*/
	int _alliedTeamTotalDead = 0;
	int _enemyTeamTotalDead = 0;

	/*For skill area moved logic*/
	UserSkillInfo _onSelectSkillData;
	bool _showSkillTargetFlag = false;

	/*For skill status show*/
	vector<string> _skillStatusImageList;
	
	vector<vector<string>> _enemyStatusImagePath;

	/*Titled Map for Path finding and simple background*/
	TMXTiledMap* _myMap;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////

	///DATABASE API
	virtual UserUnitInfo getUnitDataFromDataBase(int unitId);
	virtual vector<UserSkillInfo> getUnitSkillFromDataBase(UserUnitInfo unitData);
	virtual vector<UserUnitInfo> getEnemyUnitsData(vector<int> enemyIdList);
	

	CC_SYNTHESIZE(int, _unitStatus, UnitStatus);
	virtual void sendMoveEvent(int direction);
	
	///LAYOUT BASE///
	virtual void onEnter();

	///CREATE UI CONTENT + PHYSIC WORLD///
	/*Create the content of battle scene: battle background, physical object, tree, rock, character
	 Initial data for battle logic
	*/
	virtual void createContent();

	/*Show skill mp cost value in the skill button (@parent);
	*/
	virtual void displaySkillMpInButton(Button *parent, int mp);
	/*create battle physic bolder*/
	virtual void createPhysicBolder();
	virtual Node* createHBolder();
	virtual Node* createVBolder();
	/*create rock and tree and put to battle background by random position*/
	virtual void createRandomRock();
	/*For physic contact event*/
	virtual bool onPhysicContactBegin(const PhysicsContact &contact);
	/*Pre-Solve of physic contact event*/
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
	
	///CHARACTER MOVE ACTION///
	/*This function will let the @characterSprite run move animation base on @directionID*/
	virtual void actionCharacter(int directionId, Sprite *characterSprite);
	/* copy function for enemy AUTOMOVE TEST*/
	virtual void actionCharacterCopy(int directionId, Sprite *sprite);

	/* create move animation from list off image base on @imageId as move direction offset and @path as image folder path*/
	virtual Animation* createMoveAnimationWithDefine(int imageId, string path);
	
	/* create attack animation from list off image base on @imageId as attack direction offset and @path as image folder path*/
	virtual Animation* createAttackAnimationWithDefine(int imageId, string path);
	
	/*this function will let the @target sprite run action look as it rotate base on @direc*/
	virtual void rotateCharacter(Sprite *target, int direc);

	/*This function will be call when main character attack animation finished ( 0.5s)*/
	virtual void characterAttackCallback();
	/*This function will be called when enemy (as pSender) attack animation was finished*/
	virtual void enemyAttackCallback(Ref *pSEnder, int index);
	
	/*function to display the @dameValue as the image in the @position with color base on colorType ( 1: Yellow, 2: Blue */
	virtual void showAttackDame(int dameValue, Vec2 pos, int colorType);

	virtual void enemyDieAction(int id);
	/* display dead time and save kill/dead info
	*/
	virtual void runRespawnAction(int killerId);

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

	void update(float delta);
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
	/*this function will detect all unit in skill effect area base on @skill info (skill distance, skill area type, skill type...
	return value is the vector store all unit id which contained by effect area
	@drawFlg is flag to decide this function will show or not show the skill effect area
	*/
	virtual vector<int> detectUnitInAoe(UserSkillInfo skill, int unitFlg, bool drawFlg = true);
	/* run logic and effect of heal skills
	*/
	virtual void skillRestoreAction(UserSkillInfo skillInfo);

	virtual void skillRestoreAll(UserSkillInfo skillInfo);
	virtual void skillRestoreOne(UserSkillInfo skillInfo);

	/*Run logic and effect of buff skills*/
	virtual void skillBuffAction(UserSkillInfo skillInfo);
	virtual void skillHelpAll(UserSkillInfo skillInfo);
	virtual void skillHelpOne(UserSkillInfo skillInfo);

	/*Run logic and effect of attack skills*/
	virtual void skillAttackAction(UserSkillInfo skillInfo);
	virtual void skillAttackAll(UserSkillInfo skillInfo);
	virtual void skillAttackOne(UserSkillInfo skillInfo);

	/*Run logic and effect of poison skills */
	virtual void skillPoisonAction(UserSkillInfo skillInfo);
	/*Run logic and effect of Stun skills*/
	virtual void skillStunAction(UserSkillInfo skillInfo);

	/*Calculate logic and play effect for poison skill for defined unit base on @index as the index in _allEnemyUnitData*/
	virtual void poisonEffectAction(UserSkillInfo skill, int index);

	/*End of battle logic*/
	virtual void endBattle();
	/*Calculate dame rate base on attacker and target attribute
	@Return: 1.5, 1, 0
	*/
	virtual float caculDameRate(int mainC, int enemy);
	/*run logic when use touch hand hold finger in skill button. It will show the skill effect area
	*/
	virtual void longPressAction(Button *pSender, UserSkillInfo skill);

	/* this function will calculate for detect a @point is contained in the triangle or not
	@Return true if point inside the triangle and false for else*/
	virtual bool detectPointInTriangle(Vec2 point, vector<Vec2> points);
	virtual float makeDot(Vec2 v1, Vec2 v2);
	virtual Vec2 makePoint(Vec2 v1, Vec2 v2);

	/*save dame deal from @attackUnit to @beAttackUnit as the attack target*/
	virtual void saveDameInfo(int dame, int attackUnitId, int beAttackUnitId, int teamFlg);
	
	/*save kill and dead info*/
	virtual void saveKillDeadInfo(int killerId, int deadUnitId, int teamFlg);

	/*function to show status of unit by animation. (status: buff, de-buff, poison, stun..*/
	virtual void displayUnitStatus(Sprite *parent, int statusType, UserSkillInfo skillInfo, int spIndex = 0);
	virtual Animation*  createStatusAnimation(string imagePath);

	virtual void enemyUnitAutoMoveTest();
	/*define the function to play effect and logic when player move on fountain area*/
	virtual void fountainRestoreEffect();
	virtual void enemyRespawAction(int index);


	/////////ECLIPSE ROTATE EFFECT

	virtual void createSorceryEffect(Sprite* spriteUnit, std::string eclipseFilePath);
	virtual void removeSorceryEclipse(Ref* pSender);
	virtual void removeEffect(Ref* pSender);

	/*For calculate and display effect status*/
	virtual void pushStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void removeStatusImagePath(string imagepath, vector<string> &allImages);
	virtual void displayStatusInTime(Sprite* parent, vector<string> allImages);

	/*function to return the index of string was element of he vector<string>*/
	virtual int findIndexOfString(vector<string> v, string element);

	/*get the tilecoord( title index in the titled map) by the title position*/
	virtual Vec2 getTitlePosForPosition(Vec2 location);
	
	/*get the position of the title cell*/
	virtual Vec2 getPositionForTitleCoord(Vec2 titleCoord);
	/*Logic for A start path finding function*/
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