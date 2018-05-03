/*********************************************************************************

Fichier 	:  	Segment.h
Auteur 		:	Huber Michael & Th�o Tchoffo
Date		:	16.04.2018

*********************************************************************************/

#pragma once
#include "Point.h"


namespace EngravingSystem {

	class Segment {

	private:
		Point _pointA;
		Point _pointB;

	public:

		/* M�thode In-line */
		void SetPointA(Point point) { _pointA = point; }
		void SetPointB(Point point) { _pointB = point; }
		Point& GetPointA(void) { return _pointA; }
		Point& GetPointB(void) { return _pointB; }

		/* M�thode se trouvant dans le fichier "Segment.cpp" */
		void XmlWrite(XmlWriter& writer);
		void XmlRead(XmlReader& reader);


	};

	/* Surcharge de l'op�rateur "<<" */
	inline
	ostream& operator<<(ostream& stream, Segment& segment){

		stream << "Point A : " << segment.GetPointA() << endl;
		stream << "Point B : " << segment.GetPointB() << endl;
		return stream;
	}

}