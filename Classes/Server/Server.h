#ifndef __CCTestServer_H__
#define __CCTestServer_H__

#include <string>
#include "cocos2d.h"
#include "network/SocketIO.h"

#include "base/Define.h"
USING_NS_CC;
using namespace cocos2d::network;
using namespace std;

typedef std::function<void()> SVCallback;
typedef std::function<void(SIOClient* client, const std::string data)> SocketIOCallback;

class  NodeServer : public SocketIO::SIODelegate
{
public:

	/** returns a shared instance of TestServer */
	static void createInstance(SocketIOCallback connectedCallback = nullptr);
	static NodeServer *getInstance();

	/** purge the shared instance of TestServer */
	static void destroyInstance();

	/*void setDisconnectCallback(const SVCallback& callback);

	void setConnectCallback(const SVCallback& callback);*/

public:
	NodeServer();
	virtual ~NodeServer();

	bool init(SocketIOCallback cb);

	SIOClient* getClient();
	void freeClient();
	void loadConfigFile(const std::string& filename);
	void startConnectWithHandler(string name, SIOEvent event);
	void sendMessageWithName(string name, string message);
	string getString();
	CC_SYNTHESIZE(bool, _connectedFlg, ConnectedFlg);
private:
	static NodeServer    *s_sharedTestServer;
	static std::string		s_configfile;

	SVCallback _onConnectCallback;
	SVCallback _disConnectCallback;

	virtual void onConnect(SIOClient* client);
	virtual void onMessage(SIOClient* client, const std::string& data);
	virtual void onClose(SIOClient* client);
	virtual void onError(SIOClient* client, const std::string& data);

protected:

	SIOClient*  _valueDict;
	string _name;
};

// end of global group
/// @}


#endif // __CCTestServer_H__
