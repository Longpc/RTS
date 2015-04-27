#ifndef LIST_USER_PHP_API_H__
#define LIST_USER_PHP_API_H__

#include "HttpClientBase.h"
#include "base/database/DataUtils.h"

typedef std::function<void(vector<RoomUser>)> ListUserCallback;

class ListUserAPI : public Ref
{
public:
	static ListUserAPI *getInstance();
	static void destroyInstance();

	bool init();
	/*return true if callback server already response and this data was loaded
	, false when mean this class not initial before this function was called
	*/
	bool setLoadDataCompledCallback(ListUserCallback callB);
	/*
	return the list of user get from sever. Null when sever not response or response data was null*/
	vector<RoomUser> getListUserData();
	string getUserNameByUserId(int user_id);

private:
	static ListUserAPI *s_ListUserAPI;
	vector<RoomUser> _listUser;
	ListUserCallback _callBack;
	bool _isResponsed = false;
};

#endif