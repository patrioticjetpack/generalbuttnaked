#include "IntVariableTable.h"
#include "LabelTable.h"
#include "DroneCommandTable.h"
#include "InstructionTable.h"
#include "FlightPlanParse.h"
#include "FlightPlanExecute.h"
#include <iostream>
#include <fstream>


using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::string;


// Provides an example of using the FlightPlanParse and FlightPlanExecute classes supporting
// FPL parsing, optional execution tracing and optional drone communication.
// The program prompts for the name of a FPL file, parses the file, displays the parse tables
// generated, and executes the FPL program according to the modes selected.


int main()
{
	cout << "Enter the FPL file name without the .txt suffix: ";

	string file_name;
	cin >> file_name;
	file_name += ".txt";

	ifstream fpl_file(file_name);

	if (fpl_file.is_open()) {
		IntVariableTable  int_variables;
		LabelTable        labels;
		DroneCommandTable drone_commands;
		InstructionTable  instructions;
		FlightPlanParse   fpl_parse(int_variables, labels, drone_commands, instructions);
		string            line;
		while (getline(fpl_file, line)) {
			line += ' ';
			fpl_parse.parseLine(line);
		}
		int_variables.display();
		labels.display();
		drone_commands.display();
		if (fpl_parse.parseSuccess()) {
			fpl_parse.displayInstructions();
			FlightPlanExecute fpl_execute(int_variables, labels, drone_commands, instructions);
			fpl_execute.executeProgram(DroneMode::BOTH, TraceMode::ALL_OPCODES);
			cout << endl << "Press any letter followed by the return key to exit the application: ";
			char c;
			cin >> c;
		}
	}
	else {
		cout << "File " << file_name << " not found" << endl;
	}

	return 0;
}
