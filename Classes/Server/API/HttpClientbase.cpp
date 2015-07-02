#include "HttpClientBase.h"

HttpClientBase* HttpClientBase::s_HttpClientBase = nullptr;
HttpClientBase * HttpClientBase::getInstance()
{
	if (!s_HttpClientBase) {
		s_HttpClientBase = new (std::nothrow) HttpClientBase();

	}
	return s_HttpClientBase;
}

void HttpClientBase::destroyInstance()
{
	CC_SAFE_RELEASE_NULL(s_HttpClientBase);
}

void HttpClientBase::postAPIWithMethodNameAndParam(string add, string dataParam , PostCallback callBack)
{
	if (callBack!= nullptr) {
		_callback = callBack;
	}
	auto httpRequest = new HttpRequest();
	auto addressValue = Configuration::getInstance()->getValue("APIServer");
	string address = addressValue.asString().c_str();
	httpRequest->setUrl(address.append(add.c_str()).c_str());
	httpRequest->setRequestType(HttpRequest::Type::POST);
	httpRequest->setResponseCallback(callBack);
	httpRequest->setTag("clientBase");
	log("POSTING DATA: %s", dataParam.c_str());
	const char* buffer = dataParam.c_str();
	httpRequest->setRequestData(buffer, strlen(buffer));

	auto client = HttpClient::getInstance();
	client->enableCookies(nullptr);
	client->send(httpRequest);
	httpRequest->release();
}

std::string HttpClientBase::getJSONData()
{
	return _data;
}

void HttpClientBase::setAPICallback(PostCallback posBack)
{
	if (posBack != nullptr) {
		_callback = posBack;
	}
}

