
#include "Server.h"
#include "platform/CCFileUtils.h"


NodeServer* NodeServer::s_sharedTestServer = nullptr;

NodeServer::NodeServer()
{
}

NodeServer::~NodeServer()
{
}

NodeServer* NodeServer::getInstance()
{
	if (!s_sharedTestServer)
	{
		log("create new");
		s_sharedTestServer = new (std::nothrow) NodeServer();
		s_sharedTestServer->init();
	}

	return s_sharedTestServer;
}

void NodeServer::destroyInstance()
{
	CC_SAFE_RELEASE_NULL(s_sharedTestServer);
}

bool NodeServer::init()
{
	_valueDict = SocketIO::connect(Configuration::getInstance()->getValue("NodeJSServer").asString().c_str(), *this);
	CCASSERT(_valueDict, "Error cannot create Socket IO");
	log("Connect to NodeJs server: %s", Configuration::getInstance()->getValue("NodeJSServer").asString().c_str());
	return true;
}

//
// generic getters for properties
//
SIOClient* NodeServer::getClient()
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
void NodeServer::loadConfigFile(const std::string& filename)
{
	
}
void NodeServer::startConnectWithHandler(string name, SIOEvent event)
{
	_name = name;
	if (_valueDict)
	{
		_valueDict->on(name.c_str(), event);
		return;
	}
	log("Failed in start connect");
}

void NodeServer::sendMessageWithName(string name, string message)
{
	if (_valueDict) {
		_valueDict->emit(name.c_str(), message.c_str());
	}
	else {
		log("Failed in send message");
	}
	
}

void NodeServer::onConnect(SIOClient* client)
{
	log("----->onConnect");
}

void NodeServer::onMessage(SIOClient* client, const std::string& data)
{
	log("----->onMessage");
}

void NodeServer::onClose(SIOClient* client)
{
	log("----->onClose");
	this->destroyInstance();
}

void NodeServer::onError(SIOClient* client, const std::string& data)
{
	log("----->onError");
	log("with error: %s", data.c_str());
}

std::string NodeServer::getString()
{
	return _name;
}

