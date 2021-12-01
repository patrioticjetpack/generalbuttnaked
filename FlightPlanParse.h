#ifndef FLIGHT_PLAN_PARSE_H
#define FLIGHT_PLAN_PARSE_H


#include <string>


// FlightPlanParse class version 1.2


// When a compiler parses a language, it builds a variety of data structures such as tables.
// The Flight Plan Language (FPL) has tokens separated by white space (blanks and tab characters).
// The possible token types are: opcodes, integer variables, integer constants, labels, and
// drone commands.
// FPL program may also have comments that extend from a '#' character to the end of the line.
// There is a separate table maintained for integer variables, labels, and drone commands.
// There is also a table of instructions, where each instruction entry is defined by a line
// containing an opcode and from 0 to 2 operands.
// Opcodes are represented by an enumerated type rather than a table.
// A table is not needed for integer constants because the integer value is simply stored
// directly in the instruction table's operand1 or operand2 fields.
// These tables are used during FPL execution.


// Forward declarations to reduce the need for include files.

class IntVariableTable;
class LabelTable;
class DroneCommandTable;
class InstructionTable;


// The FlightPlanParse class encapsulates all member functions and data structures
// needed to parse FPL programs.

class FlightPlanParse
{
public:		// member functions intended to be used by clients of the class

	FlightPlanParse(IntVariableTable&  variables,
		            LabelTable&        labels,
		            DroneCommandTable& drone_commands,
		            InstructionTable&  instructions);	// constructor

	void parseLine(const std::string& line);
	bool parseSuccess() const;
	void displayInstructions() const;

private:	// member functions not intended to be used by clients of the class

	bool        addLabelOrInstruction(std::string tokens[]);
	std::string indexToInstructionLine(int index) const;
	bool        validInstructionOperands(int index) const;

private:	// data members should always have private scope

	IntVariableTable&  int_variable_table;		// records integer variables
	LabelTable&        label_table;				// records labels
	DroneCommandTable& drone_command_table;		// records drone commands
	InstructionTable&  instruction_table;		// records instructions

	bool parse_success = true;					// indicates if all FPL lines read so far parsed without errors
};


#endif // FLIGHT_PLAN_PARSE_H
