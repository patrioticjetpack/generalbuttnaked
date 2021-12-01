#include "Tokens.h"
#include "Opcodes.h"
#include <cctype>


using std::size_t;
using std::string;


bool isIdentifier(const string& token)
{
	bool result{ isalpha(token[0]) && (token[token.length() - 1] != ':' && !isOpcode(token)) };

	// TODO: Set result to true if the token is an identifier.

	return result;
}


// Returns whether the string token argument represents an integer constant.
// Integer constants are non-empty and contain only digits optionally preceded by a '+' or '-' sign.

bool isIntConstant(const string& token)
{
	bool result{ false };
	if (token[0] == '+' || token[0] == '-' || isdigit(token[0])) {
		result = true;
		for (int i = 1; i < token.length(); i++) {
			if (isdigit(token[i]) && result) {
				result = true;
			}
			else {
				result = false;
			}
		}
	}

	// TODO: Set result to true if the token is an integer constant.

	return result;
}


// Returns whether the string token argument represents a label definition.
// Labels are non-empty, begin with an alphabetic character, and are terminated with a ':' character.
// No other error checking is performed.

bool isLabelDefinition(const string& token)
{
	bool result{ isalpha(token[0]) && (token[token.length() - 1] == ':') };

	// TODO: Set result to true if the token is a label definition.

	return result;
}


// Returns whether the string token argument represents a drone command.
// Drone commands consist of at least 2 characters where the first character is '<'
// and the last character is '>'.
// No other error checking is performed.

bool isDroneCommand(const string& token)
{
	bool result{ token[0] == '<' && token[token.length() - 1] == '>' };

	// TODO: Set result to true if the token is a drone command.

	return result;
}


// Returns whether the string token argument represents an opcode.

bool isOpcode(const string& token)
{
	return (stringToOpcode(token) != Opcodes::UNDEFINED);
}


// Return the string argument surrounded with double quotes.

string addQuotes(const string& token)
{
	return ('"' + token + '"');
}
