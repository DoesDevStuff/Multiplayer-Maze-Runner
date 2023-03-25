
#include "mazeAddresses.h"

//*** Broadcast address and port ***
const char *broadcastAddressString = "255.255.255.255";
const uint16_t broadcastPort = 9091;

//*** multicast address and port ***
const char *multicastAddressString = "239.255.255.250";
const uint16_t multicastPort = 9092;

//*** tcp listening address and port ***
char tcpAddressString[1024]; //gets filled by initializeController()
const uint16_t tcpPort = 9093;

struct sockaddr_in tcpAddress;
struct sockaddr_in receivedFromAddress;

struct sockaddr_in multicastAddress;
