#ifndef COMMON_H_
#define COMMON_H_

using namespace std;

extern bool quitClient;
extern bool guiInitialized;
extern int playerOrChaser;

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
#include "constants.h"
#include "buffers.h"

#include "json.hpp"
using nlohmann::json;

#include "mazeMessages.h"

#include "jsonUtils.h"

#endif /* COMMON_H_ */
