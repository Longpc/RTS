
#include "Server.h"
#include "platform/CCFileUtils.h"


TestServer* TestServer::s_sharedTestServer = nullptr;

TestServer::TestServer()
{
}

TestServer::~TestServer()
{
}

TestServer* TestServer::getInstance()
{
	if (!s_sharedTestServer)
	{
		log("create new");
		s_sharedTestServer = new (std::nothrow) TestServer();
		s_sharedTestServer->init();
	}

	return s_sharedTestServer;
}

void TestServer::destroyInstance()
{
	CC_SAFE_RELEASE_NULL(s_sharedTestServer);
}

bool TestServer::init()
{
	_valueDict = SocketIO::connect(Configuration::getInstance()->getValue("NodeJSServer").asString().c_str(), *this);
	CCASSERT(_valueDict, "Error cannot create Socket IO");
	return true;
}

//
// generic getters for properties
//
SIOClient* TestServer::getClient()
{
	if (_valueDict)
	{
		return _valueDict;
	}
	log("Return null in getClient");
	return nullptr;
}

//
// load file
//
void TestServer::loadConfigFile(const std::string& filename)
{
	
}
void TestServer::startConnectWithHandler(string name, SIOEvent event)
{
	_name = name;
	if (_valueDict)
	{
		_valueDict->on(name.c_str(), event);
		return;
	}
	log("Failed in start connect");
}

void TestServer::sendMessageWithName(string name, string message)
{
	if (_valueDict) {
		_valueDict->emit(name.c_str(), message.c_str());
	}
	else {
		log("Failed in send message");
	}
	
}

void TestServer::onConnect(SIOClient* client)
{
	log("----->onConnect");
}

void TestServer::onMessage(SIOClient* client, const std::string& data)
{
	log("----->onMessage");
}

void TestServer::onClose(SIOClient* client)
{
	log("----->onClose");
}

void TestServer::onError(SIOClient* client, const std::string& data)
{
	log("----->onError");
	log("with error: %s", data.c_str());
}

std::string TestServer::getString()
{
	return _name;
}

