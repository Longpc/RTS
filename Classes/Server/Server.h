#ifndef __CCTestServer_H__
#define __CCTestServer_H__

#include <string>

#include "cocos2d.h"
#include "network/SocketIO.h"

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

class MYCLASS_DECLSPEC TestServer : public Ref,public SocketIO::SIODelegate
{
public:

	/** returns a shared instance of TestServer */
	static TestServer *getInstance();

	/** purge the shared instance of TestServer */
	static void destroyInstance();


public:
	virtual ~TestServer();

	bool init();

	SIOClient* getClient();

	void loadConfigFile(const std::string& filename);
	void startConnectWithHandler(string name, SIOEvent event);
	void sendMessageWithName(string name, string message);
	string getString();
private:
	TestServer(void);
	static TestServer    *s_sharedTestServer;
	static std::string		s_configfile;

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
