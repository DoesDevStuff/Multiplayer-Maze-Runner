
#ifndef BROADCASTLISTENER_H_
#define BROADCASTLISTENER_H_

#include "common.h"
#include "mazeAddresses.h"
#include "buffers.h"

//sockets
extern SOCKET receiveBroadcastSocket;
extern SOCKET broadcastResponseSocket;

extern struct sockaddr_in receiveAddress;
extern struct sockaddr_in receivedBroadcastFromAddress;
extern int sockAddressLength;

void receiveBroadcastThread(void *param);

bool initializeMazeBroadcast();

#endif /* BROADCASTLISTENER_H_ */
