#ifndef DRONE_COMMAND_TABLE_H
#define DRONE_COMMAND_TABLE_H


#include <string>


// Drone command tokens including the '<' and '>' delimiters are stored in a table in the order
// they were parsed.
// There is a single table entry for multiple occurrences of the same drone command token.


class DroneCommandTable
{
public:		// member functions intended to be used by clients of the class

	DroneCommandTable();		// constructor
	~DroneCommandTable();		// destructor

	int         numCommands() const;
	int         lookupCommand(const std::string& token) const;
	int         addCommand(const std::string& token);
	std::string getCommand(int index) const;
	bool        validIndex(int index) const;
	void        display() const;

private:	// data members should always have private scope

	std::string* drone_command_table{ nullptr };	// dynamically allocated array of drone commands
	const int    MAX_DRONE_COMMANDS{ 5000 };		// maximum number of drone commands allowed
	int          num_drone_commands{ 0 };			// current number of drone commands used
};


#endif // DRONE_COMMAND_TABLE_H
