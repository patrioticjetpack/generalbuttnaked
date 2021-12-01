#include "FlightPlanParse.h"
#include "IntVariableTable.h"
#include "LabelTable.h"
#include "DroneCommandTable.h"
#include "InstructionTable.h"
#include "Tokens.h"
#include <iostream>


using std::cout;
using std::endl;
using std::size_t;
using std::string;
using std::to_string;


// FlightPlanParse class version 1.2

// The FlightPlanParse class parse a flight plan language program and creates four parse tables.


// The FlightPlanParse constructor records references to the four parse tables.
// Entries will be added to these tables during parsing of a FPL program.

FlightPlanParse::FlightPlanParse(IntVariableTable& variables,
	LabelTable& labels,
	DroneCommandTable& drone_commands,
	InstructionTable& instructions) :
	int_variable_table(variables),
	label_table(labels),
	drone_command_table(drone_commands),
	instruction_table(instructions)
{}


// Accepts a string corresponding to the next line in a FPL program, which can be either
// a label or an instruction.
// White space (space and tab characters) are skipped over.
// A '#' character indicates the start of a comment that extends to the end of the line.
// Any tokens appearing in the line are extracted and processed.
// The line must have white space after every token including the last token on the line.
// At present the FPL specification permits from 0 to 3 tokens per line, and a message is
// generated if this is not the case.

void FlightPlanParse::parseLine(const string& line)
{
	const char BLANK{ ' ' };		// Your code should use all of these constants.
	const char TAB{ '\t' };
	const char COMMENT_START{ '#' };
	const char DRONE_START{ '<' };
	const char DRONE_END{ '>' };

	enum class parse_state {
		WHITE_SPACE,
		DRONE_COMMAND,
		OTHER_TOKEN,
		COMMENT
	};


	// TODO: define a new type for the set of parse states.
	// TODO: declare and initialize a parse state variable.
	parse_state parse{ parse_state::WHITE_SPACE };

	const int MAX_TOKENS{ 3 };		// Maximum number of tokens allowed
	string tokens[MAX_TOKENS];		// Array that stores the tokens found on the line
	int num_tokens{ 0 };			// Current number of tokens found on the line
	string token;					// Accumulates characters forming a token
	bool complete_token{ false };	// Set to true when a complete token has been formed

	const size_t n{ line.length() };

	for (size_t i{ 0 }; i < n; i++) {
		const char c = line[i];		// const to catch "c = " errors in if statements
		// TODO: use a switch statement to implement the parse state machine.
		switch (parse) {
		case parse_state::WHITE_SPACE:
			if (c == BLANK || c == TAB) {
				parse = parse_state::WHITE_SPACE;
			}
			else if (c == DRONE_START) {
				token += DRONE_START;
				parse = parse_state::DRONE_COMMAND;
			}
			else if (c == COMMENT_START) {
				parse = parse_state::COMMENT;
			}
			else {
				token += c;
				parse = parse_state::OTHER_TOKEN;
			}
			break;
		case parse_state::DRONE_COMMAND:
			if (c == DRONE_END) {
				token += DRONE_END;
				complete_token = true;
			}
			else {
				token += c;
				parse = parse_state::DRONE_COMMAND;
			}
			break;
		case parse_state::OTHER_TOKEN:
			if (c == BLANK || c == TAB) {
				complete_token = true;
				parse = parse_state::WHITE_SPACE;
			}
			else if (c == COMMENT_START) {
				complete_token = true;
				parse = parse_state::COMMENT;
			}
			else {
				token += c;
				parse = parse_state::OTHER_TOKEN;
			}
			break;
		case parse_state::COMMENT:
			parse = parse_state::COMMENT;
			break;
		}
		if (complete_token) {
			if (num_tokens < MAX_TOKENS) {
				tokens[num_tokens] = token;
			}
			else if (num_tokens == MAX_TOKENS) {
				cout << "Too many operand(s) in line " << addQuotes(line) << endl;
				parse_success = false;
			}
			num_tokens++;
			token.clear();
			complete_token = false;
		}
	}

	if (num_tokens > 0) {
		if (!addLabelOrInstruction(tokens)) {
			if (isOpcode(tokens[0])) {
				cout << "Invalid or missing operand(s) ";
			}
			else if (isLabelDefinition(tokens[0])) {
				cout << "Invalid label definition ";
			}
			else {
				cout << "Unrecognized opcode ";
			}
			cout << "in line " << addQuotes(line) << endl;
			parse_success = false;
		}
	}
}


// Returns whether no errors were encountered in all FPL lines parsed so far.

bool FlightPlanParse::parseSuccess() const
{
	return parse_success;
}


// Writes the instruction table to the console.
// Rather than a raw dump of the instruction table fields, the FPL source code is
// reconstructed by looking up token string values from other tables.
// First, any label associated with the instruction is displayed on a line by itself.
// Next, the opcode is displayed.
// Finally either 0, 1 or 2 operands are displayed as string tokens separated by blanks.
// If the instruction has undeclared or undefined operands then an error message will be written
// instead of the reconstructed instruction.

void FlightPlanParse::displayInstructions() const
{
	const int num_instructions{ instruction_table.numInstructions() };

	if (num_instructions == 0) {
		cout << endl << "The instruction table is empty" << endl;
	}
	else {
		cout << endl << "Reconstructed instruction table:" << endl << endl;
		for (int i{ 0 }; i < num_instructions; i++) {
			string labels{ label_table.instructionIndexToLabels(i) };
			if (labels.length() > 0) {
				cout << labels;
			}
			if (validInstructionOperands(i)) {
				cout << "            " << indexToInstructionLine(i) << endl;
			}
		}
	}
}


// Process an FPL program statement where the argument consists of an array of 1 to 3 string tokens.
// The array should contain a single label token or else an instruction consisting of an opcode token
// followed by 0, 1 or 2 operand tokens.
// Unused tokens should be empty strings.
// A message is generated if the opcode is not recognized or if there is an opcode/operand mismatch.
// Returns whether the sequence of tokens represents a valid label or instruction.

bool FlightPlanParse::addLabelOrInstruction(string tokens[])
{
	bool valid_tokens{ false };

	if (isLabelDefinition(tokens[0])) {
		if (tokens[1].empty() && tokens[2].empty()) {
			size_t i{ tokens[0].rfind(':') };
			if (i < tokens[0].length()) {
				tokens[0].erase(i);
			}
			label_table.labelIsDefined(tokens[0], instruction_table.numInstructions());
			valid_tokens = true;
		}
	}
	else {
		InstructionEntry instruction{ stringToOpcode(tokens[0]), -1, -1, false };
		switch (instruction.opcode) {
		case Opcodes::INT:
			if (isIdentifier(tokens[1]) && isIntConstant(tokens[2])) {
				instruction.operand1 = int_variable_table.defineVariable(tokens[1], tokens[2]);
				instruction.operand2 = stoi(tokens[2]);
				instruction.constant_operand2 = true;
				valid_tokens = true;
			}
			break;
		case Opcodes::ADD:
		case Opcodes::SUB:
		case Opcodes::MUL:
		case Opcodes::DIV:
		case Opcodes::SET:
		case Opcodes::CMP:
			if (isIdentifier(tokens[1])) {
				instruction.operand1 = int_variable_table.lookupVariable(tokens[1]);
				if (isIntConstant(tokens[2])) {
					instruction.operand2 = stoi(tokens[2]);
					instruction.constant_operand2 = true;
					valid_tokens = true;
				}
				else if (isIdentifier(tokens[2])) {
					instruction.operand2 = int_variable_table.lookupVariable(tokens[2]);
					instruction.constant_operand2 = false;
					valid_tokens = true;
				}
			}
			break;
		case Opcodes::BEQ:
		case Opcodes::BNE:
		case Opcodes::BRA:
			if (isIdentifier(tokens[1]) && tokens[2].empty()) {
				instruction.operand1 = label_table.labelIsOperand(tokens[1]);
				valid_tokens = true;
			}
			break;
		case Opcodes::CMD:
			if (isDroneCommand(tokens[1]) && tokens[2].empty()) {
				instruction.operand1 = drone_command_table.addCommand(tokens[1]);
				valid_tokens = true;
			}
			break;
		case Opcodes::NOP:
			if (tokens[2].empty()) {
				if (isIntConstant(tokens[1])) {
					instruction.operand2 = stoi(tokens[1]);
					instruction.constant_operand2 = true;
					valid_tokens = true;
				}
				else if (isIdentifier(tokens[1])) {
					instruction.operand2 = int_variable_table.lookupVariable(tokens[1]);
					instruction.constant_operand2 = false;
					valid_tokens = true;
				}
			}
			break;
		case Opcodes::END:
			if (tokens[1].empty() && tokens[2].empty()) {
				valid_tokens = true;
			}
			break;
		default:
			break;
		}
		if (valid_tokens) {
			instruction_table.addInstruction(instruction);
		}
	}

	return valid_tokens;
}


// Reconstruct and return a string corresponding to instruction table entry specified
// by the index argument.
// The result should appear similar to the line that was originally parsed, except for
// possible differences in white space.
// The opcode and any operands will appear as substrings separated by white space.

string FlightPlanParse::indexToInstructionLine(int index) const
{
	InstructionEntry instruction{ instruction_table.getInstruction(index) };

	string result{ opcodeToString(instruction.opcode) };

	switch (instruction.opcode) {
	case Opcodes::INT:
	case Opcodes::ADD:
	case Opcodes::SUB:
	case Opcodes::MUL:
	case Opcodes::DIV:
	case Opcodes::SET:
	case Opcodes::CMP:
		result += ' ';
		result += int_variable_table.getName(instruction.operand1);
		result += ' ';
		if (instruction.constant_operand2) {
			result += to_string(instruction.operand2);
		}
		else {
			result += int_variable_table.getName(instruction.operand2);
		}
		break;
	case Opcodes::BEQ:
	case Opcodes::BNE:
	case Opcodes::BRA:
		result += ' ';
		result += label_table.getName(instruction.operand1);
		break;
	case Opcodes::CMD:
		result += ' ';
		result += drone_command_table.getCommand(instruction.operand1);
		break;
	case Opcodes::NOP:
		result += ' ';
		if (instruction.constant_operand2) {
			result += to_string(instruction.operand2);
		}
		else {
			result += int_variable_table.getName(instruction.operand2);
		}
		break;
	case Opcodes::END:
		break;
	default:
		break;
	}

	return result;
}


// Return whether the integer variable, label or drone command operand(s) contained
// in the instruction appearing at instruction_table[index] are declared or defined.
// An error message is written if any operands are not declared or defined.

bool FlightPlanParse::validInstructionOperands(int index) const
{
	bool valid_operands{ true };

	InstructionEntry instruction{ instruction_table.getInstruction(index) };

	switch (instruction.opcode) {
	case Opcodes::INT:
	case Opcodes::ADD:
	case Opcodes::SUB:
	case Opcodes::MUL:
	case Opcodes::DIV:
	case Opcodes::SET:
	case Opcodes::CMP:
		if ((!int_variable_table.validIndex(instruction.operand1)) ||
			((!instruction.constant_operand2) && (!int_variable_table.validIndex(instruction.operand2)))) {
			cout << "Undeclared integer variable(s) used in ";
			valid_operands = false;
		}
		break;
	case Opcodes::BEQ:
	case Opcodes::BNE:
	case Opcodes::BRA:
		if ((!label_table.validIndex(instruction.operand1)) ||
			(!instruction_table.validIndex(label_table.getValue(instruction.operand1)))) {
			cout << "Undefined label used in ";
			valid_operands = false;
		}
		break;
	case Opcodes::CMD:
		if (!drone_command_table.validIndex(instruction.operand1)) {
			cout << "Unable to find drone command used in ";
			valid_operands = false;
		}
		break;
	case Opcodes::NOP:
		if ((!instruction.constant_operand2) && (!int_variable_table.validIndex(instruction.operand2))) {
			cout << "Undeclared integer variable used in ";
			valid_operands = false;
		}
		break;
	case Opcodes::END:
		break;
	default:
		break;
	}

	if (!valid_operands) {
		cout << opcodeToString(instruction.opcode) << " instruction at location " << index
			<< " - program execution may fail" << endl;
	}

	return valid_operands;
}
