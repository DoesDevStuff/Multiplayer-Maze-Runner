#ifndef BROADCASTSENDER_H_
#define BROADCASTSENDER_H_

#include "common.h"

//socket for broadcasting
extern SOCKET sendBroadcastSocket;

//variables to hold send and receive addresses
extern struct sockaddr_in sendBroadcastAddress;
extern struct sockaddr_in receiveAddress;
extern struct sockaddr_in receivedBroadcastFromAddress; // for the receiver to know from where the message came from
extern int sockAddressLength;

void sendBroadcastThread(void *param);
bool initializeBroadcastThread();

#endif /* BROADCASTSENDER_H_ */
