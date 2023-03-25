#ifndef JSONUTILS_H_
#define JSONUTILS_H_

#include "common.h"

char *jsonObjectToString(json jsonObj, char *jsonStringBuffer);
json  stringToJsonObject(char *jsonString);

#endif /* JSONUTILS_H_ */
