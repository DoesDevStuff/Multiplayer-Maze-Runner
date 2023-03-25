#include "socketUtils.h"

int bytesAvailableToReadFromSocket(SOCKET socketToCheck, int timeoutSeconds,  int timeoutMilliSeconds) {
	if (socketToCheck <= 0)
		return -1;

	if (timeoutSeconds < 0)
		timeoutSeconds = 0;

	if (timeoutMilliSeconds < 0)
		timeoutMilliSeconds = 0;

	//number of seconds to wait for reply
	timeval timeout;
	timeout.tv_sec = timeoutSeconds;
	timeout.tv_usec = timeoutMilliSeconds*1000;;

	//set of sockets
	fd_set socketSet;
	FD_ZERO(&socketSet);
	FD_SET(socketToCheck, &socketSet);

	//Return value: -1: error, 0: timed out, > 0: data ready to be read
	int bytesAvailable = select(0, &socketSet, 0, 0, &timeout);
	return bytesAvailable;
}

bool clearUdpReadDataFromSocket(SOCKET socket) {
	char tempbuffer[maxUdpPacketSize];
	struct sockaddr_in tempFromAddress;
	int tempSockAddressLength = sizeof(struct sockaddr_in);

	while (bytesAvailableToReadFromSocket(socket, 0,  0) > 0) {
		recvfrom(socket, tempbuffer, maxUdpPacketSize, 0, (sockaddr *)&tempFromAddress, &tempSockAddressLength);
	}

	return true;
}
