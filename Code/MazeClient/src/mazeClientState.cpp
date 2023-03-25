#include "mazeClientState.h"

//client game states
const int mazeClientState_NotInitialized = 0;
const int mazeClientState_Initialized = 1;
const int mazeClientState_Accepted = 2;
const int mazeClientState_Connected = 3; //TCP connected --- in lobby
const int mazeClientState_GameStarted = 4;
const int mazeClientState_Won = 5;
const int mazeClientState_Lost = 6;

int mazeClientState = mazeClientState_NotInitialized;
int mazePlayerID = 0;
bool amIPlayer = false;

//positions changed from server update
bool positionsChangedByServer = false;
//positions changed locally
bool positionsChangedLocally = false;

//flag to trigger sending new positions to server
bool sendNewPositionsToserver = false;

//locally changed new X and Y from doPlayerMovements()
int newLocalX = -1;
int newLocalY = -1;

//positions of both players
int playerX = -1;
int playerY = -1;
int opponentX = -1;
int opponentY = -1;

int gate_X = -1;
int gate_Y = -1;

bool updateLocalChange = false;

bool stopBroadcast = false;

/*
void setMazeState(int mazeStateToSet) {
	mazeState = mazeStateToSet;
}

int getMazeState() {
	return mazeState;
}
*/
