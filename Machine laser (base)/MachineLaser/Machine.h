/*********************************************************************************

Fichier 	:  	Machine.h
Auteur 		:	Huber Michael & Théo Tchoffo
Date		:	16.04.2018

*********************************************************************************/

#pragma once
#include <stdbool.h>
#include "SimulatorController.h"
#include "Utilities.h"
#include "Segment.h"

using namespace EngravingSystem;

#define M_PI 3.14159265359


class Machine
{
public:
	Machine(char * ip)
	{
		SimulatorInitialize(&simulator, ip);
	}
private:
	SIMULATOR_CONTROLLER simulator;
	unsigned char _outputs = 0;
	double _oldPosX = 0;
	double _oldPosY = 0;
	double _oldPosZ = 0;
	double _step = 1.0;

public:
	bool HasErrors(void);
	void DisplayErrors(void);
	void SetOutput(int output_bit, int state);
	void JogAxis(char axeControllerID);
	void ManualJog(void);
	void ChangeEngravedPiece(void);
	bool WaitMovementsDone(void);
	void ReinitializeSimulator(void);
	void AddPtToPvtAllAxis(double positionX, double positionY, double positionZ, int moveTime);
	void MovePtpAllAxis(double moveAxeX, double moveAxeY, double moveAxeZ);
	void PTPSequence(void);
	void PTSequence(void);
	double GetAxisCurrentPosition(char axisControllerID);
	void StartAllAxisPTMove(void);
	void SignalingError();

};
