//============================================================================
// Name        : MazeClient.cpp
// Author      : 
// Version     :
// Description : Maze Client
//============================================================================

#include "common.h"
#include "mazeGui.h"
#include "broadcastSender.h"
#include "mazeClientState.h"

//flag for client to quit
bool quitClient = false;
//Gui initialized flag
bool guiInitialized = false;

bool initializeComponents() {
//*** SF initialization NOT done here: SF has to be initialized in the same thread where the events are handled
//--- done in playerMovementsThread() in mazeGui.cpp, this thread is started by launchThreads() function in this cpp

	if (!initializeBroadcastThread()) {
		cout << "initialization of broadcast thread failed" << endl;
		return false;
	}

	mazeClientState = mazeClientState_Initialized;
	return true;
}

bool launchThreads() {
	//SF Initialization is done in the thread playerMovementsThread in mazeGui.cpp, since
	//SF initialization is required done in the same thread where the events are handled
	_beginthread(playerMovementsThread, 0, NULL);
	while (!guiInitialized) {Sleep(loopDelay);}; // wait till GUI has started

	Sleep(oneSecDelay); //wait a second before starting other threads
	_beginthread(sendBroadcastThread, 0, NULL);

	return true;
}

//TODO: move all code to mazeClientController except the quitClient while loop
int main() {
	cout << "Hello World" << endl; // prints
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (!initializeComponents()) {
		cout << "initialization of Maze components failed, exiting..." << endl;
		return 1;
	}

	if (!launchThreads()) {
		cout << "Launching Maze threads failed, exiting..." << endl;
		return 1;
	}

	cout << "Maze Client started" << endl;

	//do nothing
	while (!quitClient) {
		Sleep(loopDelay);
	}

	closesocket(sendBroadcastSocket);
	WSACleanup();

	cout << "Maze Client has quit" << endl;

	return 0;
}
