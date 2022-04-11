#ifndef MAZECLIENTCONTROLLER_H_
#define MAZECLIENTCONTROLLER_H_

#include "common.h"

int processBroadcastMessage(char *broadcastMessage, char *returnBroadcastMessage);
bool processMulticastMessage(char *multicastReceiveBuffer);

#endif /* MAZECLIENTCONTROLLER_H_ */
