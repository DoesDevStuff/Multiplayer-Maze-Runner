#include "common.h"
#include "mazeClientAddresses.h"
#include "broadcastSender.h"
#include "socketUtils.h"
#include "mazeClientState.h"
#include "MazeClientController.h"

//socket for broadcasting
SOCKET sendBroadcastSocket;

//global buffer to hold the joining request json string
const int maxJsonMessageSize = 1408;
char jsonStringBuffer[maxJsonMessageSize];

// thread for sending broadcast
void sendBroadcastThread(void *param)
{
	boolean debug = false;
	int broadcastRetryAttemptsCount = 0;
	int processBroadcastMessageResult = -1;

	while(1)
	{
		if (stopBroadcast) {
			Sleep(waitAfterBroadcast);
			continue;
		}

		// { send broadcast join message or retry only if not in calibration
		// processBroadcastMessageResult == 1 means in calibration
		if (processBroadcastMessageResult != 1) {

			if (broadcastRetryAttemptsCount > 0)
				Sleep(broadcastRetryInterval);

			++broadcastRetryAttemptsCount;

			if (broadcastRetryAttemptsCount > broadcastRetryAttempts) {
				cout << "Could not reach server, quitting..." << endl;
				stopBroadcast = true;
				quitClient = true;
				continue;
			}

			cout << "sendBroadcastThread: sending join request broadcast to server #" << broadcastRetryAttemptsCount << endl;
			int bytesSent = sendto(sendBroadcastSocket, jsonStringBuffer, strlen(jsonStringBuffer), 0, (sockaddr *)&sendBroadcastAddress, sizeof(sendBroadcastAddress));
			if (bytesSent < 0)
				cout << "sendBroadcastThread: error sending broadcast packet" << endl;

			Sleep(waitAfterBroadcast);
		}
		// { send broadcast message or retry only if not in calibration

		// reset processBroadcastMessageResult to -1
		processBroadcastMessageResult = -1;

		if (bytesAvailableToReadFromSocket(sendBroadcastSocket, 1,  0) <= 0) {
			continue;
		}

		//receive returned message from server
		int bytesRead = recvfrom(sendBroadcastSocket, udpReceiveBuffer, maxUdpPacketSize, 0, (sockaddr *)&receivedBroadcastFromAddress, &sockAddressLength);
		udpReceiveBuffer[bytesRead] = 0;

		if (debug) cout << "sendBroadcastThread: received from: " << inet_ntoa(receivedBroadcastFromAddress.sin_addr) << ", Port: " << ntohs(receivedBroadcastFromAddress.sin_port) << ", Message: " << udpReceiveBuffer << endl;

		//process incoming message
		processBroadcastMessageResult = processBroadcastMessage(udpReceiveBuffer, udpSendBuffer);

		if (processBroadcastMessageResult <= 0) {
			if (debug) cout << "sendBroadcastThread: no action to be done" << endl;
			continue;
		}

		if (processBroadcastMessageResult == 1) {
			if (debug) cout << "sendBroadcastThread: sending calibration message to caller as it is: " << udpReceiveBuffer << endl;
			sendto(sendBroadcastSocket, udpReceiveBuffer, strlen(udpReceiveBuffer), 0, (sockaddr *)&sendBroadcastAddress, sizeof(sendBroadcastAddress));
			//sendto(sendBroadcastSocket, udpReceiveBuffer, strlen(jsonStringBuffer), 0, (sockaddr *)&receivedBroadcastFromAddress, sizeof(receivedBroadcastFromAddress));
			continue;
		}

		if (processBroadcastMessageResult == 2) {
			cout << "sendBroadcastThread: player accepted" << endl;
		}
		else {
			// this situation should not occur
			continue;
		}

		//player accepted: set stopBroadcast flag to true to stop sending join message anymore
		stopBroadcast = true;
	}
}

bool initializeBroadcastThread()
{
	json jsonJoinMessage;

	char yes = 'y'; //flag: any non-zero value is "true"

	// initialize send broadcast socket and set options for broadcast
	sendBroadcastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (setsockopt(sendBroadcastSocket, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) < 0)
	{
		cout << "Error setting socket broadcast options, exiting..." << endl;
		closesocket(sendBroadcastSocket);
		return false;
	}

	//initialize send addresses
	sendBroadcastAddress.sin_family = AF_INET;
	sendBroadcastAddress.sin_addr.s_addr = inet_addr(broadcastAddressString);
	sendBroadcastAddress.sin_port = htons(broadcastPort);

	//create brodcast json message to sent to server
	// add a Boolean that is stored as bool
	jsonJoinMessage[mazeMessageType] = mmt_newPlayerJoinRequest;
	string messageType = jsonJoinMessage.value(mazeMessageType, "message type not found");
	cout << "messageType: " << messageType << std::endl;

	//convert json message object to string
	jsonObjectToString(jsonJoinMessage, jsonStringBuffer);
    cout << "jsonStringBuffer: " << jsonStringBuffer << endl;

	return true;
}
