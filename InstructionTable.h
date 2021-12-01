#ifndef INSTRUCTION_TABLE_H
#define INSTRUCTION_TABLE_H


#include "Opcodes.h"


// An instruction consists of an opcode field, an optional first operand, and an optional second
// operand.
// An example of an instruction with zero operands is "end".
// An example of an instruction with one operand is "cmd".
// An example of an instruction with two operands is "cmp".
// The second operand of some instructions (such as "add", "sub", "mul", div", "set", "cmp" and "nop")
// can either be an integer variable or a constant value, and the constant_operand2 field will be set
// to true if the second operand is a constant value.
// The second operand of an "int" instruction is always a constant.

struct InstructionEntry {
	Opcodes opcode{ Opcodes::UNDEFINED };
	int     operand1{ -1 };
	int     operand2{ -1 };
	bool    constant_operand2{ false };
};


// All instructions are stored in a table in the order they were parsed.

class InstructionTable
{
public:		// member functions intended to be used by clients of the class

	InstructionTable();			// constructor
	~InstructionTable();		// destructor

	int              numInstructions() const;
	void             addInstruction(const InstructionEntry& instruction);
	InstructionEntry getInstruction(int index) const;
	bool             validIndex(int index) const;

private:	// data members should always have private scope

	InstructionEntry* instruction_table{ nullptr };	// dynamically allocated array of instructions
	const int         MAX_INSTRUCTIONS{ 20000 };	// maximum number of instructions allowed
	int               num_instructions{ 0 };		// current number of instructions used
};


#endif // INSTRUCTION_TABLE_H
