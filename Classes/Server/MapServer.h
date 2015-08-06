#ifndef __RTS_MAPSERVER__
#define __RTS_MAPSERVER__


#include <string>
#include "base/Define.h"
#include "network/SocketIO.h"

using namespace  cocos2d::network;
using namespace std;
USING_NS_CC;
class MapServer : public SocketIO::SIODelegate
{
public:
	~MapServer();
	static void createInstance();
	static MapServer* getInstance() {
		return s_sharedMapServer;
	};
	static void destroyInstace();
	bool init();
	SIOClient* getClient() {
		return _client;
	};
	void freeClient() {
		_client = nullptr;
	};
protected:
private:
	static MapServer *s_sharedMapServer;
	SIOClient* _client;
	virtual void onConnect(SIOClient* client);
	virtual void onClose(SIOClient* client);
	virtual void onError(SIOClient* client, const std::string& data);
	virtual void onMessage(SIOClient* client, const std::string& data);
};
#endif