#ifndef MODE_SELECT_SCENE_H__
#define MODE_SELECT_SCENE_H__
///Include scene header here///
#include "../base/LayerBase.h"
#include "Multi/MemberFullDialogLayer.h"
#include "Multi/UnitSelectScene/UnitSelectScene.h"

#include "UserSelectScene/UserSelectScene.h"


#include "Server/API/StartPHPAPI.h"
#include "Server/API/HttpClientBase.h"

///Include library header here///
#include <string.h>

//#include "network/WebSocket.h"
#include "network/SocketIO.h"

#include "Server/Server.h"

class ModeSelectScene :public LayerBase
{
public:
	//static Scene *createScene();
	UI_SCENE_CREATE_FUNC(ModeSelectScene);
	CREATE_FUNC(ModeSelectScene);
	bool init();
protected:

private:
	int index;
	SIOClient* _client;
	TextField* editBox;
	// socket.io event‚Ìevent listener
	void onReceiveEvent(SIOClient* client, const std::string& data);
	void textFieldEvent(Ref *pSender, TextField::EventType type);
	void addTalkPlayer(const std::string& str);
	void addTalkOther(const std::string& str);

	/* websocket ver
	cocos2d::network::WebSocket* websocket;
	cocos2d::LabelTTF* status;
	virtual void onOpen(cocos2d::network::WebSocket* ws);
	virtual void onMessage(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::Data& data);
	virtual void onClose(cocos2d::network::WebSocket* ws);
	virtual void onError(cocos2d::network::WebSocket* ws, const cocos2d::network::WebSocket::ErrorCode& error);
	void sendMessage(float dt);
	*/
	///VARIABLES///////////////////////////////////////////////////////////////////////
	/*this flag is false when selected room was full*/
	bool _onAccess = true;
	bool _onDisableTouch = false;
	int _retryCount = 1;
	bool _startAPICallback = false;

	///FUNCTIONS///////////////////////////////////////////////////////////////////////
	virtual void multiButtonCallback(Ref *pSender, Widget::TouchEventType type);
	virtual void soloButtonCallback(Ref *pSender, Widget::TouchEventType type);
	/*
	Check multi-player room can accessible
	Return: true if accessible / false when full or can't access
	*/
	virtual bool checkRoomMember();

	/*virtual void testDialog(Ref *pSender, Widget::TouchEventType type);*/

	/*Create data base*/
	virtual void downloadDatabase();
	virtual void serverCallback(HttpClient* client, HttpResponse* response);
};

#endif