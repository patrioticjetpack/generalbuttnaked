#ifndef FLIGHT_PLAN_EXECUTE_H
#define FLIGHT_PLAN_EXECUTE_H


#include <string>


// FlightPlanExecute class version 1.2

// The tables generated by the FlightPlanParse class are used the FlightPlanExecute class
// to execute the FPL program.
// The only table modified by the FlightPlanExecute class is the integer variable table.


// Select which drone(s) to control during FPL execution, including none and both.

enum class DroneMode { NONE, SIMULATOR, TELLO, BOTH };


// Select the degree to which FPL instructions are printed out during FPL execution.

enum class TraceMode { OFF, CMD_NOP_OPCODES, ALL_OPCODES };


// Forward declarations to reduce the need for include files.

class IntVariableTable;
class LabelTable;
class DroneCommandTable;
class InstructionTable;
class DroneSimulator;
class Tello;

struct InstructionEntry;


// The FlightPlanExecute class encapsulates all member functions and data structures needed to execute
// FPL programs and communicate with a drone.
// The four parse tables used by the FlightPlanExecute class are generated by the FlightPlanParse class.
// See FlightPlanExecute.cpp and FlightPlanTello.cpp for a description of the member functions.

class FlightPlanExecute
{
public:		// member functions intended to be used by clients of the class

	FlightPlanExecute(IntVariableTable&        variables,
		              const LabelTable&        labels,
		              const DroneCommandTable& drone_commands,
		              const InstructionTable&  instructions);	// constructor
	~FlightPlanExecute();										// destructor

	void executeProgram(DroneMode drone, TraceMode trace);

private:	// member functions not intended to be used by clients of the class

	void executeNextInstruction();

	void executeIntInstruction(const InstructionEntry& instruction);
	void executeAddInstruction(const InstructionEntry& instruction);
	void executeSubInstruction(const InstructionEntry& instruction);
	void executeMulInstruction(const InstructionEntry& instruction);
	void executeDivInstruction(const InstructionEntry& instruction);
	void executeSetInstruction(const InstructionEntry& instruction);
	void executeCmpInstruction(const InstructionEntry& instruction);
	void executeBeqInstruction(const InstructionEntry& instruction);
	void executeBneInstruction(const InstructionEntry& instruction);
	void executeBraInstruction(const InstructionEntry& instruction);
	void executeCmdInstruction(const InstructionEntry& instruction);
	void executeNopInstruction(const InstructionEntry& instruction);
	void executeEndInstruction(const InstructionEntry& instruction);

	void executeSimulatorCommand(const std::string& command);
	void executeTelloCommand(const std::string& command);

	int  getOperand1(const InstructionEntry& instruction) const;
	int  getOperand2(const InstructionEntry& instruction) const;

	std::string insertVariableValues(const std::string& command) const;

private:	// data members should always have private scope

	IntVariableTable&        int_variable_table;	// records integer variables
	const LabelTable&        label_table;			// records labels
	const DroneCommandTable& drone_command_table;	// records drone commands
	const InstructionTable&  instruction_table;		// records instructions

	DroneSimulator* drone_simulator{ nullptr };		// dynamically instantiated drone simulator object
	Tello* tello_drone{ nullptr };					// dynamically instantiated Tello object

	TraceMode trace_mode{ TraceMode::OFF };			// level of instruction tracing desired
	DroneMode drone_mode{ DroneMode::NONE };		// what drone to control, if any

	int timer_start{ 0 };							// clock() value when the drone is initialized
	int program_counter{ 0 };						// index of the next instruction to execute

	bool compare_returns_equal{ false };			// result of a CMP instruction
	bool end_program{ false };						// an END instruction or error was encountered
};


#endif // FLIGHT_PLAN_EXECUTE_H
