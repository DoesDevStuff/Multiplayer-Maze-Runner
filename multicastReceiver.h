#ifndef MULTICASTRECEIVER_H_
#define MULTICASTRECEIVER_H_

#include "common.h"

void receiveMulticastThread(void *param);
bool initializeAndStartMulticastReceiver();

#endif /* MULTICASTRECEIVER_H_ */
