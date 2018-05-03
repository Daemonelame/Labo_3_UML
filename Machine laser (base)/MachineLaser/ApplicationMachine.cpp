/*********************************************************************************

Fichier 	:  	ApplicationMachine.cpp
Auteur 		:	Nop
Date		:	16.04.2018

*********************************************************************************/

#include "ApplicationMachine.h"
#include "Machine.h"
#include "XmlFile.h"
#include <iostream>

using namespace std;
using namespace EngravingSystem;


void ApplicationMachine::Graver()
{
	/* Variables */
	Chronometer timer;
	double pointA_Z = 0;
	double pointB_Z = 0;
	int time = 1000; /* temps en ms */

	/* AMémorisation des coordonnées en Z des deux points */
	pointA_Z = segment.GetPointA().GetZ();
	pointB_Z = segment.GetPointB().GetZ();
	
	/* Initialisation */
	_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 1);
	_machine.SetOutput(OUTPUT_BIT_LASER, 0);
	
	/* Déplacement au point A */
	segment.GetPointA().SetZ(45.0);
	segment.GetPointB().SetZ(45.0);
	_machine.MovePtpAllAxis(segment.GetPointA().GetX(), segment.GetPointA().GetY(), segment.GetPointA().GetZ());
	if (_machine.WaitMovementsDone()) {
		/* Démarrage du timer ainsi que la gravure */
		timer.Start();
		_machine.SetOutput(OUTPUT_BIT_LASER, 1);
		_machine.AddPtToPvtAllAxis(segment.GetPointB().GetX(), segment.GetPointB().GetY(), segment.GetPointB().GetZ(), time);
		_machine.StartAllAxisPTMove();
		if (_machine.WaitMovementsDone()) {

			/* Stop du timer ainsi que la gravure*/
			timer.Stop();
			_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);
			_machine.SetOutput(OUTPUT_BIT_LASER, 0);

			/* Déplacement aux bonnes coordonnées en Z après gravure */
			segment.GetPointA().SetZ(pointA_Z);
			segment.GetPointB().SetZ(pointB_Z);
			_machine.MovePtpAllAxis(segment.GetPointB().GetX(), segment.GetPointB().GetY(), segment.GetPointB().GetZ());

			/* Affichage du temps de gravure */
			cout << "Temps de gravure : " << timer.GetTime() << " [ms]" << endl;
		}
		else {
			timer.Stop();
			_machine.SignalingError();
		}
	}
	else {
		timer.Stop();
		_machine.SignalingError();
	}

	segment.GetPointA().SetZ(pointA_Z);
	segment.GetPointB().SetZ(pointB_Z);

}

void ApplicationMachine::LearnPoint(Point & point) {

	/* Variables */
	char choix;

	/* Demande de déplacement au dernier point appris */
	cout << "Point courant : " << endl << point << endl;
	cout << "Voulez-vous déplacer le robot au point courant ? (1->oui - 0->non) :";
	choix = _getch();

	/* Déplacement du robot au dernier point appris */
	if (choix == '1') {
		_machine.MovePtpAllAxis(point.GetX(), point.GetY(), point.GetZ());
		_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 1);
		if (!_machine.WaitMovementsDone()) {
			_machine.SignalingError();
		}
		_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);
	}
		

	/* Déplacement du robot manuellement*/
	_machine.ManualJog();

	cout << "Voulez-vous apprendre le point avec la position actuelle ? (1->oui - 0->non) :";
	choix = _getch();

	if (choix) {
		point.SetX(_machine.GetAxisCurrentPosition(AXIS_X));
		point.SetY(_machine.GetAxisCurrentPosition(AXIS_Y));
		point.SetZ(_machine.GetAxisCurrentPosition(AXIS_Z));
		cout << "Valeurs apprises : " << endl << point << endl;
	}
}

void ApplicationMachine::LearnSegment() {

	/* Apprentissage des deux points du segment */
	LearnPoint(segment.GetPointA());
	LearnPoint(segment.GetPointB());
	
}

void ApplicationMachine::ManageUserMenu()
{
	char chosenMenu;

	ManagePeristentData();

	do
	{

		cout << "-----------------------------------------" << endl;
		cout << "------ Gravage laser de segments --------" << endl;
		cout << "-----------------------------------------" << endl;
		cout << "1 - Deplacement manuel du robot" << endl;
		cout << "2 - Charger une nouvelle piece" << endl;
		cout << "3 - Apprentissage du point A" << endl;
		cout << "4 - Apprentissage du point B" << endl;
		cout << "5 - Gravage du segment AB" << endl;
		cout << "0 - Quitter" << endl << endl;
		cout << ">";
		chosenMenu = _getch();
		cout << chosenMenu << endl;

		/* Quittance automatiqe des erreurs --> Reinitialisation  (seule méthode fonctionnelle trouvée) */
		if (_machine.HasErrors()) {
			_machine.ReinitializeSimulator();
			_machine.SetOutput(OUTPUT_BIT_LIGHT_RED, _machine.HasErrors());
		}

		switch (chosenMenu)
		{
			case '1': _machine.ManualJog();								break;
			case '2': _machine.ChangeEngravedPiece();					break;
			case '3': LearnPoint(segment.GetPointA());		break;
			case '4': LearnPoint(segment.GetPointB());		break;
			case '5': Graver();						break;
		}
		
	} while (chosenMenu != '0');

	WriteDataFile();
	_machine.ReinitializeSimulator();
}


void ApplicationMachine::ManagePeristentData()
{
	try
	{
		if (ReadDataFile())
			cout << "Lecture des donnees de l'application s est bien deroulee." << endl;
		else {
			cout << "Fichier des donnees persistentes non trouve." << endl;
			
		}
	}
	catch (exception &e)
	{
		Point point;
		point.SetX(0);
		point.SetY(0);
		point.SetZ(0);
		segment.SetPointA(point);
		segment.SetPointB(point);
		cout << "Erreur durant le chargement du fichier des donnees persistentes : " << e.what() << endl;
	}
}

bool ApplicationMachine::ReadDataFile() {
	XmlFileReader reader(ApplicationDataFile);
	if (reader.IsOpen()) {
		reader.ReadOpenMarkup("Data");
		reader.ReadOpenMarkup("Segment");
		segment.XmlRead(reader);
		reader.ReadCloseMarkup("Segment");
		reader.ReadCloseMarkup("Data");
		reader.Close();
		return true;
	}
	return false;
}

void ApplicationMachine::WriteDataFile()
{
	XmlFileWriter writer(ApplicationDataFile);
	if (writer.IsOpen())
	{
		writer.WriteOpenMarkup("Data");
		writer.WriteOpenMarkup("Segment");
		segment.XmlWrite(writer);
		writer.WriteCloseMarkup("Segment");
		writer.WriteCloseMarkup("Data");
		writer.Close();
	}
	else
		throw exception("Fichier pas correctement ouvert.");
}