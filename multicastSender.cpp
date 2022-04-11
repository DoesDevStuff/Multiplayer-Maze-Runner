#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
#include <Iphlpapi.h> // must be after WinSock2.h
#include <Windows.h>
#include <process.h>
#include <conio.h>

#include <iostream>
#include <ctime>
#include <string>
#include "multicastSender.h"
#include "mazeAddresses.h"
#include "buffers.h"
#include "mazeServerState.h"
#include "mazeMessages.h"

using namespace std;

//global variables to hold date and time
const int dateTimeArraySize = 16;
char currentDate[dateTimeArraySize];
char currentTime[dateTimeArraySize];

//socket used for multicasting
SOCKET multicastSenderSocket;

bool createMulticastMessage(MazePlayer *mazePlayerPtr, char *multicastSendBuffer) {
	json multicastJson;
	multicastJson[mazeMessageType] = mmt_gameAndPlayerStatus;
	multicastJson[mmvar_gameState] = mazeState;
	multicastJson[mmvar_clientId] = mazePlayerPtr->playerID;
	multicastJson[mmvar_isPlayer] = mazePlayerPtr->isPlayer;
	multicastJson[mmvar_playerX] = mazePlayerPtr->currentX;
	multicastJson[mmvar_playerY] = mazePlayerPtr->currentY;
	multicastJson[mmvar_playerState] = mazePlayerPtr->playerState;

	jsonObjectToString(multicastJson, multicastSendBuffer);

	return true;
}

//sends multicast udp to clients
void multicastSenderThread(void *param) {
	while(true) {
		for (int i=0; i < maxPlayers; i++) {
			MazePlayer *mazePlayerPtr = &mazePlayers[i];

			if (!mazePlayerPtr->sendMulticast)
				continue;

			mazePlayerPtr->sendMulticast = false;

			if (!createMulticastMessage(mazePlayerPtr, mazePlayerPtr->multicastSendBuffer))
				continue;

			int bytesSent = sendto(multicastSenderSocket, mazePlayerPtr->multicastSendBuffer, strlen(mazePlayerPtr->multicastSendBuffer)+1, 0, (sockaddr *)&multicastAddress, sizeof(multicastAddress));
			if (bytesSent < 0)
				cout << "multicastSenderThread: Error sending multicast packet" << endl;

			cout << "multicastSenderThread: multicastMessage sent: " << mazePlayerPtr->multicastSendBuffer << endl;
		}

	}
}

bool initializeMulticastSender() {
//	{ delete later
	char *forPrintingAddress;
	int portForPrinting;
	char bufferForPrintingPort[10];
//	} delete later

	multicastSenderSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	multicastAddress.sin_family = AF_INET;
	multicastAddress.sin_addr.s_addr = inet_addr(multicastAddressString);
	multicastAddress.sin_port = htons(multicastPort);

//	{ delete later
	forPrintingAddress = inet_ntoa(multicastAddress.sin_addr);
	portForPrinting = (int) ntohs(multicastAddress.sin_port);
	itoa(portForPrinting, (char *)bufferForPrintingPort, sizeof(bufferForPrintingPort));
	cout << "sendToAddress IP:" << forPrintingAddress  << " port:" << bufferForPrintingPort << endl;
//	} delete later

	uint8_t ttl = 1;
	if (setsockopt(multicastSenderSocket, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl, sizeof(ttl)) < 0) {
		cout << "Error setting TTL" << endl;
		closesocket(multicastSenderSocket);
		return false;
	}

	cout << "initializeTcpListener: launching tcpListenThread" << endl;
	_beginthread(multicastSenderThread, 0, NULL);

	return true;
}
