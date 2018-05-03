#include "UDPConnection.h"

#define UDP_SERVER_PORT					14884
#define DATA_BUFFER_SIZE				128
#define BASE_MESSAGE_LENGTH				4
#define CONNECTION_TIMEOUT				500 //[ms]

#define CONERROR_NONE				"No error on connection"
#define CONERROR_SEND_TIMEOUT		"Send method timed out too many times (see MAX_SEND_ATTEMPTS)!"
#define CONERROR_RECEIVE_TIMEOUT	"Receive method timed out too many times (see MAX_RECEIVE_ATTEMPTS)!"
#define CONERROR_TIMEOUT			"Connection timed out!"
#define CONERROR_SOCKET				"Socket error while waiting for a message!"
#define CONERROR_BAD_CHECKSUM		"Bad Checksum on message!"

const int MAX_SEND_ATTEMPTS = 3;
const int MAX_RECEIVE_ATTEMPTS = 3;

static WSADATA Info; // Windows Specific
static int open_connections = 0;

UINT8 UdpChecksum(UINT8 * buffer, int length)
{
	UINT8 checkSum = 0;
	int i;
	for (i = 0; i < (length - 1); i++)
	{
		checkSum += buffer[i];
	}
	return checkSum;
}

int UdpWaitForReception(UDP_CONNECTION * connection, long maxMillisecondsTime)
{
    struct timeval waitingTime;
    fd_set fdSetReception;
    int status;

    // Initializing waiting time 
    waitingTime.tv_sec = maxMillisecondsTime / 1000; 
    waitingTime.tv_usec= (maxMillisecondsTime % 1000) * 1000;
    // Initializing looked up socket list initializing
    FD_ZERO (&fdSetReception);
    FD_SET (connection->ClientSocket, &fdSetReception);
    // Temporized waiting of a message
    status = select((int)connection->ClientSocket, &fdSetReception, NULL, NULL, &waitingTime);
    if (status == 0) // No message received
		return 0;
    else if (status == SOCKET_ERROR)
    {
        connection->ConnectionError = 1;
		connection->ConnectionErrorDescription = CONERROR_SOCKET;
        return 0;
    }
    else
        return 1; // Message received
}

void UdpClearReceptionBuffer(UDP_CONNECTION * connection)
{
    char message[DATA_BUFFER_SIZE];
    int length;
    struct sockaddr_in address;
    while (UdpWaitForReception(connection, 0))
	{
        recvfrom(connection->ClientSocket, message, DATA_BUFFER_SIZE, 0,
			(struct sockaddr *)&address, &length);
	}
}

void UdpSendMessage(UDP_CONNECTION * connection, char commandNumber, void* data, int dataLength)
{
	int attempts = 0;
	int status = 0;
	int i, messageLength;
	UINT8 message[DATA_BUFFER_SIZE];

	messageLength = dataLength + BASE_MESSAGE_LENGTH;
	message[0] = commandNumber;
	message[1] = connection->MessageCount++;
	message[2] = dataLength;
	for(i = 0; i < dataLength; i++)
		message[i+3] = ((UINT8*)data)[i];

	message[messageLength - 1] = UdpChecksum(message, messageLength);

	if (!connection->ConnectionError)
	{
		UdpClearReceptionBuffer(connection);
		
		while (attempts < MAX_SEND_ATTEMPTS)
		{
			attempts++;
			status = sendto(connection->ClientSocket, (char*)message, messageLength, 0, (struct sockaddr*)&connection->ServerAddress, sizeof(connection->ServerAddress));
			if (status)
				break;
		}
		if (attempts >= MAX_SEND_ATTEMPTS)
		{
			connection->ConnectionError = 1;
			connection->ConnectionErrorDescription = CONERROR_SEND_TIMEOUT;
		}
	}
}

int UdpReceiveMessage(UDP_CONNECTION * connection, void * receiveData, int* receiveDataLength)
{
	int addressOriginalSize;
	int status, i;
	UINT8 reception[DATA_BUFFER_SIZE];

	if (UdpWaitForReception(connection, CONNECTION_TIMEOUT))
	{
		addressOriginalSize = sizeof connection->ServerAddress;
		status = recvfrom(connection->ClientSocket, (char*)reception, DATA_BUFFER_SIZE, 0, 
				(struct sockaddr*)&connection->ServerAddress, &addressOriginalSize);
		if (UdpChecksum(reception, status) == reception[status - 1])
		{
			if ((receiveData != NULL) && (receiveDataLength != NULL))
			{
				*receiveDataLength = (int)reception[2];
				for (i = 0; i < reception[2]; i++)
					((UINT8*)receiveData)[i] = reception[i + 3];
			}
			return 1;
		}
		else
		{
			connection->ConnectionError = 1;
			connection->ConnectionErrorDescription = CONERROR_BAD_CHECKSUM;
			return 2;
		}
	}
	else
		return 0;
}

void UdpSend(UDP_CONNECTION * connection, char commandNumber, void* data, int dataLength)
{
	UdpSendReceive(connection, commandNumber, (void*)data, dataLength, NULL, NULL);
}

void UdpSendReceive(UDP_CONNECTION * connection, char commandNumber, void* data, int dataLength, void* receiveData, int* receiveDataLength)
{
	int attempts = 0;

	if (!connection->ConnectionError)
	{
		while (attempts < MAX_RECEIVE_ATTEMPTS)
		{
			attempts++;
			UdpSendMessage(connection, commandNumber, (void*)data, dataLength);
			if (UdpReceiveMessage(connection, receiveData, receiveDataLength))
				break;
		}
		if (attempts >= MAX_RECEIVE_ATTEMPTS)
		{
			if (receiveDataLength != NULL)
				*receiveDataLength = 0;
			connection->ConnectionError = 1;
			connection->ConnectionErrorDescription = CONERROR_RECEIVE_TIMEOUT;
		}
	}
}

void UdpInitializeConnection(UDP_CONNECTION * connection, char* simulator_ip_address)
{
	if (open_connections == 0)
		WSAStartup(MAKEWORD(2, 2), &Info);    

	open_connections++;
    connection->ClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	connection->ConnectionError = 0;
	connection->ConnectionErrorDescription = CONERROR_NONE;
	connection->MessageCount = 0;

	memset(&connection->ServerAddress, sizeof(struct sockaddr_in), 0);
    connection->ServerAddress.sin_family = AF_INET;
    connection->ServerAddress.sin_port = htons(UDP_SERVER_PORT); 
    connection->ServerAddress.sin_addr.s_addr = inet_addr(simulator_ip_address);
}

void UdpTerminateConnection(UDP_CONNECTION * connection)
{
	open_connections--;
	if (open_connections == 0)
		WSACleanup();
}

int UdpErrorOnConnection(UDP_CONNECTION * connection)
{
	return connection->ConnectionError;
}

const char* UdpGetConnectionErrorDescription(UDP_CONNECTION * connection)
{
	return connection->ConnectionErrorDescription;
}

void UdpClearConnectionError(UDP_CONNECTION * connection)
{
	connection->ConnectionError = 0;
	connection->ConnectionErrorDescription = CONERROR_NONE;
}