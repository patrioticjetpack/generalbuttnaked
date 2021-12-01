#include "Opcodes.h"


using std::string;


// Returns the Opcode enumerated type value matching the string token.
// An UNDEFINED_OPCODE value is retuned if the string token is not a valid opcode.

Opcodes stringToOpcode(const string& token)
{
	Opcodes result{ Opcodes::UNDEFINED };

	if (token == "int") {
		result = Opcodes::INT;
	}
	else if (token == "add") {
		result = Opcodes::ADD;
	}
	else if (token == "sub") {
		result = Opcodes::SUB;
	}
	else if (token == "mul") {
		result = Opcodes::MUL;
	}
	else if (token == "div") {
		result = Opcodes::DIV;
	}
	else if (token == "set") {
		result = Opcodes::SET;
	}
	else if (token == "cmp") {
		result = Opcodes::CMP;
	}
	else if (token == "beq") {
		result = Opcodes::BEQ;
	}
	else if (token == "bne") {
		result = Opcodes::BNE;
	}
	else if (token == "bra") {
		result = Opcodes::BRA;
	}
	else if (token == "cmd") {
		result = Opcodes::CMD;
	}
	else if (token == "nop") {
		result = Opcodes::NOP;
	}
	else if (token == "end") {
		result = Opcodes::END;
	}

	return result;
}


// Returns the string matching the Opcode enumerated type argument, including
// "UNDEFINED_OPCODE" if the argument is undefined.

string opcodeToString(Opcodes opcode)
{
	string result;

	switch (opcode) {
	case Opcodes::INT:
		result = "int";
		break;
	case Opcodes::ADD:
		result = "add";
		break;
	case Opcodes::SUB:
		result = "sub";
		break;
	case Opcodes::MUL:
		result = "mul";
		break;
	case Opcodes::DIV:
		result = "div";
		break;
	case Opcodes::SET:
		result = "set";
		break;
	case Opcodes::CMP:
		result = "cmp";
		break;
	case Opcodes::BEQ:
		result = "beq";
		break;
	case Opcodes::BNE:
		result = "bne";
		break;
	case Opcodes::BRA:
		result = "bra";
		break;
	case Opcodes::CMD:
		result = "cmd";
		break;
	case Opcodes::NOP:
		result = "nop";
		break;
	case Opcodes::END:
		result = "end";
		break;
	default:
		result = "UNDEFINED_OPCODE";
		break;
	}

	return result;
}
