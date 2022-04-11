#include "jsonUtils.h"

char *jsonObjectToString(json jsonObj, char *jsonStringBuffer) {
	if ( (jsonObj == NULL) || (jsonStringBuffer == NULL) ) {
		return NULL;
	}

	string jsonMessageDump = jsonObj.dump();
    int len = jsonMessageDump.size();
    copy(jsonMessageDump.begin(), jsonMessageDump.end(), jsonStringBuffer);
    jsonStringBuffer[len] = '\0';

    return jsonStringBuffer;
}

json stringToJsonObject(char *jsonString) {
	if (jsonString == NULL)
		return NULL;

	int jsonStringLength = strlen(jsonString);
	//detect if this could be json - start and end should be { and }
	if ( (jsonStringLength < 2) || (jsonString[0] != '{') || (jsonString[jsonStringLength-1] != '}') ) {
		cout << "stringToJsonObject: not a json string: " << jsonString << endl;
		return NULL;
	}

	json jsonObj;
	try {
	jsonObj = json::parse(jsonString);
	}
	catch(exception& e) {
		//jsonObj = NULL;
		cout << "exception converting string to json object: " << e.what() << endl;
		cout << "json string: " << endl << jsonString << endl;
		return NULL;
	}

	if (jsonObj == NULL)
		cout << "stringToJsonObject: could not parse message: " << jsonString << endl;

	return jsonObj;
}
