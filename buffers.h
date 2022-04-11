#ifndef BUFFERS_H_
#define BUFFERS_H_

//maximum packet sizes
extern const int maxTcpPacketSize;
extern const int maxUdpPacketSize;

//tcp buffers and lengths of bytes filled
extern char tcpSendBuffer[];
extern char tcpReceiveBuffer[];

//udp buffers and lengths of bytes filled
extern char udpSendBuffer[];
extern char udpReceiveBuffer[];

//multicast buffers and lengths of bytes filled
extern char udpMulticastSendBuffer[];
extern char udpMulticastReceiveBuffer[];

#endif /* BUFFERS_H_ */
