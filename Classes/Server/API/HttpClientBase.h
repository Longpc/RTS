#ifndef HTTP_CLIENT_BASE_H__
#define HTTP_CLIENT_BASE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "base/Define.h"
#include <string>
#include "json/rapidjson.h"
#include "json/document.h"

USING_NS_CC;
using namespace network;
using namespace std;

typedef std::function<void(HttpClient *cl, HttpResponse* response)> PostCallback;
class HttpClientBase :public Ref
{
public:
	static HttpClientBase *getInstance();
	static void destroyInstance();
	void postAPIWithMethodNameAndParam(string address, string dataParam, PostCallback callback = nullptr);
	void setAPICallback(PostCallback posBack);
	string getJSONData();
protected:
private:
	static HttpClientBase *s_HttpClientBase;
	string _data;
	PostCallback _callback = nullptr;
};

#endif