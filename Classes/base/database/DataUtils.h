#ifndef _DATA_UTILS_H__
#define _DATA_UTILS_H__

#include <stdlib.h>
#include "cocos2d.h"
#include "extensions/cocos-ext.h"

#include "base/Define.h"

#include <string>
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace std;
class DataUtils
{
public:
	/*
	convert number to string
	@number float : number to convert into a string
	Return: a string
	*/
	static string numberToString(float number);
	/*Convert string containing the number to number
	@str string: string containing the number
	Return: the number if it was contained by inputted string or default value = 0
	*/
	static float stringToFloat(const string str);
	/*
	
	*/
	static char* MultiByteToUTF8(const char* src);

	/*Convert UnitInfoNewTo Json Value*/
	static rapidjson::Document::GenericValue& convertUnitDataToJsonObject(UnitInforNew unitData, rapidjson::Document::AllocatorType& allo);
};

#endif