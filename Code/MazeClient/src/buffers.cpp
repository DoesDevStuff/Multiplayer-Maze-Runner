#include "buffers.h"

//maximum packet sizes
const int maxTcpPacketSize = 8192;
const int maxUdpPacketSize = 1408;

//tcp buffers and lengths of bytes filled
char tcpSendBuffer[maxTcpPacketSize];
char tcpReceiveBuffer[maxTcpPacketSize];
int tcpSendBufferLength;
int tcpReceiveBufferLength;

//udp buffers and lengths of bytes filled
char udpSendBuffer[maxUdpPacketSize];
char udpReceiveBuffer[maxUdpPacketSize];
int udpSendBufferLength;
int udpReceiveBufferLength;

//multicast buffers and lengths of bytes filled
char udpMulticastSendBuffer[maxUdpPacketSize];
char udpMulticastReceiveBuffer[maxUdpPacketSize];
int udpMulticastSendBufferLength;
int udpMulticastReceiveBufferLength;
