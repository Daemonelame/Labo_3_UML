/*********************************************************************************

Fichier 	:  	Machine.cpp
Auteur 		:	Huber Michael & Théo Tchoffo
Date		:	16.04.2018

*********************************************************************************/

#include "ApplicationMachine.h"
#include "Machine.h"
#include <string>
#include <ctime>
#include <iostream>

using namespace std;
using namespace EngravingSystem;


bool Machine::HasErrors(void)
{
	return SimulatorErrorOnConnection(&simulator) ||
		   SimulatorIsAxisOnError(&simulator, AXIS_X) ||
		   SimulatorIsAxisOnError(&simulator, AXIS_Y) ||
		   SimulatorIsAxisOnError(&simulator, AXIS_Z);
}

void Machine::DisplayErrors(void)
{
	cout << "Error diagnostic : " << endl;
	if (SimulatorErrorOnConnection(&simulator))
	{
		cout << "- Could not communicate with simulator." << endl;
		cout << "  %s\n", SimulatorGetConnectionErrorDescription(&simulator);
	}
	if (SimulatorIsAxisOnError(&simulator, AXIS_X))
		cout << "- Axis X in error." << endl;
	if (SimulatorIsAxisOnError(&simulator, AXIS_Y))
		cout << "- Axis Y in error." << endl;
	if (SimulatorIsAxisOnError(&simulator, AXIS_Z))
		cout << "- Axis Z in error." << endl;
}

void Machine::SetOutput(int output_bit, int state)
{
	if (state)
		_outputs |= 1 << output_bit;
	else
		_outputs &= ~(1 << output_bit);
	SimulatorSetOutput(&simulator, OUTPUTS_BYTE, _outputs);
}

void Machine::JogAxis(char axeControllerID)
{
	
	double targetPosition = SimulatorGetAxisPositionToReach(&simulator, axeControllerID) + _step;

	char * saturatedMessage = NULL;
	if (_step < 0)
	{
		double axisMinPosition = SimulatorGetAxisMinPosition(&simulator, axeControllerID);
		if (targetPosition < axisMinPosition)
		{
			targetPosition = axisMinPosition;
			saturatedMessage = "minimal";
		}
	}
	else
	{
		double axisMaxPosition = SimulatorGetAxisMaxPosition(&simulator, axeControllerID);
		if (targetPosition > axisMaxPosition)
		{
			targetPosition = axisMaxPosition;
			saturatedMessage = "maximal";
		}
	}

	if (saturatedMessage != NULL)
	{
		cout << Utils::GetAxesName(axeControllerID);
		cout << " : Position " << saturatedMessage << " atteinte." << endl;
	}

	SimulatorAxisMoveTo(&simulator, axeControllerID, targetPosition);
}

void Machine::ManualJog(void)
{
	const double StepMin = 0.01;
	const double StepMax = 10.0;

	bool EtatLaser = false;
	SetOutput(OUTPUT_BIT_LIGHT_ORANGE, 1);

	cout << "==== Deplacement manuel du robot ====" << endl;
	cout << "+ : Augmentation du pas de deplacement" << endl;
	cout << "- : Diminution du pas de deplacement" << endl;
	cout << "s : Deplacement X a gauche" << endl;
	cout << "d : Deplacement X a droite" << endl;
	cout << "f : Deplacement Y en arriere" << endl;
	cout << "v : Deplacement Y en avant" << endl;
	cout << "t : Deplacement Z en haut" << endl;
	cout << "g : Deplacement Z en bas" << endl;
	cout << "l : activation / desactivation du laser" << endl;
	cout << "q : Quitter le mode deplacement manuel" << endl << endl;
	cout << "Pas actuel : " << _step << endl;

	char key;
	do
	{
		key = _getch();

		switch (key)
		{
		case '+':
			if (_step < StepMax)
			{
				_step *= 2;
				if (_step > StepMax)
					_step = StepMax;

				cout << "Pas actuel : " << _step << (_step == StepMax ? "(max)" : "") << endl;
			}
			break;
		case '-':
			if (_step > StepMin)
			{
				_step /= 2;
				if (_step < StepMin)
					_step = StepMin;

				cout << "Pas actuel : " << _step << (_step == StepMin ? "(min)" : "") << endl;
			}
			break;
		case 's': 
			_step < 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_X);  
			break;
		case 'd': 
			_step > 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_X); 
			break;
		case 'f':
			_step < 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_Y);  
			break;
		case 'v': 
			_step > 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_Y); break;
		case 'g':
			_step < 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_Z);  
			break;
		case 't': 
			_step > 0 ? _step = (-1)*_step : _step;
			JogAxis(AXIS_Z); 
			break;
		case 'l':
			EtatLaser = !EtatLaser;
			SetOutput(OUTPUT_BIT_LASER, EtatLaser);
			break;
		}
		if (HasErrors())
			DisplayErrors();
	} while (key != 'q');
	SetOutput(OUTPUT_BIT_LIGHT_ORANGE, 0);
	SetOutput(OUTPUT_BIT_LASER, 0);
}

void Machine::ChangeEngravedPiece(void)
{
	// Changing piece works on rising edge
	SetOutput(OUTPUT_BIT_NEW_PIECE, 1);
	SetOutput(OUTPUT_BIT_NEW_PIECE, 0);
}

bool Machine::WaitMovementsDone(void)
{
	while (!(SimulatorIsAxisMoveDone(&simulator, AXIS_X) &
			 SimulatorIsAxisMoveDone(&simulator, AXIS_Y) &
			 SimulatorIsAxisMoveDone(&simulator, AXIS_Z)))
	{
		if (HasErrors())
			return false;
		Sleep(10);
	}
	return true;
}

void Machine::ReinitializeSimulator(void)
{
	Sleep(500);
	SimulatorAcknowledgeAxisError(&simulator, AXIS_X);
	SimulatorAcknowledgeAxisError(&simulator, AXIS_Y);
	SimulatorAcknowledgeAxisError(&simulator, AXIS_Z);
	SimulatorAxisMoveTo(&simulator, AXIS_X, 0);
	SimulatorAxisMoveTo(&simulator, AXIS_Y, 0);
	SimulatorAxisMoveTo(&simulator, AXIS_Z, 0);
	WaitMovementsDone();
	Sleep(500);
}

void Machine::AddPtToPvtAllAxis(double positionX, double positionY, double positionZ, int moveTime)
{
	SimulatorAddPTValueToAxis(&simulator, AXIS_X, positionX - _oldPosX, moveTime);
	SimulatorAddPTValueToAxis(&simulator, AXIS_Y, positionY - _oldPosY, moveTime);
	SimulatorAddPTValueToAxis(&simulator, AXIS_Z, positionZ - _oldPosZ, moveTime);
	_oldPosX = positionX;
	_oldPosY = positionY;
	_oldPosZ = positionZ;
}

void Machine::MovePtpAllAxis(double moveAxeX, double moveAxeY, double moveAxeZ)
{
	SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
	SimulatorAxisMoveTo(&simulator, AXIS_Y, moveAxeY);
	SimulatorAxisMoveTo(&simulator, AXIS_Z, moveAxeZ);
	_oldPosX = moveAxeX;
	_oldPosY = moveAxeY;
	_oldPosZ = moveAxeZ;
}

void Machine::PTPSequence(void)
{
	int i;
	double moveAxeX, moveAxeY, moveAxeZ;

	SetOutput(OUTPUT_BIT_LIGHT_GREEN, 1);
	ReinitializeSimulator();
	ChangeEngravedPiece();

	// Move to start position
	moveAxeX = 50;
	moveAxeY = -30;
	moveAxeZ = 40;
	SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
	SimulatorAxisMoveTo(&simulator, AXIS_Y, moveAxeY);
	SimulatorAxisMoveTo(&simulator, AXIS_Z, moveAxeZ);
	WaitMovementsDone();
	Sleep(500);

	// Engrave the snake
	SetOutput(OUTPUT_BIT_LASER, 1);
	for (i = 0; i < 5; i++)
	{
		moveAxeX = 50 - i * 20;
		SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
		WaitMovementsDone();

		moveAxeY = 30;
		SimulatorAxisMoveTo(&simulator, AXIS_Y, moveAxeY);
		WaitMovementsDone();

		moveAxeX = 40 - i * 20;
		SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
		WaitMovementsDone();

		moveAxeY = -30;
		SimulatorAxisMoveTo(&simulator, AXIS_Y, moveAxeY);
		WaitMovementsDone();
	}
	SetOutput(OUTPUT_BIT_LASER, 0);

	// Move to line start position
	moveAxeX = -40;
	moveAxeY = 30;
	SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
	SimulatorAxisMoveTo(&simulator, AXIS_Y, moveAxeY);
	WaitMovementsDone();

	// Engrave the line
	moveAxeX = 50;
	SetOutput(OUTPUT_BIT_LASER, 1);
	SimulatorAxisMoveTo(&simulator, AXIS_X, moveAxeX);
	WaitMovementsDone();
	SetOutput(OUTPUT_BIT_LASER, 0);

	// End of process
	ReinitializeSimulator();
	SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);
}

void Machine::PTSequence(void)
{
	int angle = 0;
	double radiansAngle = 0;
	int diameter = 20;
	int maxSpeed = 20;
	int moveTime = 100;

	double moveAxeX, moveAxeY, moveAxeZ;

	SetOutput(OUTPUT_BIT_LIGHT_GREEN, 1);
	ReinitializeSimulator();
	ChangeEngravedPiece();

	// Move to start position using PTP
	moveAxeX = 0;
	moveAxeY = 0;
	moveAxeZ = 45;
	MovePtpAllAxis(moveAxeX, moveAxeY, moveAxeZ);
	WaitMovementsDone();
	Sleep(500);

	// Calculate and add the point for the circle
	for (angle = 5; angle <= 360; angle += 5)
	{
		radiansAngle = angle * M_PI / 180;
		moveAxeX = sin(radiansAngle) * diameter;
		moveAxeY = cos(radiansAngle) * diameter;
		moveAxeY -= diameter;
		AddPtToPvtAllAxis(moveAxeX, moveAxeY, moveAxeZ, moveTime);
	}

	// Engrave the circle
	SetOutput(OUTPUT_BIT_LASER, 1);
	SimulatorStartAllAxisPTMove(&simulator);
	WaitMovementsDone();
	SetOutput(OUTPUT_BIT_LASER, 0);

	// Move to the square start position using PTP
	moveAxeX = 10;
	moveAxeY = 10;
	moveAxeZ = 45;
	MovePtpAllAxis( moveAxeX, moveAxeY, moveAxeZ);
	WaitMovementsDone();
	Sleep(500);

	// Calculate and add the point for the square
	moveAxeX = 35;
	moveAxeY = 35;
	AddPtToPvtAllAxis(moveAxeX, moveAxeY, moveAxeZ, 500);

	moveAxeX = 60;
	moveAxeY = 10;
	AddPtToPvtAllAxis(moveAxeX, moveAxeY, moveAxeZ, 500);

	moveAxeX = 35;
	moveAxeY = -15;
	AddPtToPvtAllAxis(moveAxeX, moveAxeY, moveAxeZ, 500);

	moveAxeX = 10;
	moveAxeY = 10;
	AddPtToPvtAllAxis(moveAxeX, moveAxeY, moveAxeZ, 500);

	// Engrave the square
	SetOutput(OUTPUT_BIT_LASER, 1);
	SimulatorStartAllAxisPTMove(&simulator);
	WaitMovementsDone();
	SetOutput(OUTPUT_BIT_LASER, 0);

	// End of Process
	ReinitializeSimulator();
	SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);
}

double Machine::GetAxisCurrentPosition(char axisControllerID)
{
	return SimulatorGetAxisCurrentPosition(&simulator, axisControllerID);
}

void Machine::StartAllAxisPTMove()
{
	SimulatorStartAllAxisPTMove(&simulator);
}

void Machine::SignalingError() {

	SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);
	SetOutput(OUTPUT_BIT_LIGHT_RED, 1);
	SetOutput(OUTPUT_BIT_LASER, 0);
	DisplayErrors();
}
