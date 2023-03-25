#include "mazeServerController.h"

#include "broadcastListener.h"
#include "mazeAddresses.h"
#include "mazeServerState.h"
#include "mazeMessages.h"
#include "jsonUtils.h"
#include "multicastSender.h"
#include "tcpReceiver.h"
#include <sys/time.h>
#include "jsonUtils.h"

//flag for server to quit
bool quitServerFlag = false;

//calibration string: added with json keywords, the json Message string will be approx 1024 byes
const char *calibrationStringData = "The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head. The Quick Brown Fox Jumps Over The Lazy Dog's Head.";
MazeCalibration mazeCalib;
//global buffer to hold the calibration message to be sent to client
const int maxCalibrationMessageBufferSize = 1408;
char calibrationMessageBuffer[maxCalibrationMessageBufferSize];


void quitServer() {
	try {
		//giving exceptions if these are called while threads are running, commenting them for now TODO: find solution for this
		//closesocket(receiveBroadcastSocket);
		//closesocket(broadcastResponseSocket);
		//WSACleanup();
	}
	catch(exception &ex) {
		cout << " quitServer: exception: " << ex.what() << endl;
		return;
	}
}

bool generateCalibrationMessage(char *calibrationMessageBuffer) {
	json jsonCalibrationObject;
	cout << "generating calibration message..." << endl;

	jsonCalibrationObject[mazeMessageType] = mmt_newPlayerCalibration;
	jsonCalibrationObject[mmvar_calibrationData] = calibrationStringData;

	//convert json message object to string
	jsonObjectToString(jsonCalibrationObject, calibrationMessageBuffer);
    cout << "calibrationMessageBuffer: " << calibrationMessageBuffer << endl;

    return true;
}

long getCurrentMillisec() {
	struct timeval tvStruct;

	gettimeofday(&tvStruct, NULL);
	long millisec = tvStruct.tv_sec * 1000 + tvStruct.tv_usec / 1000;

    return millisec;
}

bool resetCalibStruct() {
	mazeCalib.calibrationInProgress = false;
	mazeCalib.messagesSent = 0;
	mazeCalib.messagesReceived = 0;
	mazeCalib.isCalibrationSuccessful = false;
	mazeCalib.startMillisec = -1;
	mazeCalib.endMillisec = -1;
	mazeCalib.maximumMillisecToPass = 20000; //TODO: is this the correct maximum?
	mazeCalib.maxCalibrationRepeatCount = 1000;
	mazeCalib.calibrationString = calibrationMessageBuffer;
	mazeCalib.calibrationStringSize = strlen(mazeCalib.calibrationString);

	return true;
}

bool initializeController() {
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	generateCalibrationMessage(calibrationMessageBuffer);
	resetCalibStruct();

	setMazeState(mazeState_NotInitialized);
	initializeMazeState();

    //get this machine's name
	char hostName[256]; // Host name buffer
	int hostNameSize = gethostname(hostName, sizeof(hostName));
	hostName[hostNameSize] = 0; //add ending zero

	//get tcp host info
	struct hostent *host_info;
	host_info = gethostbyname(hostName);
	cout << "Host Name: " << host_info->h_name << endl;

	//get this machine's IP address (take the first found)
	struct in_addr hostAddress;
	hostAddress.s_addr = *(u_long *) host_info->h_addr_list[0]; // take the first IP address
	strcpy(tcpAddressString, inet_ntoa(hostAddress)); //convert IPv4 address to dotted-decimal format

	//fill tcpaddres
	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_addr.s_addr = hostAddress.s_addr;
	tcpAddress.sin_port = htons(tcpPort);

	cout << "Server IP Address: " << tcpAddressString << endl;

	//initialize multicast sender - starts multicastThread
	if (!initializeMulticastSender()) {
		cout << "Initialization of multicast failed." << endl;
		return false;
	}

	//initialize tcp listener starts tcpListenThread
	if (!initializeTcpListener()) {
		cout << "Initialization of tcp listener failed." << endl;
		return false;
	}

	//initialize broadcast receiver - starts broadcast thread
	if (!initializeMazeBroadcast()) {
		cout << "Initialization of broadcast failed." << endl;
		return false;
	}

	cout << "Initialization complete, Maze Server started, players can now join." << endl;
	setMazeState(mazeState_Initialized);

	return true;
}

bool processCalibrationMessage(const char *broadcastMessage, char *returnBroadcastMessage) {
	try {
		if (!mazeCalib.calibrationInProgress)
			return false; //spurious case where a calibration message is received without in progress flag = true

		//start of calibration
		if (mazeCalib.startMillisec < 0) {
			mazeCalib.startMillisec = getCurrentMillisec();
			mazeCalib.messagesSent++;
			strcpy(returnBroadcastMessage, calibrationMessageBuffer);
			return true; //start sending calibration message
		}

		// fail if the client does not return the same calibration message as sent
		if (strcmp(broadcastMessage, calibrationMessageBuffer) != 0) {
			mazeCalib.isCalibrationSuccessful = false;
			return false; //calibration complete, and failed
		}

		mazeCalib.messagesSent++;

		//if number of calibration iterations are done
		if (mazeCalib.messagesSent >= mazeCalib.maxCalibrationRepeatCount) {
			mazeCalib.endMillisec = getCurrentMillisec();
			long timeTaken = mazeCalib.endMillisec - mazeCalib.startMillisec;

			cout << "processCalibrationMessage: calibration complete, time taken: " << timeTaken << "(ms) for " << mazeCalib.maxCalibrationRepeatCount << " calibration iterations" << endl;

			//calibration is successful if time taken is less than maximumMillisecToPass;
			mazeCalib.isCalibrationSuccessful = (timeTaken <= mazeCalib.maximumMillisecToPass) ? true : false;

			if (mazeCalib.isCalibrationSuccessful)
				cout << "processCalibrationMessage: calibration is successful" << endl;
			else
				cout << "processCalibrationMessage: calibration failed because of time taken beyond acceptable limits" << endl;

			//return false as calibration is complete, and the results are in mazeCalib.isCalibrationSuccessful
			return false;
		}

		//continue doing calibration as number of calibration iterations are not complete
		strcpy(returnBroadcastMessage, calibrationMessageBuffer);
		return true;
	}
	catch(exception &ex) {
		cout << " processCalibrationMessage: exception: " << ex.what() << endl;
		return false;
	}
}

bool processBroadcastMessage(char *broadcastMessage, char *returnBroadcastMessage, struct sockaddr_in receivedBroadcastFromAddress) {
	try {
		bool debug = false;

		json jsonBroadCastObject = stringToJsonObject(broadcastMessage);
		if (jsonBroadCastObject == NULL) {
			cout << "processBroadcastMessage: could not convert client message to json object: " << broadcastMessage << endl;
			return false;
		}

		string messageType = jsonBroadCastObject.value("mazeMessageType", "no value");
		if (debug) cout << "processBroadcastMessage: messageType: " << messageType << endl;

		// if message type is none of the following, send nothing back (invalid broadcast message)
		if ( !( (messageType == mmt_newPlayerJoinRequest) || (messageType == mmt_newPlayerCalibration) ) ) {
			cout << "processBroadcastMessage: invalid messageType: " << messageType << endl;
			return false; //return false if the request none of mmt_newPlayerJoinRequest mmt_newPlayerCalibration
		}

		//if it is a join request, and calibration is in progress, return nothing to client (let it retry)
		if ( (messageType == mmt_newPlayerJoinRequest) && (mazeCalib.calibrationInProgress) ) {
			if (debug) cout << "processBroadcastMessage: calibration is in progress, not resonding to: " << messageType << endl;
			return false;
		}

		bool allPlayersJoined = (playersJoined >= maxPlayers) ? true : false;

		//calibration is in progress
		if ( (messageType == mmt_newPlayerCalibration) && (mazeCalib.calibrationInProgress) ) {
			if (debug) cout << "processBroadcastMessage: calibration is in progress: " << messageType << endl;
			if (processCalibrationMessage(broadcastMessage, returnBroadcastMessage))
				return true; //keep doing calibration

			//if false, calibration is complete, results will be in mazeCalib structure
			//*** now code should fall to isPlayerAccepted
		}

		//code comes here when there is new join request and calibration not done - calibration is initiated
		if ( (messageType == mmt_newPlayerJoinRequest) && (!mazeCalib.calibrationInProgress) && (!allPlayersJoined) ) {
			resetCalibStruct();
			mazeCalib.calibrationInProgress = true;
			cout  << endl << "*** processBroadcastMessage: New player join request received, starting calibration..." << endl;
			if (processCalibrationMessage(broadcastMessage, returnBroadcastMessage))
				return true;

			//return false just in case --- ideally there should not be a case that processCalibrationMessage()
			//will return false at this point of initiating calibration
			return false;
		}

		//new player will be accepted only if required players have not joined and calibration is passed (see "bool isPlayerAccepted =" above
		bool isPlayerAccepted = ( (!allPlayersJoined) && (mazeCalib.isCalibrationSuccessful) ) ? mmval_true : mmval_false;

		//prepare the accept/reject broadcast response
		json jsonResponse;
		jsonResponse[mazeMessageType] = mmt_newPlayerServerResponse; //type of message
		jsonResponse[mmvar_isAccepted] = isPlayerAccepted;
		//if not accepted, return with only this variable (isPlayerAccepted) in message
		if (!isPlayerAccepted) {
			//prepare rejected broadcast response
			jsonObjectToString(jsonResponse, returnBroadcastMessage);

			//print reason why player was rejected
			if (allPlayersJoined)
				cout << "processBroadcastMessage: player rejected because all players have already joined" << endl;
			else if (!mazeCalib.isCalibrationSuccessful) cout << "processBroadcastMessage: player rejected because calibration failed" << endl;

			//process complete for new player - reset maze calibration
			resetCalibStruct();
			return true;
		}

		//accepted: fill player details
		mazePlayers[playersJoined].playerID = (playersJoined + 1);
		mazePlayers[playersJoined].playerIndex = playersJoined;
		mazePlayers[playersJoined].isPlayer = (playersJoined == 0) ? mmval_true : mmval_false;
		mazePlayers[playersJoined].playerState = playerState_accepted;
		//currentX and currentY updated later
		mazePlayers[playersJoined].wonOrLost = false;

		//prepare accepted broadcast response
		//jsonResponse[mazeMessageType] = mmt_newPlayerServerResponse; //type of message already set above
		//jsonResponse[mmvar_isAccepted] = isPlayerAccepted; //accepted or rejected already set above
		jsonResponse[mmvar_clientId] = mazePlayers[playersJoined].playerID;
		jsonResponse[mmvar_isPlayer] = mazePlayers[playersJoined].isPlayer;
		jsonResponse[mmvar_serverTcpAddress] = tcpAddressString; //server's tcp details
		jsonResponse[mmvar_serverTcpPort] = tcpPort;
		jsonResponse[mmvar_serverMulticastAddress] = multicastAddressString; //server's multicast details
		jsonResponse[mmvar_serverMulticastPort] = multicastPort;

		jsonObjectToString(jsonResponse, returnBroadcastMessage);

		++playersJoined; //***increment player count***

		cout << "processBroadcastMessage: player accepted, playersJoined=" << playersJoined << endl;

		//process complete for new player - reset maze calibration
		resetCalibStruct();
		return true;
	}
	catch(exception &ex) {
		cout << " processBroadcastMessage: exception: " << ex.what() << endl;
		return false;
	}
}

bool processNewTcpConnection(SOCKET newSocket) {
	try {
		bool debug = true;

		//read message from new tcp connection, if it is coming from a joined client,
		//store the socket in corresponding player in mazePlayers array,
		//and start a new thread to receive new move messages from this socket

		//read this new socket for playerTcpConnection message"
		int bytesReceived = recv(newSocket, tcpReceiveBuffer, maxTcpPacketSize, 0);
		if (bytesReceived <= 0) {
			return false; //no content - return
			cout << "processNewTcpConnection: error receiving message from new tcp client connection"<< endl;
		}

		tcpReceiveBuffer[bytesReceived] = 0; //add ending zero
		if (debug) cout << "processNewTcpConnection: message from new tcp client connection: bytes=" << bytesReceived << ", Message:" << tcpReceiveBuffer << endl;

		json newTcpConnectionJsonObject = stringToJsonObject(tcpReceiveBuffer);
		if (newTcpConnectionJsonObject == NULL) {
			cout << "processNewTcpConnection: Bad json string"<< endl;
			return false;
		}

		//const char *mmt_playerTcpConnection = "playerTcpConnection";
		//variables in this message type, expected values:
		//const char *mmvar_clientID = "clientID"; //client ID
		string messageType = newTcpConnectionJsonObject.value(mazeMessageType, "no value");
		if (messageType != mmt_playerTcpConnection) {
			cout << "processNewTcpConnection: message type not of type: " << mmt_playerTcpConnection << endl;
			return false; //not mmt_playerTcpConnection
		}

		int clientID = newTcpConnectionJsonObject.value(mmvar_clientId, -1);
		if (clientID <= 0) {
			cout << "processNewTcpConnection: Bad client ID: " << clientID << endl;
			return false; //bad client ID
		}

		int playerX = newTcpConnectionJsonObject.value(mmvar_playerX, -1);
		if (playerX <= 0) {
			cout << "processNewTcpConnection: Bad playerX: " << playerX << endl;
			//return false; //bad playerX
		}

		int playerY = newTcpConnectionJsonObject.value(mmvar_playerY, -1);
		if (playerY <= 0) {
			cout << "processNewTcpConnection: Bad playerY: " << playerY << endl;
			//return false; //bad playerY
		}

		int gateX = newTcpConnectionJsonObject.value(mmvar_gateX, -1);
		if (gateX <= 0) {
			cout << "processNewTcpConnection: Bad gateX: " << gateX << endl;
			//return false; //bad gateX
		}
		else gate_X = gateX;

		int gateY = newTcpConnectionJsonObject.value(mmvar_gateY, -1);
		if (gateY <= 0) {
			cout << "processNewTcpConnection: Bad gateY: " << gateY << endl;
			//return false; //bad gateY
		}
		else gate_Y = gateY;

		//delete later
		cout << "processNewTcpConnection: gate_X: " << gate_X << endl;
		cout << "processNewTcpConnection: gate_Y: " << gate_Y << endl;

		MazePlayer *mazePlayerPtr;
		bool mazePlayerFound = false;
		for (int i = 0; i < maxPlayers; i++) {
			mazePlayerPtr = &mazePlayers[i];
			// valid only if playerID == clientID, and player state == playerState_broadCastAccepted
			if ( (mazePlayerPtr->playerID == clientID) && (mazePlayerPtr->playerState == playerState_accepted) ) {
				mazePlayerFound = true;
				break;
			}
		}

		if (!mazePlayerFound) {
			cout << "processNewTcpConnection: player with this ID not found: " << clientID << endl;
			return false; //bad client ID
		}

		if (debug) cout << "processNewTcpConnection: Player ID is: " << clientID << endl;
		//change the state of the client to tcpConnected ***also means player is in lobby***
		mazePlayerPtr->playerState = playerState_tcpConnected;
		mazePlayerPtr->currentX = playerX;
		mazePlayerPtr->currentY = playerY;
		mazePlayerPtr->tcpSocket = newSocket; //save this socket in the player object

		if (debug) cout << "processNewTcpConnection: Player index is: " << mazePlayerPtr->playerIndex << endl;
		//start a new thread to receive new move messages from this socket
		// start tcp receive thread

		//start connected client tread with player index
		_beginthread(connectedClientTcpThread, 0, &mazePlayerPtr->playerIndex);
		if (debug) cout << "processNewTcpConnection: thread connectedClientTcpThread started with player index " << mazePlayerPtr->playerIndex << endl;

		//check if all players are in tcpConnected state
		bool sendStartGameToPlayers = true;
		for (int i = 0; i < maxPlayers; i++) {
			mazePlayerPtr = &mazePlayers[i];
			bool tcpConnected = (mazePlayerPtr->playerState == playerState_tcpConnected);
			sendStartGameToPlayers &= tcpConnected;
			}

		//if sendStartGameToPlayers has remained true means all players
		//   are in tcpConnected state (in lobby). in this case,
		//   start the game by setting game and player state as started
		//then trigger multicast sendMulticast by mazePlayerPtr->sendMulticast after 1 second
		if (sendStartGameToPlayers == true) {

			if (debug) cout << "processNewTcpConnection: Sending Game started message..." << endl;

			Sleep(1000); //wait for 1 second and trigger sending multicast
			mazeState = mazeState_GameStarted;
			//set state of all players as playing and trigger multicast
			for (int i = 0; i < maxPlayers; i++) {
				mazePlayerPtr = &mazePlayers[i];
				mazePlayerPtr->playerState = playerState_playing;
				mazePlayerPtr->sendMulticast = true;
				}
		}

		return true;
	}
	catch(exception &ex) {
		cout << " xyz: exception: " << ex.what() << endl;
		return false;
	}
}

void checkAndQuitIfGameOver() {
	try {
		int playerPositionX[2] = {-1, -2};
		int playerPositionY[2] = {-1, -2};

		for (int i = 0; i < maxPlayers; i++) {
			if (mazePlayers[i].currentX <=0) return;
			if (mazePlayers[i].currentY <=0) return;

			playerPositionX[i] = mazePlayers[i].currentX;
			playerPositionY[i] = mazePlayers[i].currentY;
			}

		//if x and y coordinates of both player and opponent are same, means opponent has won - game over
		if ( (playerPositionX[0] == playerPositionX[1]) && (playerPositionY[0] == playerPositionY[1]) ) {
			cout << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << "*** Opponent Wins, Game Over ***" << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << endl;
			quitServerFlag = true;
		}

		//if x and y coordinates of player and gate are same, means player has won - game over
		if ( (playerPositionX[0] == gate_X) && (playerPositionY[0] == gate_Y) ) {
			cout << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << "*** Player Wins, Game Over ***" << endl;
			cout << "------------------------------------------------------------" << endl;
			cout << endl;
			quitServerFlag = true;
		}
	}
	catch(exception &ex) {
		cout << " checkAndQuitIfGameOver: exception: " << ex.what() << endl;
		return;
	}
}

bool processTcpPlayerMovementMessage(int playerIndex) {
	try {
		//get MazePlayer from mazePlayers --- get player at index
		MazePlayer *mazePlayerPtr = &mazePlayers[playerIndex];

		if (strlen(mazePlayerPtr->tcpReceiveBuffer) <= 0) {
			cout << "processTcpMessage: blank incoming json string" << endl;
		}

		json newTcpPlayerMovementJsonObject = stringToJsonObject(mazePlayerPtr->tcpReceiveBuffer);
		if (newTcpPlayerMovementJsonObject == NULL) {
			cout << "processTcpMessage: Bad json string" << endl;
			return false;
		}

		string messageType = newTcpPlayerMovementJsonObject.value(mazeMessageType, "no value");
		if (messageType != mmt_playerCoordinates) {
			cout << "processNewTcpConnection: message type not of type: " << mmt_playerCoordinates << endl;
			return false; //not mmt_playerTcpConnection
		}

		int clientID = newTcpPlayerMovementJsonObject.value(mmvar_clientId, -1);
		if ( (clientID <= 0) || (clientID != mazePlayerPtr->playerID) ) {
			cout << "processNewTcpConnection: Bad or non-matching client ID: " << clientID << endl;
			return false; //bad client ID
		}

		int newX = newTcpPlayerMovementJsonObject.value(mmvar_playerX, -1);
		int newY = newTcpPlayerMovementJsonObject.value(mmvar_playerY, -1);
		if ( (newX < 0) || (newY < 0) ) {
			cout << "processNewTcpConnection: Bad coordinates: " << clientID << endl;
			return false; //bad client ID
		}

		mazePlayerPtr->currentX = newX;
		mazePlayerPtr->currentY = newY;
		mazePlayerPtr->sendMulticast = true;

		checkAndQuitIfGameOver();

		return true;
	}
	catch(exception &ex) {
		cout << " xyz: exception: " << ex.what() << endl;
		return false;
	}

}
