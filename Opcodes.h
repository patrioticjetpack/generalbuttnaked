#ifndef OPCODES_H
#define OPCODES_H


#include <string>


// The opcodes form an enumerated type that provides a symbolic name for each opcode.
// This is better than assigning a meaningless integer code (such as 2) to each opcode.

enum class Opcodes { UNDEFINED, INT, ADD, SUB, MUL, DIV, SET, CMP, BEQ, BNE, BRA, CMD, NOP, END };


// Conversion functions between opcode strings and the opcode enumeration.

Opcodes     stringToOpcode(const std::string& token);
std::string opcodeToString(Opcodes opcode);


#endif // OPCODES_H
