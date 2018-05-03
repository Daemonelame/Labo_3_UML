/*********************************************************************************

Fichier 	:  	Point.h
Auteur 		:	Huber Michael & Th�o Tchoffo
Date		:	16.04.2018

*********************************************************************************/

#pragma once
#include <iostream>
#include "Xml.h"

namespace EngravingSystem {

	class Point {
	private:
		double _x = 0;
		double _y = 0;
		double _z = 0;

	public:

		/* M�thode In-line */
		double GetX(void)  { return _x; }
		double GetY(void)  { return _y; }
		double GetZ(void)  { return _z; }
		void SetX(double x) { _x = x; }
		void SetY(double y) { _y = y; }
		void SetZ(double z) { _z = z; }

		/* M�thode se trouvant dans le fichier "Point.cpp" */
		void XmlWrite(XmlWriter& writer);
		void XmlRead(XmlReader& reader);



	};

	/* Surcharge de l'op�rateur "<<" */
	inline
	ostream& operator<<(ostream& stream, Point & point)
	{
		stream << "X : " << point.GetX() << endl;
		stream << "Y : " << point.GetY() << endl;
		stream << "Z : " << point.GetZ() << endl;
		return stream;
	}

}