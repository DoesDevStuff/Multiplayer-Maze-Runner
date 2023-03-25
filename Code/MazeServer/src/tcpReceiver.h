#ifndef TCPRECEIVER_H_
#define TCPRECEIVER_H_

#include "common.h"
#include "buffers.h"

bool initializeTcpListener();
void tcpListenThread(void *param);
void connectedClientTcpThread(void *param);

#endif /* TCPRECEIVER_H_ */
