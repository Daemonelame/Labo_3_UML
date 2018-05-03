#pragma once
#include "Xml.h"
#include <fstream>
using namespace std;

class XmlFileWriter : public XmlWriter
{
public:
	XmlFileWriter() : XmlWriter(_stream)
	{
	}
	XmlFileWriter(const char * filename) : XmlWriter(_stream)
	{
		Open(filename);
	}
	bool IsOpen() const
	{
		return _stream.is_open();
	}
	void Open(const char * filename)
	{
		_stream.open(filename, ios::out);
	}
	void Close()
	{
		_stream.close();
	}

private:
	ofstream _stream;
};

class XmlFileReader : public XmlReader
{
public:
	XmlFileReader() : XmlReader(_stream)
	{
	}
	XmlFileReader(const char * filename) : XmlReader(_stream)
	{
		Open(filename);
	}
	bool IsOpen() const
	{
		return _stream.is_open();
	}
	void Open(const char * filename)
	{
		_stream.open(filename, ios::in);
		if (!IsOpen())
			throw exception("File not found.");
	}
	void Close()
	{
		_stream.close();
	}
private:
	ifstream _stream;
};