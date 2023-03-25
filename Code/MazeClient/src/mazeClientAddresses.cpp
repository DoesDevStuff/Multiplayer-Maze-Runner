#include "mazeClientAddresses.h"

int sockAddressLength = sizeof(struct sockaddr_in);

//*** Broadcast address and port ***
//Broadcast is sent to all local (Lan) addresses, on a particular port
const char *broadcastAddressString = "255.255.255.255";
const uint16_t broadcastPort = 9091;

//variables to hold broadcast addresses
struct sockaddr_in sendBroadcastAddress;
struct sockaddr_in receivedBroadcastFromAddress; //for the broadcast receiver to know from where the message came from

//*** tcp listening address and port ***
char tcpAddressString[1024]; //gets filled from server response
int tcpPort = 0;  //gets filled from server response

//*** multicast address and port ***
char multicastAddressString[1024]; //gets filled from server response
int multicastPort = 0; //gets filled from server response

struct sockaddr_in tcpAddress; //for sending

struct sockaddr_in multicastReceiveAddress;
struct sockaddr_in receivedMulticastFromAddress;
