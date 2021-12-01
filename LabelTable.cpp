#include "LabelTable.h"
#include "Tokens.h"
#include <cassert>
#include <iostream>
#include <iomanip>


using std::cout;
using std::endl;
using std::left;
using std::right;
using std::string;
using std::setw;


// The LabelTable constructor dynamically allocates the fixed size array of labels.

LabelTable::LabelTable()
{
	label_table = new LabelEntry[MAX_LABELS];
}


// The LabelTable destructor deallocates the label array.

LabelTable::~LabelTable()
{
	delete[] label_table;
}


// Returns the number of labels added to the label table.

int LabelTable::numLabels() const
{
	return num_labels;
}


// Adds the token and its value to the label table if this is the first time the label
// has been encountered.
// The label should not include a ':' character at the end.
// Returns the index of the table entry for the label unless the table is full in which
// case a message is generated and -1 is returned.
// If the label has been previously referenced but not defined, the label's value is set.
// If the label has been previously defined, a message is generated.

int LabelTable::labelIsDefined(const string& token, int value)
{
	int index{ lookupLabel(token) };

	if (index == -1) {
		index = addLabel(token, value);
	}
	else {
		if (label_table[index].value == -1) {
			label_table[index].value = value;
		}
		else {
			cout << "Label " << addQuotes(token) << " is defined more than once - first occurrence used" << endl;
		}
	}

	return index;
}


// Allow label forward references (i.e. a label can be used in a branch instruction
// before the label is defined) by adding the token argument to the label table but
// with the value set to -1.
// The label should not include a ':' character at the end.
// Returns the label's index in the label table.

int LabelTable::labelIsOperand(const string& token)
{
	int index{ lookupLabel(token) };

	if (index == -1) {
		index = addLabel(token, -1);
	}

	return index;
}


// Returns the label string present in the label table entry specified by the index argument.
// An assertion is triggered if the index argument is out of bounds.

string LabelTable::getName(int index) const
{
	assert(validIndex(index));

	return label_table[index].name;
}


// Returns the label value present in the label table entry specified by the index argument.
// An assertion is triggered if the index argument is out of bounds.

int LabelTable::getValue(int index) const
{
	assert(validIndex(index));

	return label_table[index].value;
}


// Returns whether the argument is a valid label table index.

bool LabelTable::validIndex(int index) const
{
	return ((index >= 0) && (index < num_labels));
}


// Returns the label string, if any, associated with an instruction specified by the instruction
// table index argument.
// If there are no labels associated with the specified instruction, an empty string is returned.
// One or more labels can optionally precede an instruction.
// Each label found will be appended with ":\n".

string LabelTable::instructionIndexToLabels(int index) const
{
	string result;

	for (int i{ 0 }; i < num_labels; i++) {
		if (label_table[i].value == index) {
			result += label_table[i].name;
			result += ":\n";
		}
	}

	return result;
}


// Displays the contents of the label table on the console.

void LabelTable::display() const
{
	if (num_labels == 0) {
		cout << endl << "The label table is empty" << endl;
	}
	else {
		cout << endl << "Label table: [index | label name | label value]" << endl << endl;
		for (int i{ 0 }; i < num_labels; i++) {
			cout << right << setw(8)  << i << "    "
				 << left  << setw(24) << addQuotes(label_table[i].name)
				 << right << setw(8)  << label_table[i].value << endl;
		}
	}
}


// Returns the index of the string token argument in the label table, or -1 if the
// label is not found in the label table.
// The label should not include a ':' character at the end.

int LabelTable::lookupLabel(const string& token) const
{
	int index{ -1 };

	for (int i{ 0 }; i < num_labels; i++) {
		if (label_table[i].name == token) {
			index = i;
			break;
		}
	}

	return index;
}


// Adds the token and its value to the end of the label table.
// The label should not include a ':' character at the end.
// Returns the index of the table entry for the label unless the table is full in which
// case a message is generated and -1 is returned.

int LabelTable::addLabel(const string& token, int value)
{
	int index{ -1 };

	if (num_labels < MAX_LABELS) {
		index = num_labels;
		label_table[index].name  = token;
		label_table[index].value = value;
		num_labels++;
	}
	else {
		cout << "MAX_LABELS (" << MAX_LABELS << ") exceeded" << endl;
	}

	return index;
}
