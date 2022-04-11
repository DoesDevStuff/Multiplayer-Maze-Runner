#include "MazeClientController.h"
#include "mazeClientAddresses.h"
#include "tcpSender.h"
#include "multicastReceiver.h"
#include "mazeClientState.h"

boolean hasCalibrationStarted = false;

int processBroadcastMessage(char *broadcastMessage, char *returnBroadcastMessage) {
	boolean debug = false;


	string incomingMessage(broadcastMessage);
	int messageLength = incomingMessage.size();

//TODO: { use stringToJsonObject() from jsonUtils
	//detect if this could be json - start and end should be { and }
	if ( (messageLength < 2) || (incomingMessage[0] != '{') || (incomingMessage[messageLength-1] != '}') ) {
		cout << "processBroadcastMessage: invalid Maze message" << endl;
		return 0;
	}

	json jsonMessage = json::parse(broadcastMessage);
	if (jsonMessage == NULL) {
		cout << "processBroadcastMessage: could not parse message from client: " << incomingMessage << endl;
		return 0;
	}
//TODO: } use stringToJsonObject() from jsonUtils

	//get the type of message received from server
	string messageType = jsonMessage.value(mazeMessageType, "no value");
	if (debug) cout << "processBroadcastMessage: messageType: " << messageType << endl;


	if (messageType == mmt_newPlayerCalibration) {
		if (!hasCalibrationStarted) {
			cout << "Calibration started..." << endl;
			hasCalibrationStarted = true;
		}

		return 1; //message type is mmt_newPlayerCalibration --- return 1 to send calibration message back to server
	}

	if (messageType != mmt_newPlayerServerResponse)
		return 0; //return false if the request is not mmt_newPlayerJoinRequest (there are only 2 types)

	bool isPlayerAccepted = jsonMessage.value(mmvar_isAccepted, false);
	if (isPlayerAccepted == false) {
		cout << "processBroadcastMessage: Server did not accept joining request, quitting..." << endl;
		cout << broadcastMessage << endl;
		quitClient = true; //TODO: ???is this okay here or move to elsewhere??
		return 0;
	}

	//player accepted
	int clientID = jsonMessage.value(mmvar_clientId, -1);
	bool isPlayer = jsonMessage.value(mmvar_isPlayer, 99);
	string serverTcpAddress = jsonMessage.value(mmvar_serverTcpAddress, "no value");
	int serverTcpPort = jsonMessage.value(mmvar_serverTcpPort, 0);
	string serverMulticastAddress = jsonMessage.value(mmvar_serverMulticastAddress, "no value");
	int serverMulticastPort = jsonMessage.value(mmvar_serverMulticastPort, 0);

	//update player details, state
	mazePlayerID = clientID;
	amIPlayer = isPlayer;
	mazeClientState = mazeClientState_Accepted;
	//copy tcp address and port
	strcpy(tcpAddressString, serverTcpAddress.c_str());
	tcpPort = serverTcpPort;
	//copy multicast address and port
	strcpy(multicastAddressString, serverMulticastAddress.c_str());
	multicastPort = serverMulticastPort;

/*
	cout << "messageType: " << messageType << endl;
	cout << "isPlayerAccepted: " << isPlayerAccepted << endl;
	cout << "clientID: " << clientID << endl;
	cout << "isPlayer: " << isPlayer << endl;
	cout << "serverTcpAddress: " << serverTcpAddress << endl;
	cout << "serverTcpPort: " << serverTcpPort << endl;
	cout << "serverMulticastAddress: " << serverMulticastAddress << endl;
	cout << "serverMulticastPort: " << serverMulticastPort << endl;
*/

	cout << "Calibration complete" << endl;
	hasCalibrationStarted = false;

	cout << "processBroadcastMessage: Server accepted joining request, I am: " << (isPlayer ? "Player" : "Opponent") << endl;
	cout << broadcastMessage << endl;


	//first start multicast thread that listens to multicast details provided by server to get player movements
	if (!initializeAndStartMulticastReceiver())
		return 0;

	//then connect to server on tcp details provided by server to send self movements to server
	if (!initializeAndStartTcpSenderThread())
		return 0;

	//once startMulticastThread() and startTCPThread() are successful, client is in lobby
	return 2;
}

bool processMulticastMessage(char *multicastReceiveBuffer) {
	bool debug = true;
	json jsonMulticastCastObject = stringToJsonObject(multicastReceiveBuffer);
	if (jsonMulticastCastObject == NULL) {
		cout << "processMulticastMessage: could not convert client message to json object: " << multicastReceiveBuffer << endl;
		return false;
	}

	string messageType = jsonMulticastCastObject.value(mazeMessageType, "no value");
	if (messageType != mmt_gameAndPlayerStatus) {
		cout << "processMulticastMessage: Wrong message type: " << messageType << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: messageType: " << messageType << endl;

	int gameState = jsonMulticastCastObject.value(mmvar_gameState, -1);
	if (gameState < 0) {
		cout << "processMulticastMessage: bad gameState: " << gameState << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: gameState: " << gameState << endl;

	int clientId = jsonMulticastCastObject.value(mmvar_clientId, -1);
	if (clientId <= 0) {
		cout << "processMulticastMessage: bad clientId: " << clientId << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: clientId: " << clientId << endl;

	// no validation needed
	bool isPlayer = jsonMulticastCastObject.value(mmvar_isPlayer, -1);
	if (debug) cout << "processMulticastMessage: isPlayer: " << isPlayer << endl;

	int updatedX = jsonMulticastCastObject.value(mmvar_playerX, -1);
	if (updatedX <= 0) {
		cout << "processMulticastMessage: bad updatedX: " << updatedX << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: updatedX: " << updatedX << endl;

	int updatedY = jsonMulticastCastObject.value(mmvar_playerY, -1);
	if (updatedY <= 0) {
		cout << "processMulticastMessage: bad updatedY: " << updatedY << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: updatedY: " << updatedY << endl;


	int playerState = jsonMulticastCastObject.value(mmvar_playerState, -1);
	if (playerState <= 0) {
		cout << "processMulticastMessage: bad playerState: " << playerState << endl;
		return false;
	}
	if (debug) cout << "processMulticastMessage: playerState: " << playerState << endl;


	if (isPlayer) { //update for player
		playerX = updatedX;
		playerY = updatedY;
	}
	else { //update for opponent
		opponentX = updatedX;
		opponentY = updatedY;
	}

	//since player positions may have changed, set the positionsChangedByServer flag
	//so that playerMovementsThread (in mazeGui.cpp) will redraw the players on the board
	positionsChangedByServer = true;

	//update client state from server message
	mazeClientState = playerState;

	return true;
}
