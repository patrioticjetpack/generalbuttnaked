#include "FlightPlanExecute.h"
#include "IntVariableTable.h"
#include "LabelTable.h"
#include "DroneCommandTable.h"
#include "InstructionTable.h"
#include "DroneSimulatorApi.h"
#include "TelloApi.h"
#include <iostream>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <windows.h>


// FlightPlanExecute class version 1.2

// The FlightPlanExecute member functions execute a FPL program using the parse tables
// created by the FlightPlanParse class.


using std::cout;
using std::endl;
using std::right;
using std::setw;
using std::size_t;
using std::string;
using std::to_string;


// The FlightPlanExecute constructor records references to the four parse tables.
// Only the integer variable table is modified during FPL execution.

FlightPlanExecute::FlightPlanExecute(IntVariableTable&        variables,
	                                 const LabelTable&        labels,
	                                 const DroneCommandTable& drone_commands,
	                                 const InstructionTable&  instructions) :
	int_variable_table(variables),
	label_table(labels),
	drone_command_table(drone_commands),
	instruction_table(instructions)
{}


// The FlightPlanExecute destructor deallocates the Simulator or Tello object if one of these was created.

FlightPlanExecute::~FlightPlanExecute()
{
	if (drone_simulator != nullptr) {
		delete drone_simulator;
	}

	if (tello_drone != nullptr) {
		delete tello_drone;
	}
}


// Execute the FPL program beginning at index 0 of the instruction_table.
// Execution uses the drone and trace modes specified in the arguments.
// Execution continues until either an "end" instruction is executed, or an invalid opcode or operand
// is encountered.
// A message is generated if the program cannot be executed because the instruction table is empty.

void FlightPlanExecute::executeProgram(DroneMode drone, TraceMode trace)
{
	drone_mode = drone;
	trace_mode = trace;

	if (instruction_table.numInstructions() == 0) {
		cout << endl << "Program execution cannot proceed because the instruction table is empty" << endl;
	}
	else {
		if (trace_mode == TraceMode::ALL_OPCODES) {
			cout << endl << "Program execution: [program counter | operation]" << endl << endl;
		}
		else if (trace_mode == TraceMode::CMD_NOP_OPCODES) {
			cout << endl << "Program execution: [CMD and NOP operations]" << endl << endl;
		}
		timer_start           = clock();
		program_counter       = 0;
		compare_returns_equal = false;
		end_program           = false;
		while (!end_program) {
			executeNextInstruction();
		}
	}
}


// Execute the instruction appearing at instruction_table[program_counter],
// and update the program_counter.

void FlightPlanExecute::executeNextInstruction()
{
	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << right << setw(8) << program_counter << "    ";
	}

	InstructionEntry instruction = instruction_table.getInstruction(program_counter);

	switch (instruction.opcode) {
	case Opcodes::INT:
		executeIntInstruction(instruction);
		break;
	case Opcodes::ADD:
		executeAddInstruction(instruction);
		break;
	case Opcodes::SUB:
		executeSubInstruction(instruction);
		break;
	case Opcodes::MUL:
		executeMulInstruction(instruction);
		break;
	case Opcodes::DIV:
		executeDivInstruction(instruction);
		break;
	case Opcodes::SET:
		executeSetInstruction(instruction);
		break;
	case Opcodes::CMP:
		executeCmpInstruction(instruction);
		break;
	case Opcodes::BEQ:
		executeBeqInstruction(instruction);
		break;
	case Opcodes::BNE:
		executeBneInstruction(instruction);
		break;
	case Opcodes::BRA:
		executeBraInstruction(instruction);
		break;
	case Opcodes::CMD:
		executeCmdInstruction(instruction);
		break;
	case Opcodes::NOP:
		executeNopInstruction(instruction);
		break;
	case Opcodes::END:
		executeEndInstruction(instruction);
		break;
	default:
		cout << "Undefined instruction opcode (" << opcodeToString(instruction.opcode)
			 << ") at location " << program_counter << " - program terminated" << endl;
		end_program = true;
		break;
	}
}


// Initialize an integer variable to a constant.

void FlightPlanExecute::executeIntInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::INT);

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << int_variable_table.getName(instruction.operand1) << " = " << instruction.operand2 << endl;
	}

	int_variable_table.setValue(instruction.operand1, instruction.operand2);

	program_counter++;
}


// Add another integer variable or constant to an integer variable.

void FlightPlanExecute::executeAddInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::ADD);

	int operand1{ getOperand1(instruction) };
	int operand2{ getOperand2(instruction) };
	int new_value{ operand1 + operand2 };

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << int_variable_table.getName(instruction.operand1) << " = " << operand1 << " + "
		 	 << operand2 << " = " << new_value << endl;
	}

	int_variable_table.setValue(instruction.operand1, new_value);

	program_counter++;
}


// Subtract another integer variable or a constant from an integer variable.

void FlightPlanExecute::executeSubInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::SUB);

	int operand1{ getOperand1(instruction) };
	int operand2{ getOperand2(instruction) };
	int new_value{ operand1 - operand2 };

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << int_variable_table.getName(instruction.operand1) << " = " << operand1 << " - "
			 << operand2 << " = " << new_value << endl;
	}

	int_variable_table.setValue(instruction.operand1, new_value);

	program_counter++;
}


// Multiply an integer variable by another integer variable or a constant.

void FlightPlanExecute::executeMulInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::MUL);

	int operand1{ getOperand1(instruction) };
	int operand2{ getOperand2(instruction) };
	int new_value{ operand1 * operand2 };

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << int_variable_table.getName(instruction.operand1) << " = " << operand1 << " * "
			 << operand2 << " = " << new_value << endl;
	}

	int_variable_table.setValue(instruction.operand1, new_value);

	program_counter++;
}


// Divide an integer variable by another integer variable or a constant.
// Attempting to divide by zero causes program termination.

void FlightPlanExecute::executeDivInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::DIV);

	int operand1{ getOperand1(instruction) };
	int operand2{ getOperand2(instruction) };

	if (operand2 == 0) {
		cout << "Attempted division by zero at location " << program_counter
			<< " - program terminated" << endl;
		end_program = true;
	}
	else {
		int new_value{ operand1 / operand2 };
		if (trace_mode == TraceMode::ALL_OPCODES) {
			cout << int_variable_table.getName(instruction.operand1) << " = " << operand1 << " / "
				 << operand2 << " = " << new_value << endl;
		}
		int_variable_table.setValue(instruction.operand1, new_value);
		program_counter++;
	}
}


// Set an integer variable to another integer variable or a constant.

void FlightPlanExecute::executeSetInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::SET);

	int new_value{ getOperand2(instruction) };

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << int_variable_table.getName(instruction.operand1) << " = " << new_value << endl;
	}

	int_variable_table.setValue(instruction.operand1, new_value);

	program_counter++;
}


// Compare two integer variables, or compare an integer variable to a constant.

void FlightPlanExecute::executeCmpInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::CMP);

	int operand1{ getOperand1(instruction) };
	int operand2{ getOperand2(instruction) };

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << operand1 << " == " << operand2 << " ?" << endl;
	}

	compare_returns_equal = (operand1 == operand2);

	program_counter++;
}


// Branch to a label if the previous compare instruction operands were equal.

void FlightPlanExecute::executeBeqInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::BEQ);

	if (trace_mode == TraceMode::ALL_OPCODES) {
		if (compare_returns_equal) {
			cout << "BEQ taken to label " << label_table.getName(instruction.operand1) << endl;
		}
		else {
			cout << "BEQ skipped" << endl;
		}
	}

	if (compare_returns_equal) {
		program_counter = label_table.getValue(instruction.operand1);
	}
	else {
		program_counter++;
	}
}


// Branch to a label if the previous compare instruction operands were unequal.

void FlightPlanExecute::executeBneInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::BNE);

	if (trace_mode == TraceMode::ALL_OPCODES) {
		if (compare_returns_equal) {
			cout << "BNE skipped" << endl;
		}
		else {
			cout << "BNE taken to label " << label_table.getName(instruction.operand1) << endl;
		}
	}

	if (compare_returns_equal) {
		program_counter++;
	}
	else {
		program_counter = label_table.getValue(instruction.operand1);
	}
}


// Unconditionally branch to a label.

void FlightPlanExecute::executeBraInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::BRA);

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << "BRA to label " << label_table.getName(instruction.operand1) << endl;
	}

	program_counter = label_table.getValue(instruction.operand1);
}


// Execute a drone command.
// Drone commands containing identifiers beginning with '%' will have the identifier substrings
// replaced with the current values of the corresponding integer variables.

void FlightPlanExecute::executeCmdInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::CMD);

	string command{ drone_command_table.getCommand(instruction.operand1) };
	string modified_command{ insertVariableValues(command) };

	if ((trace_mode == TraceMode::ALL_OPCODES) || (trace_mode == TraceMode::CMD_NOP_OPCODES)) {
		cout << "CMD " << command;
		if (modified_command != command) {
			cout << " becomes CMD " << modified_command;
		}
		cout << endl;
	}

	switch (drone_mode) {
	case DroneMode::NONE:
		break;
	case DroneMode::SIMULATOR:
		executeSimulatorCommand(modified_command);
		break;
	case DroneMode::TELLO:
		executeTelloCommand(modified_command);
		break;
	case DroneMode::BOTH:
		executeSimulatorCommand(modified_command);
		executeTelloCommand(modified_command);
		break;
	default:
		break;
	}

	program_counter++;
}


// The main() application thread (rather than the drone) will suspend until the number
// of seconds contained in an integer variable or constant has elapsed.
// Time is relative to the start of the FPL program execution.
// Let n = the number of seconds.
// This behavior is not the same as a delay of n seconds.
// For example, if the current time is 5 seconds and n = 7, the application thread will
// resume in 2 seconds.
// The application thread does not suspend if the current time is greater than n.

void FlightPlanExecute::executeNopInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::NOP);

	int wait_until_time{ getOperand2(instruction) };

	if ((trace_mode == TraceMode::ALL_OPCODES) || (trace_mode == TraceMode::CMD_NOP_OPCODES)) {
		cout << "Wait until " << wait_until_time << " seconds since initialization" << endl;
	}

	int pause{ (wait_until_time * CLOCKS_PER_SEC) - clock() + timer_start };

	if (pause > 0) {
		Sleep(pause);
	}

	program_counter++;
}


// Terminate program execution.

void FlightPlanExecute::executeEndInstruction(const InstructionEntry& instruction)
{
	assert(instruction.opcode == Opcodes::END);

	if (trace_mode == TraceMode::ALL_OPCODES) {
		cout << "END" << endl;
	}

	end_program = true;
}


// The instruction's first operand is assumed to contain a valid index into the integer variable table.
// The indexed variable's value is returned.

int FlightPlanExecute::getOperand1(const InstructionEntry& instruction) const
{
	return int_variable_table.getValue(instruction.operand1);
}


// The instruction's second operand is assumed to contain a valid index into the integer variable table
// (if the instruction's constant_version field is false) or an integer constant (if the instruction's
// constant_version field is true).
// Either the indexed variable's value or the constant value is returned.

int FlightPlanExecute::getOperand2(const InstructionEntry& instruction) const
{
	int right_operand{};

	if (instruction.constant_operand2) {
		right_operand = instruction.operand2;
	}
	else {
		right_operand = int_variable_table.getValue(instruction.operand2);
	}

	return right_operand;
}


// Return the command argument such that all "%variable_name" substrings are replaced with the
// current value of the corresponding integer variables.
// The '%' character indicates that the characters which follow the '%' character up to any
// space or the end of string represents an integer variable name.
// The integer variable names in the command do not necessarily have to be "x", "y" and "z".
// Each variable name is extracted from the command string, the name is looked up using
// IntVariableTable::lookupVariable(), and the variable's current value is determined using
// IntVariableTable::getValue().
// If a variable cannot be found in the integer variable table then a value of 0 is used.
// Example: If integer variables named v1, v2 and v3 have values 23, 39 and 35 respectively,
// then the string argument "go %v1 %v2 %v3 30" will be transformed to "go 23 39 35 30".

string FlightPlanExecute::insertVariableValues(const string& command) const
{
	enum class EditStates { COPY_MODE, INSERT_MODE };
	EditStates state{ EditStates::COPY_MODE };

	string modified_command;
	string variable_name;

	const size_t n{ command.length() };

	for (size_t i{ 0 }; i < n; i++) {
		const char c{ command[i] };
		switch (state) {
		case EditStates::COPY_MODE:
			if (c == '%') {
				variable_name.clear();
				state = EditStates::INSERT_MODE;
			}
			else {
				modified_command += c;
			}
			break;
		case EditStates::INSERT_MODE:
			if ((c == ' ') || (c == '>')) {
				int index{ int_variable_table.lookupVariable(variable_name) };
				int value{ 0 };
				if (int_variable_table.validIndex(index)) {
					value = int_variable_table.getValue(index);
				}
				modified_command += to_string(value);
				modified_command += c;
				state = EditStates::COPY_MODE;
			}
			else {
				variable_name += c;
			}
			break;
		default:
			break;
		}
	}

	return modified_command;
}
