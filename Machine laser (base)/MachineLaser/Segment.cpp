/*********************************************************************************

Fichier 	:  	Segment.cpp
Auteur 		:	Huber Michael & Théo Tchoffo
Date		:	17.04.2018

*********************************************************************************/
#include "Segment.h"



namespace EngravingSystem {

	void Segment::XmlWrite(XmlWriter& writer)
	{
		writer.WriteOpenMarkup("Point A");
		_pointA.XmlWrite(writer);
		writer.WriteCloseMarkup("Point A");

		writer.WriteOpenMarkup("Point B");
		_pointB.XmlWrite(writer);
		writer.WriteCloseMarkup("Point B");
	}

	void Segment::XmlRead(XmlReader& reader)
	{
		reader.ReadOpenMarkup("Point A");
		_pointA.XmlRead(reader);
		reader.ReadCloseMarkup("Point A");

		reader.ReadOpenMarkup("Point B");
		_pointB.XmlRead(reader);
		reader.ReadCloseMarkup("Point B");
	}

}
