#ifndef MULTICASTSENDER_H_
#define MULTICASTSENDER_H_

#include "common.h"
#include "buffers.h"

void multicastSenderThread(void *param);
bool initializeMulticastSender();

#endif /* MULTICASTSENDER_H_ */
