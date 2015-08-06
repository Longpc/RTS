#ifndef __RTS_MOVESERVER__
#define __RTS_MOVESERVER__

#include "base/Define.h"
#include "network/SocketIO.h"

using namespace std;
using namespace cocos2d::network;
USING_NS_CC;

class MoveServer : public SocketIO::SIODelegate
{
public:
	~MoveServer();
	static void createInstace();
	static void destroyInstance();
	void freeClient() {
		_client = nullptr;
	}
	static MoveServer* getInstance() {
		return s_shareMoveServer;
	};
	bool init();
	SIOClient* getClient() {
		return _client;
	}
protected:
private:
	static MoveServer* s_shareMoveServer;
	SIOClient* _client;
	virtual void onConnect(SIOClient* client);
	virtual void onClose(SIOClient* client);
	virtual void onError(SIOClient* client, const std::string& data);
	virtual void onMessage(SIOClient* client, const std::string& data);
};

#endif