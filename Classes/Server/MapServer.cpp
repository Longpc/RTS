#include "MapServer.h"

MapServer* MapServer::s_sharedMapServer = nullptr;
void MapServer::createInstance()
{
	if (!s_sharedMapServer) {
		log("create connect to map server");
		s_sharedMapServer = new (std::nothrow) MapServer();
		s_sharedMapServer->init();
	}
}

void MapServer::destroyInstace()
{
	if (s_sharedMapServer->getClient() != nullptr){
		s_sharedMapServer->getClient()->disconnect();
	}
	s_sharedMapServer->freeClient();
	CC_SAFE_DELETE(s_sharedMapServer);
}

bool MapServer::init()
{
	_client = SocketIO::connect(Configuration::getInstance()->getValue("MapServer").asString().c_str(), *this);
	CCASSERT(_client, "Error cannot create Socket IO");
	log("Connecting to Map server: %s", Configuration::getInstance()->getValue("MapServer").asString().c_str());
	_client->on("connect", [&](SIOClient* c, const string& data){
		log("Connected to map server");
	});
	return true;
}

void MapServer::onConnect(SIOClient* client)
{
	CC_UNUSED_PARAM(client);
	NotificationCenter::getInstance()->postNotification(MAPSV_CONNECTED_MSG, (Ref*)(intptr_t)1);
}

void MapServer::onClose(SIOClient* client)
{
	CC_UNUSED_PARAM(client);
	log("map server close");
	NotificationCenter::getInstance()->postNotification(MAPSV_DISCONNECTED_MSG, (Ref*)(intptr_t)1);
}

void MapServer::onError(SIOClient* client, const std::string& data)
{
	CC_UNUSED_PARAM(client);
	log("map server error: %s", data.c_str());
}

void MapServer::onMessage(SIOClient* client, const std::string& data)
{

}

MapServer::~MapServer()
{
	_client = nullptr;
}
