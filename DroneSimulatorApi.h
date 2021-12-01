#ifndef DRONE_SIMULATOR_API_H
#define DRONE_SIMULATOR_API_H


#include <mutex>
#include <string>


// Drone Simulator API class version 1.0


// A three-dimensional integer Cartesian coordinate system for the virtual drone.
// The (0, 0, 0) origin starts off in the center of the graphics window.
// Move command coordinates are *relative* to the drone's previous position, just like the Tello drone.
// For example, if the drone is already at position (100, 100, 50) then a move command specifying
// coordinates (0, 0, 0) does not change the position of the drone.
// The x-coordinate controls left (negative) and right (positive) motion.
// The y-coordinate controls bottom of screen (negative) and top of screen (positive) motion.
// The z-coordinate controls altitude, or down (negative) and up (positive) motion.
// It is the programmer's responsibility not to go beyond the screen boundaries.

struct Coordinate3D
{
	int x{ 0 };
	int y{ 0 };
	int z{ 0 };
};


// A very basic class to control a virtual drone with commands issued by the main thread
// and displayed in a window rendered with the SFML 2D graphics library. 

class DroneSimulator
{
public:					// member functions intended to be used by clients of the class

	DroneSimulator();	// constructor

	void sendCommand(const std::string& command);

private:				// member functions not intended to be used by clients of the class

	void renderingThread();
	bool commandReceived(std::string& command, Coordinate3D& relative_3d_coord);

private:				// data members should always have private scope
	
	static const int   WINDOW_WIDTH{ 1200 };	// fixed width  of the drone simulator window in pixels
	static const int   WINDOW_HEIGHT{ 900 };	// fixed height of the drone simulator window in pixels

	static std::mutex  drone_command_mutex;		// ensures thread-safe access to drone_command
	static std::string drone_command;			// commands sent by the main thread and received by the
												// rendering thread
};


#endif // DRONE_SIMULATOR_API_H
