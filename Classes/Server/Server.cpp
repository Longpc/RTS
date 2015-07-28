
#include "Server.h"
#include "platform/CCFileUtils.h"


NodeServer* NodeServer::s_sharedTestServer = nullptr;

NodeServer::NodeServer() : _connectedFlg(false)
{
}

NodeServer::~NodeServer()
{
	_disConnectCallback = nullptr;
	_onConnectCallback = nullptr;
	_valueDict = nullptr;
}
void NodeServer::createInstance(SocketIOCallback connectedCallback /* = nullptr */)
{
	if (!s_sharedTestServer)
	{
		log("create new");
		s_sharedTestServer = new (std::nothrow) NodeServer();
		s_sharedTestServer->init(connectedCallback);
	}
}

NodeServer* NodeServer::getInstance()
{
	
	return s_sharedTestServer;
}

void NodeServer::destroyInstance()
{
	if (s_sharedTestServer->getClient() != nullptr) {
		s_sharedTestServer->getClient()->disconnect();
	}
	s_sharedTestServer->freeClient();
	CC_SAFE_DELETE(s_sharedTestServer);
}

void NodeServer::freeClient()
{
	//CC_SAFE_DELETE(_valueDict);
	_valueDict = nullptr;
}

bool NodeServer::init(SocketIOCallback cb)
{
	_valueDict = SocketIO::connect(Configuration::getInstance()->getValue("NodeJSServer").asString().c_str(), *this);
	CCASSERT(_valueDict, "Error cannot create Socket IO");
	log("Connect to NodeJs server: %s", Configuration::getInstance()->getValue("NodeJSServer").asString().c_str());
	_valueDict->on("connect", cb);
	return true;
}

//
// generic getters for properties
//
SIOClient* NodeServer::getClient()
{
	/*if (_valueDict)
	{
		return _valueDict;
	}
	log("Return null in getClient");
	return nullptr;*/
	return _valueDict;
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
	log("Node Server-->onConnect");
	if (_onConnectCallback)
	{
		_onConnectCallback();
	}
}

void NodeServer::onMessage(SIOClient* client, const std::string& data)
{
	//log("----->onMessage");
	//log("%s", data.c_str());
}

void NodeServer::onClose(SIOClient* client)
{
	/*if (client == _valueDict)
	{
		_valueDict = nullptr;
	}*/
	setConnectedFlg(false);
	log("Node Server: ----->onClose. We need disconnect callback here for reconnect in battle scene");
	if (_disConnectCallback) {
		_disConnectCallback();
	}
	NotificationCenter::getInstance()->postNotification(DISCONNECT_MSG, (Ref*)(intptr_t)1);
	//this->release();
}

void NodeServer::onError(SIOClient* client, const std::string& data)
{
	log("----->onError");
	log("with error: %s", data.c_str());
	setConnectedFlg(false);
	//_valueDict->disconnect();
	//this->release();
}

std::string NodeServer::getString()
{
	return _name;
}

void NodeServer::setDisconnectCallback(const SVCallback& callback)
{
	this->_disConnectCallback = callback;
}

void NodeServer::setConnectCallback(const SVCallback& callback)
{
	this->_onConnectCallback = callback;
}


