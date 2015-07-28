#ifndef __CCTestServer_H__
#define __CCTestServer_H__

#include <string>
#include "cocos2d.h"
#include "network/SocketIO.h"

#include "base/Define.h"
USING_NS_CC;
using namespace cocos2d::network;
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

typedef std::function<void()> SVCallback;
typedef std::function<void(SIOClient* client, const std::string data)> SocketIOCallback;

class MYCLASS_DECLSPEC NodeServer : public Ref,public SocketIO::SIODelegate
{
public:

	/** returns a shared instance of TestServer */
	static void createInstance(SocketIOCallback connectedCallback = nullptr);
	static NodeServer *getInstance();

	/** purge the shared instance of TestServer */
	static void destroyInstance();

	void setDisconnectCallback(const SVCallback& callback);

	void setConnectCallback(const SVCallback& callback);

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
