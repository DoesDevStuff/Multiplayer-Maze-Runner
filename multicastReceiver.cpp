
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

#include "mazeClientAddresses.h"
#include "buffers.h"
#include "multicastReceiver.h"
#include "MazeClientController.h"

//socket used receiving multicast
SOCKET multicastReceiveSocket;

//multicast receive thread
void receiveMulticastThread(void *param) {
	bool debug = true;

	while(1) {
		int bytesRead = recvfrom(multicastReceiveSocket, udpMulticastReceiveBuffer, maxUdpPacketSize, 0, (sockaddr *)&receivedMulticastFromAddress, &sockAddressLength);
		if (bytesRead < 0)
			continue;
		udpMulticastReceiveBuffer[bytesRead] = 0;

		if (debug) cout << "receiveMulticastThread: received From:" << inet_ntoa(receivedMulticastFromAddress.sin_addr)  << " port:" << ntohs(receivedMulticastFromAddress.sin_port) << " message:" << udpMulticastReceiveBuffer << endl;
		processMulticastMessage(udpMulticastReceiveBuffer);
	}
}

bool initializeAndStartMulticastReceiver() {
	multicastReceiveSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	char yes = 'y';

	//allow multiple sockets to use the same PORT number
	if (setsockopt(multicastReceiveSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
		perror("Error setting socket re-use port options, exiting...");
		closesocket(multicastReceiveSocket);
		return 1;
	}

	multicastReceiveAddress.sin_family = AF_INET;
	multicastReceiveAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	multicastReceiveAddress.sin_port = htons(multicastPort);

	cout << "multicastReceiveAddress IP:" << inet_ntoa(multicastReceiveAddress.sin_addr)  << " port:" << ntohs(multicastReceiveAddress.sin_port) << endl;

	if (bind(multicastReceiveSocket, (sockaddr*)&multicastReceiveAddress, sizeof(multicastReceiveAddress)) < 0) {
		perror("Error binding socket to receive address, exiting...");
		closesocket(multicastReceiveSocket);
		return 1;
	}

	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(multicastAddressString);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	//join multicast group
	if (setsockopt(multicastReceiveSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0) {
		perror("Error setting socket options to join multicast group, exiting...");
		closesocket(multicastReceiveSocket);
		return 2;
	}

	//start multicast receive thread
	_beginthread(receiveMulticastThread, 0, NULL);

	cout << "Started multicast receive thread..." << endl << endl;

	return true;
}
