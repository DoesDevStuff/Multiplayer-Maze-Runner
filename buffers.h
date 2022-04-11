#ifndef BUFFERS_H_
#define BUFFERS_H_

//maximum packet sizes
extern const int maxTcpPacketSize;
extern const int maxUdpPacketSize;

//tcp buffers and lengths of bytes filled
extern char tcpSendBuffer[];
extern char tcpReceiveBuffer[];
extern int tcpSendBufferLength;
extern int tcpReceiveBufferLength;

//udp buffers and lengths of bytes filled
extern char udpSendBuffer[];
extern char udpReceiveBuffer[];
extern int udpSendBufferLength;
extern int udpReceiveBufferLength;

//multicast buffers and lengths of bytes filled
extern char udpMulticastSendBuffer[];
extern char udpMulticastReceiveBuffer[];
extern int udpMulticastSendBufferLength;
extern int udpMulticastReceiveBufferLength;

#endif /* BUFFERS_H_ */
