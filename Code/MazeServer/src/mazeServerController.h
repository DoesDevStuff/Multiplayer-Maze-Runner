#ifndef MAZESERVERCONTROLLER_H_
#define MAZESERVERCONTROLLER_H_

#include "common.h"

//flag for server to quit
extern bool quitServerFlag;

//used while calibration of a client is in progress
typedef struct {
	bool calibrationInProgress = false;
	struct sockaddr_in receivedBroadcastFromAddress;
	int messagesSent = 0;
	int messagesReceived = 0;
	bool isCalibrationSuccessful = false;
	long startMillisec = -1; //start of calibration
	long endMillisec = -1;   //end of calibration
	long maximumMillisecToPass = -1; //calibration delay should not be larger than this
	int maxCalibrationRepeatCount; //how many times to send the calibration message to find bytes per second
	const char *calibrationString; //calibration string
	int calibrationStringSize;
} MazeCalibration;

extern MazeCalibration mazeCalib;

bool generateCalibrationMessage(char *calibrationMessageBuffer);
long getCurrentMillisec();
bool resetCalibStruct();
bool initializeController();
void quitServer();

bool processBroadcastMessage(char *broadcastMessage, char *returnBroadcastMessage, struct sockaddr_in receivedBroadcastFromAddress);
bool processNewTcpConnection(SOCKET newSocket);
bool processTcpPlayerMovementMessage(int playerIndex);

#endif /* MAZESERVERCONTROLLER_H_ */
