#ifndef __UDP_CONNECTION_H_
#define __UDP_CONNECTION_H_

#include <winsock.h>

typedef unsigned char UINT8;

typedef struct
{
	SOCKET ClientSocket;
	struct sockaddr_in ServerAddress;
	int ConnectionError;
	char* ConnectionErrorDescription;
	UINT8 MessageCount;
} UDP_CONNECTION;

void UdpInitializeConnection(UDP_CONNECTION * connection, char* simulator_ip_address);
void UdpTerminateConnection(UDP_CONNECTION * connection);

void UdpSend(UDP_CONNECTION * connection, char command, void * data, int dataLength);
void UdpSendReceive(UDP_CONNECTION * connection, char commandNumber, void* data, int dataLength, void* receiveData, int* receiveDataLength);

int UdpErrorOnConnection(UDP_CONNECTION * connection);
const char* UdpGetConnectionErrorDescription(UDP_CONNECTION * connection);

void UdpClearConnectionError(UDP_CONNECTION * connection);

#endif