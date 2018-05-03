/*********************************************************************************

Fichier 	:  	Point.cpp
Auteur 		:	Huber Michael & Théo Tchoffo
Date		:	17.04.2018

*********************************************************************************/
#include "Point.h"

namespace EngravingSystem {

	void Point::XmlWrite(XmlWriter& writer)
	{
		writer.Write("X", _x);
		writer.Write("Y", _y);
		writer.Write("Z", _z);
	}
	void Point::XmlRead(XmlReader& reader)
	{
		reader.Read("X", _x);
		reader.Read("Y", _y);
		reader.Read("Z", _z);
	}

}
