#ifndef SOCKETUTILS_H_
#define SOCKETUTILS_H_

#include "common.h"
#include "buffers.h"

int bytesAvailableToReadFromSocket(SOCKET socketToCheck, int timeoutSeconds,  int timeoutMilliSeconds);
bool clearUdpReadDataFromSocket(SOCKET socket);

#endif /* SOCKETUTILS_H_ */
