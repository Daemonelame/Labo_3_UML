#pragma once
#include <iostream>  
#include <string>

using namespace std;

const char  OpenMarkupBegin = '<';
const string CloseMarkupBegin = "</";
const char MarkupEnd = '>';

class XmlWriter {
private:
	ostream &_stream;

public :
	XmlWriter(ostream& stream) : _stream(stream)
	{}
	void WriteOpenMarkup(const string & name)
	{
		if (name.length() == 0)
			throw exception("Markup name cannot be empt");
		_stream << OpenMarkupBegin << name << MarkupEnd << endl;
	}
	void WriteCloseMarkup(const string & name)
	{
		if (name.length() == 0)
			throw exception("Markup name cannot be empt");
		_stream << CloseMarkupBegin << name << MarkupEnd << endl;
	}

	void Write(const string & name, const string & value)
	{
		WriteOpenMarkup(name);
			_stream << value << endl;
		WriteCloseMarkup(name);
	}
	void Write(const string & name, const bool & value)
	{
		WriteOpenMarkup(name);
			_stream << value << endl;
		WriteCloseMarkup(name);
	}
	void Write(const string & name, const double & value)
	{
		WriteOpenMarkup(name);
			_stream << value << endl;
		WriteCloseMarkup(name);
	}
	void Write(const string & name, const int & value)
	{
		WriteOpenMarkup(name);
			_stream << value << endl;
		WriteCloseMarkup(name);
	}
};

class XmlReader {
private:
	istream &_stream;

public:
	XmlReader(istream &stream) : _stream(stream)
	{
	}

	void ReadOpenMarkup(const string & name)
	{
		string line;
		getline(_stream, line);
		if (line.length() > 2)
		{
			if (line[0] != OpenMarkupBegin)
				throw exception("No open markup found as expected at first line char.");
			if (line.substr(1, line.length() - 2) != name)
				throw exception("Markup expected mismatch");
			if (line[line.length() - 1] != MarkupEnd)
				throw exception("No end of markup found as expected at last line char.");
		}
		else
			throw exception("No open markup found as expected.");
	}

	void ReadCloseMarkup(const string & name)
	{
		string line;
		getline(_stream, line);
		if (line.length() > 3)
		{
			int t = line.length();
			if (line[0] != OpenMarkupBegin || line[1] != '/')
				throw exception("No close markup found as expected at first line chars.");
			if (line.substr(2, line.length() - 3) != name)
				throw exception("Markup expected mismatch");
			if (line[line.length() - 1] != MarkupEnd)
				throw exception("No end of markup found as expected at last line char.");
		}
		else
			throw exception("No close markup found as expected.");
	}

	void Read(const string & name, string & value)
	{
		ReadOpenMarkup(name);
			getline(_stream, value);
		ReadCloseMarkup(name);
	}

	void Read(const string & name, bool & value)
	{
		string valueStr;
		Read(name, valueStr);
		value = valueStr[0] == '1';
	}
	void Read(const string & name, double & value)
	{
		string valueStr;
		Read(name, valueStr);
		value = atof(valueStr.c_str());
	}
	void Read(const string & name, int & value)
	{
		string valueStr;
		Read(name, valueStr);
		value = atoi(valueStr.c_str());
	}
};