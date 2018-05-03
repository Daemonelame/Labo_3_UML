/*********************************************************************************

Fichier 	:  	ApplicationMachine.h
Auteur 		:	Huber Michael & Théo Tchoffo
Date		:	16.04.2018

*********************************************************************************/

#pragma once
#include "Machine.h"
#include "SimulatorController.h"
#include "Segment.h"
#include "Chronometer.h"
#include <fstream>
#include <conio.h>
#include <math.h>

using namespace EngravingSystem;

class ApplicationMachine
{
public:
	ApplicationMachine(char* ip) : _machine(ip){}
private:
	Machine _machine;
	Segment segment;
	const char* ApplicationDataFile = "ApplicationData.xml";

public:
	void Graver();
	void LearnPoint(Point & point);
	void LearnSegment();
	void ManageUserMenu();
	void ManagePeristentData();
	bool ReadDataFile();
	void WriteDataFile();
};
