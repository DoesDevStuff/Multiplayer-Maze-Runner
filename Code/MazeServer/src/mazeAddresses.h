#ifndef MAZEADDRESSES_H_
#define MAZEADDRESSES_H_

#include "common.h"

extern const char *broadcastAddressString;
extern const uint16_t broadcastPort;

//*** multicast address and port ***
extern const char *multicastAddressString;
extern const uint16_t multicastPort;

//---------------------------------------------------------
//*** tcp listening address and port ***
extern char tcpAddressString[1024];
extern const uint16_t tcpPort;

extern struct sockaddr_in tcpAddress;
extern struct sockaddr_in receivedFromAddress;

extern struct sockaddr_in multicastAddress;

#endif /* MAZEADDRESSES_H_ */
