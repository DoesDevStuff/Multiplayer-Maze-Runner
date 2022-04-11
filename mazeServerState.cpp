#include "mazeServerState.h"
#include "mazePlayer.h"

int mazeState = 0; // current maze state
//possible maze states
const int mazeState_NotInitialized = 0;
const int mazeState_Initialized = 1;
const int mazeState_RequiredPlayersJoined = 2;  //means both in lobby
const int mazeState_Abrakadabra = 3; //dummy state to match server's "game started" state (below) to client's "game started" state of value 4
const int mazeState_GameStarted = 4;
const int mazeState_GameEnded = 5;

const int maxPlayers = 2; //current implementation allows only 2 players
MazePlayer mazePlayers[maxPlayers]; //array of MazePlayer structures

int gate_X = -1;
int gate_Y = -1;

int playersJoined = 0; //how many players have joined

bool initializeMazeState() {
	for (int i=0; i < maxPlayers; i++) {
		MazePlayer *mazePlayerPtr = &mazePlayers[i];
		mazePlayerPtr->playerID = -1;
		mazePlayerPtr->playerIndex = -1; //index of the player in the player array;
		mazePlayerPtr->isPlayer = 0;
		mazePlayerPtr->playerState = -1;
		mazePlayerPtr->currentX = -1;
		mazePlayerPtr->currentY = -1;
		mazePlayerPtr->wonOrLost = false;
		//mazePlayer.SOCKET //don't initialize here
		mazePlayerPtr->tcpReceiveBuffer = new char(maxTcpPacketSize);
		mazePlayerPtr->sendMulticast = false;
		mazePlayerPtr->multicastSendBuffer = new char(maxUdpPacketSize);;
	}

	return true;
}

void setMazeState(int mazeStateToSet) {
	mazeState = mazeStateToSet;
}

int getMazeState() {
	return mazeState;
}
