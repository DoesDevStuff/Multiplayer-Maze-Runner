#include "broadcastListener.h"
#include "buffers.h"
#include "mazeServerController.h"
#include "socketUtils.h"

//sockets
SOCKET receiveBroadcastSocket;
SOCKET broadcastResponseSocket;

//global variables to hold send and receive addresses
//struct sockaddr_in sendAddress;
//TODO: rename and move to mazeAddresses.cpp/.h
struct sockaddr_in receiveAddress;
struct sockaddr_in receivedBroadcastFromAddress;// where broadcast came from - filled by receiveBroadcastThread function
int sockAddressLength = sizeof(struct sockaddr_in);

//Broadcast receive Thread - looks for "new player joining" message
void receiveBroadcastThread(void *param) {
	bool debug = false;
	while(true) {
		if (quitServerFlag ==true) { cout << "receiveBroadcastThread: Quitting" << endl; return; }

		//check if any data is available on receiveBroadcastSocket
		int available = bytesAvailableToReadFromSocket(receiveBroadcastSocket, 0,  10);

		if (quitServerFlag ==true) { cout << "receiveBroadcastThread: Quitting" << endl; return; }

		//if calibration is taking too long, or client not responding, reset calibration
		if (mazeCalib.calibrationInProgress) {
			long currentMillisec = getCurrentMillisec();
			if ( (currentMillisec - mazeCalib.startMillisec) >= mazeCalib.maximumMillisecToPass ) {
				cout << "receiveBroadcastThread: resetting calibration: timeout" << endl;
				resetCalibStruct();
			}
		}

		//if no data is available on receiveBroadcastSocket, keep looping
		if (available <=0)
			continue;

		//receive broadcast message - also gets from where the broadcast came from (receivedBroadcastFromAddress)
		int bytesRead = recvfrom(receiveBroadcastSocket, udpReceiveBuffer, maxUdpPacketSize, 0, (sockaddr *)&receivedBroadcastFromAddress, &sockAddressLength);
		if (bytesRead <= 0)
			continue;

		udpReceiveBuffer[bytesRead] = 0;
		if (debug) cout << "received from: " << inet_ntoa(receivedBroadcastFromAddress.sin_addr) << ", Port: " << ntohs(receivedBroadcastFromAddress.sin_port) << ", Message: " << udpReceiveBuffer << endl;

		//process the incoming message (in mazeServerController), returns true if a message is to be sent back
		if (!processBroadcastMessage(udpReceiveBuffer, udpSendBuffer, receivedBroadcastFromAddress)) {
			if (debug) cout << "receiveBroadcastThread: nothing to return" << endl;
			continue;
		}

		if (debug) cout << "receiveBroadcastThread: Sending response to join request: " << udpSendBuffer << endl;
		sendto(broadcastResponseSocket, udpSendBuffer, strlen(udpSendBuffer)+1, 0, (sockaddr *)&receivedBroadcastFromAddress, sizeof(receivedBroadcastFromAddress));
	}
}

bool initializeMazeBroadcast() {
	char yes = 'y'; //flag: any non-zero value is "true"

	// set receive broadcast socket as UDP, and set broadcast re-use port options
	receiveBroadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (setsockopt(receiveBroadcastSocket, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) < 0)
	{
		cout << "Error setting socket broadcast option" << endl;
		closesocket(receiveBroadcastSocket);
		return false;
	}

	if (setsockopt(receiveBroadcastSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
	{
		cout << "Error setting socket SO_REUSEADDR option" << endl;
		closesocket(receiveBroadcastSocket);
		return false;
	}

	//since broadcast is sent to sent to *all* IP addresses on the local network on a specific port
	//so set receiver IP address as INADDR_ANY, and port as the broadcast port
	receiveAddress.sin_family = AF_INET;
	receiveAddress.sin_port = htons(broadcastPort);
	receiveAddress.sin_addr.s_addr = INADDR_ANY; // note: INADDR_ANY

	//bind the broadcast receiver socket to the above address, now this socket is all set to receive broadcasts

	//int bind (int socket, struct sockaddr *addr, socklen_t length)
	if ( bind(receiveBroadcastSocket, (sockaddr*)&receiveAddress, sizeof(receiveAddress)) < 0)
	{
		cout << "Error binding socket to receive address, exiting..." << endl;
		closesocket(receiveBroadcastSocket);
		return false;
	}

	// set response socket as UDP
	broadcastResponseSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// start broadcast receive thread
	_beginthread(receiveBroadcastThread, 0, NULL);

	cout << "Maze broadcast receiver started" << endl;

	return true;
}
