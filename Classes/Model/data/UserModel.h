#ifndef USER_MODEL_H__
#define USER_MODEL_H__

#include "cocos2d.h"
#include "base/Define.h"

USING_NS_CC;
using namespace std;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#define BUILDING_MYDLL 12
#ifdef BUILDING_MYDLL
#define MYCLASS_DECLSPEC __declspec(dllexport)
#else
#define MYCLASS_DECLSPEC __declspec(dllimport)
#endif
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
# define MYCLASS_DECLSPEC CC_DLL
#endif

class MYCLASS_DECLSPEC UserModel : public Ref
{
public:
	static UserModel *getInstance();
	CC_SYNTHESIZE(UserInfo, _userInfo, UserInfo);
	CC_SYNTHESIZE(vector<UnitInforNew>, _listUnit, UserUnitsInfo);

private:
	static UserModel *s_UserModel;
};
#endif