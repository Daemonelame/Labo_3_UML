#include <stdio.h>
#include "SimulatorController.h"

#define ERROR_MESSAGE_TYPE				255

//////////////////////////////////////////////
// Message types
//////////////////////////////////////////////
// PTP commands
#define AXIS_MOVE_TO					0
#define AXIS_BRAKE						1

// PVT commands
#define ADD_PVT_VALUE_TO_AXIS			2
#define START_AXIS_PVT_MOVE				3
#define START_ALL_AXIS_PVT_MOVE			4
#define CLEAR_AXIS_PVT_TABLE			5
#define GET_AXIS_PVT_TABLE_SIZE			6
#define GET_PVT_BUFFER_STATE			7

// Error commands
#define IS_AXIS_ON_ERROR				8
#define GET_AXIS_ERROR_DESCRIPTION		9
#define ACKNOWLEDGE_AXIS_ERROR			10
#define RESET_SIMULATOR					11

// Global commands
#define IS_AXIS_MOVE_DONE				12
#define GET_AXIS_MAX_POSITION			13
#define GET_AXIS_MIN_POSITION			14
#define GET_AXIS_CURRENT_POSITION		15
#define GET_AXIS_POSITION_TO_REACH		16
#define SET_AXIS_TOLERANCE				17
#define GET_AXIS_TOLERANCE				18

// IO commands
#define GET_FLAGS						19
#define SET_FLAGS						20

void SimulatorInitialize(SIMULATOR_CONTROLLER * simulator, char* simulator_ip_address)
{	
	UdpInitializeConnection(&simulator->connection, simulator_ip_address);
}

void SimulatorFinalize(SIMULATOR_CONTROLLER * simulator)
{
	UdpTerminateConnection(&simulator->connection);
}

int SimulatorErrorOnConnection(SIMULATOR_CONTROLLER * simulator)
{
	return UdpErrorOnConnection(&simulator->connection);
}

const char* SimulatorGetConnectionErrorDescription(SIMULATOR_CONTROLLER * simulator)
{
	return UdpGetConnectionErrorDescription(&simulator->connection);
}

void SimulatorClearConnectionErrors(SIMULATOR_CONTROLLER * simulator)
{
	UdpClearConnectionError(&simulator->connection);
//	UdpSend(&simulator->connection, RESET_SIMULATOR, NULL, 0);
}

////////////////////////////////////
// PTP command functions
////////////////////////////////////
void SimulatorAxisMoveTo(SIMULATOR_CONTROLLER * simulator, char axisControllerID, double position)
{
	#pragma pack(push, 1)
	struct structData
	{
		UINT8 paramByte1;
		double paramDouble1;
	};
	#pragma pack(pop)	
	struct structData paramData;

	paramData.paramByte1 = axisControllerID;
	paramData.paramDouble1 = position;

	UdpSend(&simulator->connection, AXIS_MOVE_TO, &paramData, sizeof(paramData));
}

void SimulatorAxisBrake(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	UdpSend(&simulator->connection, AXIS_BRAKE, &axisControllerID, sizeof(axisControllerID));
}

////////////////////////////////////
// PT command functions
// PVT trajectory calculation not yet implemented
////////////////////////////////////

void SimulatorAddPTValueToAxis(SIMULATOR_CONTROLLER * simulator, char axisControllerID, double position, int relativeTimeInMs)
{
	#pragma pack(push, 1)
	struct structData
	{
		UINT8 paramByte1;
		double paramDouble1;
		double paramDouble2;
		int paramInt1;
	};
	#pragma pack(pop)	
	struct structData paramData;
	paramData.paramByte1 = axisControllerID;
	paramData.paramDouble1 = position;
	paramData.paramDouble2 = 0;
	paramData.paramInt1 = relativeTimeInMs;

	UdpSend(&simulator->connection, ADD_PVT_VALUE_TO_AXIS, &paramData, sizeof(paramData));
}

void SimulatorStartAxisPTMove(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	UdpSend(&simulator->connection, START_AXIS_PVT_MOVE, &axisControllerID, sizeof(axisControllerID));
}

void SimulatorStartAllAxisPTMove(SIMULATOR_CONTROLLER * simulator)
{
	UdpSend(&simulator->connection, START_ALL_AXIS_PVT_MOVE, NULL, 0);
}

void SimulatorClearAxisPTTable(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	UdpSend(&simulator->connection, CLEAR_AXIS_PVT_TABLE, &axisControllerID, sizeof(axisControllerID));
}

int SimulatorGetAxisPTTableSize(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	char axisPVTTableSize;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_PVT_TABLE_SIZE, &axisControllerID, sizeof(axisControllerID), &axisPVTTableSize, &receiveDataLength);	
	return axisPVTTableSize;
}

/////////////////////////////////////////////////////////
// Function returns:
//   0 : Tables are emtpy
//   1 : Tables are low
//   2 : Tables are normal
//   3 : Tables are full
char SimulatorGetPVTBufferState(SIMULATOR_CONTROLLER * simulator)
{
	char pvtBufferState;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_PVT_BUFFER_STATE, NULL, 0, &pvtBufferState, &receiveDataLength);	
	return pvtBufferState;
}


////////////////////////////////////
// Error command functions
////////////////////////////////////

/////////////////////////////////////////////////////////
// Function returns:
//   0 : No error on axis
//   1 : Error(s) on axis
char SimulatorIsAxisOnError(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	char axisOnError;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, IS_AXIS_ON_ERROR, &axisControllerID, sizeof(axisControllerID), &axisOnError, &receiveDataLength);
	if (axisOnError)
	{
		if (receiveDataLength == 1)
			axisOnError = 1;
		else if (UdpErrorOnConnection(&simulator->connection))
			axisOnError = 0;
	}
	return axisOnError;
}

/////////////////////////////////////////////////////////
// Function returns: The length of the string array
int SimulatorGetAxisErrorDescription(SIMULATOR_CONTROLLER * simulator, char axisControllerID, char* errorDescription)
{
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_ERROR_DESCRIPTION, &axisControllerID, sizeof(axisControllerID), errorDescription, &receiveDataLength);	
	return receiveDataLength;
}

void SimulatorAcknowledgeAxisError(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	if (SimulatorIsAxisOnError(simulator, axisControllerID))
		UdpSend(&simulator->connection, ACKNOWLEDGE_AXIS_ERROR, &axisControllerID, sizeof(axisControllerID));
}


////////////////////////////////////
// Global command functions
////////////////////////////////////
/////////////////////////////////////////////////////////
// Function returns:
//   0 : Axis move isn't done yet
//   1 : Axis move is done
char SimulatorIsAxisMoveDone(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	char moveDone;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, IS_AXIS_MOVE_DONE, &axisControllerID, sizeof(axisControllerID), &moveDone, &receiveDataLength);	
	return moveDone;
}

double SimulatorGetAxisMaxPosition(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	double positionMax;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_MAX_POSITION, &axisControllerID, sizeof(axisControllerID), &positionMax, &receiveDataLength);	
	return positionMax;
}

double SimulatorGetAxisMinPosition(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	double positionMin;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_MIN_POSITION, &axisControllerID, sizeof(axisControllerID), &positionMin, &receiveDataLength);	
	return positionMin;
}

double SimulatorGetAxisCurrentPosition(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	double currentPosition;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_CURRENT_POSITION, &axisControllerID, sizeof(axisControllerID), &currentPosition, &receiveDataLength);	
	return currentPosition;
}

double SimulatorGetAxisPositionToReach(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	double positionToReach;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_POSITION_TO_REACH, &axisControllerID, sizeof(axisControllerID), &positionToReach, &receiveDataLength);	
	return positionToReach;
}

void SimulatorSetAxisTolerance(SIMULATOR_CONTROLLER * simulator, char axisControllerID, double tolerance)
{
	#pragma pack(push, 1)
	struct structData
	{
		UINT8 paramByte1;
		double paramDouble1;
	};
	#pragma pack(pop)	
	struct structData paramData;
	paramData.paramByte1 = axisControllerID;
	paramData.paramDouble1 = tolerance;

	UdpSend(&simulator->connection, SET_AXIS_TOLERANCE, &paramData, sizeof(paramData));
}

double SimulatorGetAxisTolerance(SIMULATOR_CONTROLLER * simulator, char axisControllerID)
{
	double tolerance;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_AXIS_TOLERANCE, &axisControllerID, sizeof(axisControllerID), &tolerance, &receiveDataLength);	
	return tolerance;
}

////////////////////////////////////
// Flag access command functions
////////////////////////////////////
void SimulatorSetOutput(SIMULATOR_CONTROLLER * simulator, char portNumber, char value)
{
	#pragma pack(push, 1)
	struct structData
	{
		UINT8 paramByte1;
		UINT8 paramByte2;
	};
	#pragma pack(pop)	
	struct structData paramData;
	paramData.paramByte1 = portNumber;
	paramData.paramByte2 = value;

	UdpSend(&simulator->connection, SET_FLAGS, &paramData, sizeof(paramData));
}

char SimulatorGetInput(SIMULATOR_CONTROLLER * simulator, char portNumber)
{
	char outputFlags;
	int receiveDataLength;

	UdpSendReceive(&simulator->connection, GET_FLAGS, &portNumber, sizeof(portNumber), &outputFlags, &receiveDataLength);	
	return outputFlags;
}