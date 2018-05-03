#include <windows.h>

class Chronometer
{
private:
	int _startTime = 0;
	int _elaspedTime = 0;
	bool _started = false;;

public:
	void Start()
	{
		_startTime = GetTickCount();
		_started = true;
	}

	void Stop()
	{
		_started = false;
		_elaspedTime += GetTickCount() - _startTime;
	}

	void Reset()
	{
		_elaspedTime = 0;
		_started = false;
	}

	int GetTime()
	{
		if (_started)
			return _elaspedTime + GetTickCount() - _startTime;
		else
			return _elaspedTime;
	}
};
