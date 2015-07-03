#ifndef LOADING_SPRITE
#define LOADING_SPRITE

#include "base/Define.h"

class LoadingSprite : public LayerColor
{
public:
	static LoadingSprite* getInstance();
	void destroyInstance();
	bool init();
	void showLoadingFail();
protected:
private:
	static LoadingSprite *loading;
};


#endif