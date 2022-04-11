#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <Iphlpapi.h> // must be after WinSock2.h
#include <Windows.h>
#include <process.h>
#include <conio.h>

#include <iostream>
#include <ctime>
#include <string>

#include "json.hpp"

using namespace std;
using nlohmann::json;

#include "mazeMessages.h"
#include "jsonUtils.h"

#endif /* COMMON_H_ */
