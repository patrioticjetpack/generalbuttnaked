#include "FlightPlanExecute.h"
#include "TelloApi.h"
#include <iostream>


// FlightPlanExecute class version 1.2

// This subset of the FlightPlanExecute member functions concentrates on sending commands
// to the Tello drone.


using std::cout;
using std::endl;
using std::string;
using std::size_t;


// Execute the Tello command argument.
// Generic FPL drone commands are translated into Tello-specific commands:
// 1) A Tello object will be created when an "<initialize>" drone command is executed.
// 2) The '<' and '>' drone command delimiters are removed.
// 3) The "move" command is translated to the Tello "go" command.
// 4) The "arm" command (as used in the Tello smartphone app) is translated to a "speed" command.
// It is assumed that drone commands containing identifiers beginning with '%' will have the identifier
// substrings already replaced with the current values of the corresponding integer variables.

void FlightPlanExecute::executeTelloCommand(const string& command)
{
	static const string default_speed{ " 30" };		// Tello's default speed in cm/sec

	if (command == "<initialize>") {
		if (tello_drone == nullptr) {
			tello_drone = new Tello();
			if (!tello_drone->canInitialize()) {
				cout << "Tello initialization failed" << endl;
				delete tello_drone;
				tello_drone = nullptr;
			}
		}
		else {
			cout << "Tello is already initialized" << endl;
		}
	}
	else {
		if (tello_drone == nullptr) {
			cout << "Tello not initialized - " << command << " command skipped" << endl;
		}
		else {
			string tello_command;
			const size_t n{ command.length() };
			if ((n > 7) && (command.substr(0, 6) == "<move ")) {
				tello_command = "go ";
				tello_command += command.substr(6, n - 7);
				tello_command += default_speed;
			}
			else if (command == "<arm>") {
				tello_command = "speed";
				tello_command += default_speed;
			}
			else if (n > 2) {
				tello_command = command.substr(1, n - 2);
			}
			tello_drone->sendCommand(tello_command);
		}
	}
}
