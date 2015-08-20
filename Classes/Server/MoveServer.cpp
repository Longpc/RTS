#include "MoveServer.h"

MoveServer* MoveServer::s_shareMoveServer = nullptr;
void MoveServer::createInstace()
{
	log("create move server instance");
	if (!s_shareMoveServer) {
		s_shareMoveServer = new (std::nothrow) MoveServer();
		s_shareMoveServer->init();
	}
}

bool MoveServer::init()
{
	_client = SocketIO::connect(Configuration::getInstance()->getValue("MoveServer").asString().c_str(), *this);
	CCASSERT(_client, "Error cannot create move server 's client");
	log("Connecting to Move server: %s", Configuration::getInstance()->getValue("MoveServer").asString().c_str());
	_client->on("connect", [&](SIOClient* c, const string& data){
		log("Connected to move server");
	});
	return true;
}

void MoveServer::onConnect(SIOClient* client)
{
	CC_UNUSED_PARAM(client);
	NotificationCenter::getInstance()->postNotification(MOVESV_CONNECTED_MSG, (Ref*)(intptr_t)1);
}

void MoveServer::onClose(SIOClient* client)
{
	CC_UNUSED_PARAM(client);
	log("move server closed");
	NotificationCenter::getInstance()->postNotification(MOVESV_DISCONNECTED_MSG, (Ref*)(intptr_t)1);
}

void MoveServer::onError(SIOClient* client, const std::string& data)
{
	CC_UNUSED_PARAM(client);
	log("move server error: %s", data.c_str());
}

void MoveServer::onMessage(SIOClient* client, const std::string& data)
{

}

void MoveServer::destroyInstance()
{
	if (s_shareMoveServer->getClient() != nullptr)
	{
		s_shareMoveServer->getClient()->disconnect();
	}
	s_shareMoveServer->freeClient();
	CC_SAFE_DELETE(s_shareMoveServer);
}

MoveServer::~MoveServer()
{
	_client = nullptr;
}
