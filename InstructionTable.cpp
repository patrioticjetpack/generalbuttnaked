#include "InstructionTable.h"
#include <cassert>
#include <iostream>


using std::cout;
using std::endl;


// The InstructionTable constructor dynamically allocates the fixed size array of instructions.

InstructionTable::InstructionTable()
{
	instruction_table = new InstructionEntry[MAX_INSTRUCTIONS];
}


// The InstructionTable destructor deallocates the instruction array.

InstructionTable::~InstructionTable()
{
	delete[] instruction_table;
}


// Returns the number of instructions added to the instruction table.

int InstructionTable::numInstructions() const
{
	return num_instructions;
}


// Adds the instruction entry argument to the end of the instruction table.
// A message is generated if there is no available entry in the instruction table.

void InstructionTable::addInstruction(const InstructionEntry& instruction)
{
	if (num_instructions < MAX_INSTRUCTIONS) {
		instruction_table[num_instructions] = instruction;
		num_instructions++;
	}
	else {
		cout << "MAX_INSTRUCTIONS (" << MAX_INSTRUCTIONS << ") exceeded" << endl;
	}
}


// Returns the instruction entry at the specified index in the instruction table.
// An assertion is triggered if the index is out of bounds.

InstructionEntry InstructionTable::getInstruction(int index) const
{
	assert(validIndex(index));

	return instruction_table[index];
}


// Returns whether the argument is a valid instruction table index.

bool InstructionTable::validIndex(int index) const
{
	return ((index >= 0) && (index < num_instructions));
}
