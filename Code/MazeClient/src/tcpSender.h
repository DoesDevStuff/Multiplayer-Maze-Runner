#ifndef TCPSENDER_H_
#define TCPSENDER_H_

#include "common.h"
//socket for tcp - to send player positions
extern SOCKET clientTcpSocket;

void tcpSenderThread(void *param);
bool initializeAndStartTcpSenderThread();


#endif /* TCPSENDER_H_ */
