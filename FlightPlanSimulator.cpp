#include "FlightPlanExecute.h"
#include "DroneSimulatorApi.h"
#include <iostream>


// FlightPlanExecute class version 1.2

// This subset of the FlightPlanExecute member functions concentrates on sending commands
// to the drone simulator.


using std::cout;
using std::endl;
using std::string;
using std::size_t;


// Execute the drone simulator command argument.
// Generic FPL drone commands are translated into simulator-specific commands:
// 1) A simulator object will be created when an "<initialize>" drone command is executed.
// 2) The '<' and '>' drone command delimiters are removed.
// It is assumed that drone commands containing identifiers beginning with '%' will have the identifier
// substrings already replaced with the current values of the corresponding integer variables.

void FlightPlanExecute::executeSimulatorCommand(const string& command)
{
	if (command == "<initialize>") {
		if (drone_simulator == nullptr) {
			drone_simulator = new DroneSimulator();
		}
		else {
			cout << "The drone simulator is already initialized" << endl;
		}
	}
	else {
		if (drone_simulator == nullptr) {
			cout << "Drone simulator not initialized - " << command << " command skipped" << endl;
		}
		else {
			const size_t n{ command.length() };
			if ((n > 2) && (command[0] == '<') && (command[n - 1] == '>')) {
				string simulator_command{ command.substr(1, n - 2) };
				drone_simulator->sendCommand(simulator_command);
			}
		}
	}
}
