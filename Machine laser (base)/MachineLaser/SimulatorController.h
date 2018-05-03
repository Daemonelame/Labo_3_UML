#ifndef __SIMULATOR_CONTROLLER_H_
#define __SIMULATOR_CONTROLLER_H_

#include "UDPConnection.h"

// Axes 
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

// Constants used for working with output flags
#define OUTPUTS_BYTE 0

#define OUTPUT_BIT_LASER 0
#define OUTPUT_BIT_NEW_PIECE 1
#define OUTPUT_BIT_LIGHT_RED 2
#define OUTPUT_BIT_LIGHT_ORANGE    3
#define OUTPUT_BIT_LIGHT_GREEN 4

// Constants used for working with input flags
#define INPUTS_BYTE 0

#define INPUT_BIT_BUTTON_GREEN 0
#define INPUT_BIT_BUTTON_ORANGE 1
#define INPUT_BIT_BUTTON_RED 2

typedef struct
{
    UDP_CONNECTION connection;
} SIMULATOR_CONTROLLER;

// Connection 
void SimulatorInitialize(SIMULATOR_CONTROLLER * machine, 
                         char* simulator_ip_address);
void SimulatorFinalize(SIMULATOR_CONTROLLER * machine);

// Communication error management
int SimulatorErrorOnConnection(SIMULATOR_CONTROLLER * machine);
const char* SimulatorGetConnectionErrorDescription(SIMULATOR_CONTROLLER * machine);
void SimulatorClearConnectionErrors(SIMULATOR_CONTROLLER * machine);

// PTP command functions
void SimulatorAxisMoveTo(SIMULATOR_CONTROLLER * machine, 
                         char axisControllerID, 
                         double position);
void SimulatorAxisBrake(SIMULATOR_CONTROLLER * machine, 
                        char axisControllerID);

// PT command functions
void SimulatorAddPTValueToAxis(SIMULATOR_CONTROLLER * machine, 
                               char axisControllerID, 
                               double position, 
                               int relativeTimeInMs);
void SimulatorStartAxisPTMove(SIMULATOR_CONTROLLER * machine, 
                              char axisControllerID);
void SimulatorStartAllAxisPTMove(SIMULATOR_CONTROLLER * machine);
void SimulatorClearAxisPTTable(SIMULATOR_CONTROLLER * machine, 
                               char axisControllerID);
int SimulatorGetAxisPTTableSize(SIMULATOR_CONTROLLER * machine, 
                                char axisControllerID);
char SimulatorGetPTBufferState(SIMULATOR_CONTROLLER * machine);

// Error command functions
char SimulatorIsAxisOnError(SIMULATOR_CONTROLLER * machine, 
                            char axisControllerID);
int SimulatorGetAxisErrorDescription(SIMULATOR_CONTROLLER * machine, 
                                     char axisControllerID, 
                                     char* errorDescription);
void SimulatorAcknowledgeAxisError(SIMULATOR_CONTROLLER * machine, 
                                   char axisControllerID);

// End move 
char SimulatorIsAxisMoveDone(SIMULATOR_CONTROLLER * machine, 
                             char axisControllerID);
void SimulatorSetAxisTolerance(SIMULATOR_CONTROLLER * machine, 
                               char axisControllerID, 
                               double tolerance);
double SimulatorGetAxisTolerance(SIMULATOR_CONTROLLER * machine, 
                                 char axisControllerID);

// Get position
double SimulatorGetAxisMaxPosition(SIMULATOR_CONTROLLER * machine, 
                                   char axisControllerID);
double SimulatorGetAxisMinPosition(SIMULATOR_CONTROLLER * machine, 
                                   char axisControllerID);
double SimulatorGetAxisCurrentPosition(SIMULATOR_CONTROLLER * machine, 
                                       char axisControllerID);
double SimulatorGetAxisPositionToReach(SIMULATOR_CONTROLLER * machine, 
                                       char axisControllerID);

// Inputs/Outputs
void SimulatorSetOutput(SIMULATOR_CONTROLLER * machine, 
                        char portNumber, 
                        char value);
char SimulatorGetInput(SIMULATOR_CONTROLLER * machine, 
                       char portNumber);

#endif