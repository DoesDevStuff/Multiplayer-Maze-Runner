#include "mazePlayer.h"

//player states - values as below, playerState in MazePlayer holds one of these
const int playerState_notConnected = 0;				//not connected
const int playerState_calibrationInProgress = 1;	//connected and calibration is in progress
const int playerState_accepted = 2;					//broadcast accepted
const int playerState_tcpConnected = 3;				//TCP connected - in lobby
const int playerState_playing = 4;					//playing
const int playerState_Won = 5;						//implicit: game ended
const int mazeClientState_Lost = 6;					//implicit: game ended
