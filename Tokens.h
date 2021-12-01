#ifndef TOKENS_H
#define TOKENS_H


#include <string>


// A collection of free functions used to determine a token's type (identifier such as a variable name or
// label used as an operand, integer constant, label definition, drone command, or opcode).

bool isIdentifier(const std::string& token);
bool isIntConstant(const std::string& token);
bool isLabelDefinition(const std::string& token);
bool isDroneCommand(const std::string& token);
bool isOpcode(const std::string& token);


// A utility function to surround a token string with double quotes.
// This can help to highlight extraneous white space before or after a token string.

std::string addQuotes(const std::string& token);


#endif // TOKENS_H
