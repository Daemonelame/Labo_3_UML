#pragma once
#include "XmlFile.h"
#include "Chronometer.h"
#include "Engravings.h"
#include "AdaptPositionToPlan.h"

class Application : public IPointTeacher
{
public:
	Application(char * ip, EngravingFactory & engravingFactory) : _machine(ip), _engravings(engravingFactory)
	{
		_machine.Reinitialize();
	}

	void Run()
	{
		LoadPeristentData();
		ManageUserMenu();
		SavePersistentData();
	}

private :
	//////////////////////////////////////////////////////////////////////////////////////////
	// User menu management
	//////////////////////////////////////////////////////////////////////////////////////////
	void ManageUserMenu()
	{
		char chosenMenu;
		do
		{
			if (_machine.HasErrors())
				DisplayErrors();
			_machine.SetOutput(OUTPUT_BIT_LIGHT_RED, _machine.HasErrors());

			cout << "-----------------------------------------" << endl;
			cout << "---------- Machine graveur 3D -----------" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "1 - Deplacement manuel du robot" << endl;
			cout << "2 - Charger une nouvelle piece" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "3 - Ajouter une gravure" << endl;
			cout << "4 - Modifier une gravure" << endl;
			cout << "5 - Supprimer une gravure" << endl;
			cout << "6 - Afficher les gravures" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "7 - Graver" << endl;
			cout << "-----------------------------------------" << endl;
			cout << "0 - Quitter" << endl << endl;
			cout << ">";
			chosenMenu = _getch();
			cout << chosenMenu << endl;

			switch (chosenMenu)
			{
			case '1': ManualJog();			break;
			case '2': ChangePiece();		break;
			case '3': AddEngraving(); 		break;
			case '4': EditEngraving();		break;
			case '5': DeleteEngraving();	break;
			case '6': DisplayEngravings();  break;
			case '7': Engrave();			break;
			}
		} while (chosenMenu != '0');
	}

	void ManualJog()
	{
		const double StepMin = 0.01;
		const double StepMax = 10.0;
		static double Step = 1.0;

		bool EtatLaser = FALSE;

		_machine.SetOutput(OUTPUT_BIT_LIGHT_ORANGE, 1);

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
		cout << "Pas actuel : " << Step << endl;

		char key;
		do
		{
			key = _getch();

			switch (key)
			{
			case '+':
				if (Step < StepMax)
				{
					Step *= 2;
					if (Step > StepMax)
						Step = StepMax;

					cout << "Pas actuel : " << Step << (Step == StepMax ? "(max)" : "") << endl;
				}
				break;
			case '-':
				if (Step > StepMin)
				{
					Step /= 2;
					if (Step < StepMin)
						Step = StepMin;

					cout << "Pas actuel : " << Step << (Step == StepMin ? "(min)" : "") << endl;
				}
				break;
			case 's': JogAxis(AXIS_X, Step);  break;
			case 'd': JogAxis(AXIS_X, -Step); break;
			case 'f': JogAxis(AXIS_Y, Step);  break;
			case 'v': JogAxis(AXIS_Y, -Step); break;
			case 'g': JogAxis(AXIS_Z, Step);  break;
			case 't': JogAxis(AXIS_Z, -Step); break;
			case 'l':
				EtatLaser = !EtatLaser;
				_machine.SetOutput(OUTPUT_BIT_LASER, EtatLaser);
				break;
			}
			if (_machine.HasErrors())
				DisplayErrors();
		} while (key != 'q');
		_machine.SetOutput(OUTPUT_BIT_LIGHT_ORANGE, 0);
		_machine.SetOutput(OUTPUT_BIT_LASER, 0);
	}

	void ChangePiece()
	{
		_machine.ChangeEngravedPiece();
		cout << "Nouvelle piece chargee" << endl;
	}

	void AddEngraving()
	{
		cout << "Ajouter une gravure" << endl;
		if (!_engravings.IsFull())
		{
			cout << "Selectionner type de gravure (selection par index) : " << endl;
			for (int index = 0; index < _engravings.GetFactory().GetIdsCount(); index++)
				cout << index+1 << ") " << _engravings.GetFactory()[index] << endl;
			
			do
			{
				int selection = Utils::CinInt("Selection");
				if (selection > 0 && selection <= _engravings.GetFactory().GetIdsCount())
				{
					string id = _engravings.GetFactory()[selection - 1];
					Engraving & engraving = _engravings.GetFactory().Create(id);
					_engravings.Add(engraving);
					engraving.Edit(*this);
					break;
				}
				else
					cout << "Bad selection" << endl;
			} while (true);
		}
		else
			cout << "Liste pleine." << endl;
	}

	void EditEngraving()
	{
		cout << "Editer une gravure" << endl;
		if (!_engravings.IsEmpy())
		{
			cout << "Editer gravure (selection par index) : " << endl;
			int selection = SelectEngraving();
			cout << "Gravure selectionnee : " << endl;
			_engravings[selection].Display();
			cout << endl;
			_engravings[selection].Edit(*this);
			cout << "Gravure modifiee." << endl;
		}
		else
			cout << "Liste vide." << endl;
	}

	void DeleteEngraving()
	{
		cout << "Supprimer une gravure" << endl;
		if (!_engravings.IsEmpy())
		{
			cout << "Supprimer gravure (selection par index) : " << endl;
			_engravings.Remove(SelectEngraving());
			cout << "Gravure supprimee." << endl;
		}
		else
			cout << "Liste vide." << endl;
	}

	void DisplayEngravings()
	{
		if (!_engravings.IsEmpy())
		{
			cout << "Affichage liste de gravure : " << endl;
			cout << _engravings << endl;
		}
		else
			cout << "Liste vide." << endl;
	}

	int SelectEngraving()
	{
		int selection = 0;
		for (int i = 0; i < _engravings.GetCount(); i++)
		{
			cout << i + 1 << ") ";
			_engravings[i].Display();
			cout << endl;
		}

		do
		{
			selection = Utils::CinInt("Entrer selection") - 1;
			if (selection >= 0 && selection < _engravings.GetCount())
				break;
			else
				cout << "Index selectionne en dehors des bornes." << endl;
		} while (true);

		return selection;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Engraving
	//////////////////////////////////////////////////////////////////////////////////////////

	void Engrave()
	{
		if (!_engravings.IsEmpy())
		{
			cout << "Processing engraving : " << endl;

			// Move to start point
			_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 1);
			_machine.SetOutput(OUTPUT_BIT_LASER, 0);
			Chronometer timer;
			timer.Start();
			try
			{
				for (int index = 0; index < _engravings.GetCount(); index++)
				{
					cout << "Engraving " << index + 1 << "/" << _engravings.GetCount() << endl;
					_engravings[index].Display();
					cout << endl;
					_engravings[index].Engrave(_machine);
				}
			}
			catch (AbortException &)
			{
				cout << "Engraving aborted" << endl;
				_machine.SetOutput(OUTPUT_BIT_LASER, 0);
				SimulatorAxisBrake(_machine.GetController(), AXIS_X);
				SimulatorAxisBrake(_machine.GetController(), AXIS_Y);
				SimulatorAxisBrake(_machine.GetController(), AXIS_Z);
			}
			catch (exception & e)
			{
				cout << e.what() << endl;
			}
			timer.Stop();
			_machine.SetOutput(OUTPUT_BIT_LIGHT_GREEN, 0);

			cout << "Engraving time : " << timer.GetTime() << " ms."
				<< endl;
		}
		else
			cout << "Liste vide." << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Axis
	//////////////////////////////////////////////////////////////////////////////////////////
	void JogAxis(char axeControllerID, double step)
	{
		double targetPosition = SimulatorGetAxisPositionToReach(_machine.GetController(), axeControllerID) + step;

		char * saturatedMessage = NULL;
		if (step < 0)
		{
			double axisMinPosition = SimulatorGetAxisMinPosition(_machine.GetController(), axeControllerID);
			if (targetPosition < axisMinPosition)
			{
				targetPosition = axisMinPosition;
				saturatedMessage = "minimal";
			}
		}
		else
		{
			double axisMaxPosition = SimulatorGetAxisMaxPosition(_machine.GetController(), axeControllerID);
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

		SimulatorAxisMoveTo(_machine.GetController(), axeControllerID, targetPosition);
	}

	void Teach(Point &point, const string & name)
	{
		cout << "Apprentissage du Point " << name << endl;
		cout << "Valeur courant du point : " << endl << point << endl;
		if (Utils::UserRequest("Aller au point actuel"))
			MovePtpAllAxis(_machine.GetController(), point.GetX(), point.GetY(), point.GetZ());

		ManualJog();

		if (Utils::UserRequest("Apprendre le point avec la position courant du robot"))
		{
			point.SetX(SimulatorGetAxisCurrentPosition(_machine.GetController(), AXIS_X));
			point.SetY(SimulatorGetAxisCurrentPosition(_machine.GetController(), AXIS_Y));
			point.SetZ(SimulatorGetAxisCurrentPosition(_machine.GetController(), AXIS_Z));
		}
		cout << "Valeurs du point : " << endl << point << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Errors
	//////////////////////////////////////////////////////////////////////////////////////////
	void DisplayErrors()
	{
		cout << "Error diagnostic : " << endl;
		if (SimulatorErrorOnConnection(_machine.GetController()))
		{
			cout << "- Could not communicate with machine." << endl;
			cout << "  %s\n", SimulatorGetConnectionErrorDescription(_machine.GetController());
		}
		if (SimulatorIsAxisOnError(_machine.GetController(), AXIS_X))
			cout << "- Axis X in error." << endl;
		if (SimulatorIsAxisOnError(_machine.GetController(), AXIS_Y))
			cout << "- Axis Y in error." << endl;
		if (SimulatorIsAxisOnError(_machine.GetController(), AXIS_Z))
			cout << "- Axis Z in error." << endl;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Persistence & Serialization
	//////////////////////////////////////////////////////////////////////////////////////////
	const char * EngravingsFile = "Engravings.xml";
	const char * EngravingsEntry = "Engravings";
	void LoadPeristentData()
	{
		try
		{
			if (Utils::FileExists(EngravingsFile))
			{
				XmlFileReader reader(EngravingsFile);
				reader.ReadOpenMarkup(EngravingsEntry);
				_engravings.XmlRead(reader);
				reader.ReadCloseMarkup(EngravingsEntry);
				reader.Close();
			}
			else
				cout << "Peristent application data file not found. Default values will be taken." << endl;
		}
		catch (exception &e)
		{
			cout << "Error while loading application data file : " << e.what() << endl;
		}
	}
	
	void SavePersistentData()
	{
		XmlFileWriter writer(EngravingsFile);
		writer.WriteOpenMarkup(EngravingsEntry);
		_engravings.XmlWrite(writer);
		writer.WriteCloseMarkup(EngravingsEntry);
		writer.Close();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Members
	//////////////////////////////////////////////////////////////////////////////////////////
	Machine _machine;
	Engravings _engravings;
};