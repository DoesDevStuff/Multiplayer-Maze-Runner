#ifndef MAZECLIENTADDRESSES_H_
#define MAZECLIENTADDRESSES_H_

#include "common.h"

extern int sockAddressLength;

//*** Broadcast address and port ***
//Broadcast is sent to all local (Lan) addresses, on a particular port
extern const char *broadcastAddressString;
extern const uint16_t broadcastPort;

//variables to hold broadcast addresses
extern struct sockaddr_in sendBroadcastAddress;
extern struct sockaddr_in receivedBroadcastFromAddress; //for the broadcast receiver to know from where the message came from

//*** tcp listening address and port ***
extern char tcpAddressString[]; //gets filled from server response
extern int tcpPort;  //gets filled from server response

//*** multicast address and port ***
extern char multicastAddressString[]; //gets filled from server response
extern int multicastPort; //gets filled from server response

extern struct sockaddr_in tcpAddress; //for sending

extern struct sockaddr_in multicastReceiveAddress;
extern struct sockaddr_in receivedMulticastFromAddress;

#endif /* MAZECLIENTADDRESSES_H_ */
