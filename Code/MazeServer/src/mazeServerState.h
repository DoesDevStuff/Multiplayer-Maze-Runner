#ifndef MAZESERVERSTATE_H_
#define MAZESERVERSTATE_H_

#include "common.h"
#include "buffers.h"
#include "mazePlayer.h"

extern int mazeState;  // current maze state
//possible maze states
extern const int mazeState_NotInitialized;
extern const int mazeState_Initialized;
extern const int mazeState_RequiredPlayersJoined; //means both in lobby
extern const int mazeState_Abrakadabra; //dummy state
extern const int mazeState_GameStarted;
extern const int mazeState_GameEnded;

extern const int maxPlayers; //current implementation allows only 2 players

extern int gate_X;
extern int gate_Y;

extern MazePlayer mazePlayers[]; //array of MazePlayer structures
extern int playersJoined;  //how many players have joined

bool initializeMazeState();
void setMazeState(int mazeStateToSet);
int getMazeState();

#endif /* MAZESERVERSTATE_H_ */
