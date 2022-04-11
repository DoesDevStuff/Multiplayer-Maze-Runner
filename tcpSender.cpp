#include "tcpSender.h"
#include "mazeClientAddresses.h"
#include "mazeMessages.h"
#include "mazeClientState.h"

//socket for tcp - to send player positions
SOCKET clientTcpSocket;

bool createTcpJoinMessage(char *jsonStringBuffer) {
	json tcpJoinJson;
	tcpJoinJson[mazeMessageType] = mmt_playerTcpConnection;
	tcpJoinJson[mmvar_clientId] = mazePlayerID;

	if (amIPlayer) {
		tcpJoinJson[mmvar_playerX] = playerX;
		tcpJoinJson[mmvar_playerY] = playerY;
	}
	else {
		tcpJoinJson[mmvar_playerX] = opponentX;
		tcpJoinJson[mmvar_playerY] = opponentY;
	}

	tcpJoinJson[mmvar_gateX] = gate_X;
	tcpJoinJson[mmvar_gateY] = gate_Y;

	jsonObjectToString(tcpJoinJson, jsonStringBuffer);

	return true;
}

bool createTcpSendXYMessage(char *jsonStringBuffer) {
	json tcpXySenderJson;
	tcpXySenderJson[mazeMessageType] = mmt_playerCoordinates;
	tcpXySenderJson[mmvar_clientId] = mazePlayerID;
	tcpXySenderJson[mmvar_playerX] = newLocalX;
	tcpXySenderJson[mmvar_playerY] = newLocalY;

	jsonObjectToString(tcpXySenderJson, jsonStringBuffer);

	return true;
}

void tcpSenderThread(void *param) {
	cout << "tcpSenderThread: sending initial tcp join message..." << endl;

	int sleepAfterSend = 40;

	createTcpJoinMessage(tcpSendBuffer);
    cout << "tcpSenderThread: Json tcp join message: " << tcpSendBuffer << endl;

	int bytesSent = send(clientTcpSocket, tcpSendBuffer, strlen(tcpSendBuffer), 0);
	if (bytesSent == SOCKET_ERROR) {
		cout << "error sending message, exiting sendTcpThread..." << endl;
		return;
	}
	Sleep(sleepAfterSend);

	mazeClientState = mazeClientState_Connected;

	while(true) {
		if (!sendNewPositionsToserver) //send only if sendNewPositionsToserver is true
			continue;

		sendNewPositionsToserver = false;
		createTcpSendXYMessage(tcpSendBuffer);

		cout << "sendTcpThread: Sending position update message to server: " << tcpSendBuffer << endl;
		int bytesSent = send(clientTcpSocket, tcpSendBuffer, strlen(tcpSendBuffer), 0);
		if (bytesSent == SOCKET_ERROR)
			cout << "error sending position update message to server..." << endl;
		Sleep(sleepAfterSend);
	}
}

bool initializeAndStartTcpSenderThread() {
	clientTcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//fill tcpaddress
	tcpAddress.sin_family = AF_INET;
	tcpAddress.sin_addr.s_addr = inet_addr(tcpAddressString);
	tcpAddress.sin_port = htons(tcpPort);

	if (connect(clientTcpSocket, (SOCKADDR *) &tcpAddress, sizeof(tcpAddress)) != 0) {
         cout << "client connection failed" << endl;
         closesocket(clientTcpSocket);
         return false;
    }

    cout << "Connected to server, launching sendTcpThread" << endl;
	Sleep(100);
	_beginthread(tcpSenderThread, 0, NULL);

	return true;
}
