#pragma once
#include <string>
#include <iostream>
#include <conio.h>
#include <fstream>
using namespace std;

class Utils
{
public:
	static bool FileExists(const char * filename)
	{
		ifstream ifile(filename);
		return ifile.is_open();
	}

	static int CinInt(const string & message)
	{
		int value = 0;
		cout << message << " : ";
		bool goodEntry = false;
		do
		{
			cin >> value;
			goodEntry = cin.good();
			if (!goodEntry)
			{
				cout << "Bad entry try again!" << endl;
				cin.clear();
				cin.ignore(1, '\n');
			}
		} while (!goodEntry);
		return value;
	}

	static double CinDouble(const string & message)
	{
		double value = 0;
		cout << message << " : ";
		bool goodEntry = false;
		do
		{
			cin >> value;
			goodEntry = cin.good();
			if (!goodEntry)
			{
				cout << "Bad entry try again!" << endl;
				cin.clear();
				cin.ignore(1, '\n');
			}
		} while (!goodEntry);
		return value;
	}

	static string CinString(const string & message)
	{
		string value;
		cout << message << " : ";

		bool goodEntry = false;
		do
		{
			cin.ignore();
			getline(cin, value);
			goodEntry = cin.good();
			if (!goodEntry)
			{
				cout << "Bad entry try again!" << endl;
				cin.clear();
				cin.ignore(1, '\n');
			}
		} while (!goodEntry);
		return value;
	}

	static bool UserRequest(const string & request)
	{
		char Saisie;
		cout << request << " ? [o/n] : " << endl;
		do
		{
			Saisie = _getch();
		} while (Saisie != 'o' && Saisie != 'O' && Saisie != 'n' && Saisie != 'N');
		return Saisie == 'o' || Saisie == 'O';
	}

	static const char* GetAxesName(char axeControllerID)
	{
		const char* axesName[]{ "AxisX", "AxisY", "AxisZ" };
		if (axeControllerID >= 0 && axeControllerID < 3)
			return axesName[axeControllerID];
		else
			throw exception("Out of range");
	}
};

