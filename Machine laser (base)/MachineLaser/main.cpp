#include "Utilities.h"
#include "SimulatorController.h"
#include "ApplicationMachine.h"
#include "Machine.h"

int main(void)
{
	//Segment segment;
	char simulatorIpAdresse[1024] = "127.0.0.1";
	ApplicationMachine appMachine(simulatorIpAdresse);

	cout.precision(3);

	appMachine.ManageUserMenu();
}