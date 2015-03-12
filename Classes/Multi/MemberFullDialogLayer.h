#ifndef MEMBER_FULL_DIALOG
#define MEMBER_FULL_DIALOG
///Include scene header here///
#include "base/LayerBase.h"
#include "ModeSelectScene/ModeSelectScene.h"
#include "Multi/TeamSelectScene/MultiTeamSelectScene.h"

///Include library header here///
#include <string.h>

class MemberFullDialog :public LayerBase
{
public:
	CREATE_FUNC(MemberFullDialog);
	bool init();
private:

};


#endif