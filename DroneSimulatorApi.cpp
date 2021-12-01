#include "DroneSimulatorApi.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>


using std::cout;
using std::endl;
using std::to_string;
using std::istringstream;
using std::lock_guard;
using std::mutex;
using std::size_t;
using std::string;
using std::thread;
using std::vector;


// Drone Simulator API class version 1.0


// Static data member definitions:

mutex  DroneSimulator::drone_command_mutex;		// ensures thread-safe access to drone_command

string DroneSimulator::drone_command;			// commands sent by the main thread and received by the
												// rendering thread


// The DroneSimulator constructor creates the rendering thread and allows it to execute independently.

DroneSimulator::DroneSimulator()
{
	// Start the rendering thread.
	thread rendering_thread(&DroneSimulator::renderingThread, this);

	// Detach the rendering thread so it runs concurrently with the main thread.
	rendering_thread.detach();
}


// Send an ASCII text command to the virtual drone.
// This member function should only be called by the application (main) thread.

void DroneSimulator::sendCommand(const string& command)
{
	// Wait for the rendering thread to yield access to the shared drone_command data member.
	lock_guard<mutex> lock(drone_command_mutex);

	// Store a copy of the command.
	drone_command = command;
}


// The window drawing thread that runs concurrently with the main thread.
// However the thread will immediately exit if the font file is not found in the source code directory.
// See https://www.sfml-dev.org for a description of the SFML API.

void DroneSimulator::renderingThread()
{
	const string font_file_name{ "arial.ttf" };

	sf::Font font;

	if (font.loadFromFile(font_file_name)) {
		// Record the last drone command (such as "move" or "land") sent by the application thread.
		string last_command;
		// Maintain a dynamic array of the the drone's 3D relative coordinates specified in drone
		// commands and initialize the array with the origin's coordinates.
		vector<Coordinate3D> relative_3d_coords;
		Coordinate3D relative_3d_coord{ 0, 0, 0 };
		relative_3d_coords.push_back(relative_3d_coord);
		sf::Vector2f absolute_2d_coord(0.0f, 0.0f);
		// Initialize the altimeter text string.
		sf::Text altimeter;
		altimeter.setFont(font);
		altimeter.setCharacterSize(30);
		altimeter.setPosition(absolute_2d_coord);
		altimeter.setFillColor(sf::Color::Black);
		// Define a dynamic array of line segments to display the drone's flight path.
		sf::VertexArray flight_path(sf::LineStrip);
		// Define a small circle to highlight each waypoint in the drone's flight path.
		sf::CircleShape waypoint(3);
		// Create the virtual drone window.
		// See the SFML documentaton for an explanation of the drawing and event polling loops.
		sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Virtual Drone");
		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					window.close();
				}
			}
			// Check if a new drone command was received.
			if (commandReceived(last_command, relative_3d_coord)) {
				if (last_command == "move") {
					relative_3d_coords.push_back(relative_3d_coord);
				}
			}
			// Refresh the window.
			window.clear(sf::Color::Green);
			// Set the initial absolute coordinate to the origin.
			Coordinate3D absolute_3d_coord{ (WINDOW_WIDTH / 2), (WINDOW_HEIGHT / 2), 0 };
			// Draw the flight path waypoints.
			const size_t n{ relative_3d_coords.size() };
			flight_path.resize(n);
			for (size_t i{ 0 }; i < n; i++) {
				absolute_3d_coord.x += relative_3d_coords[i].x;
				absolute_3d_coord.y -= relative_3d_coords[i].y;
				absolute_3d_coord.z += relative_3d_coords[i].z;
				absolute_2d_coord.x = float(absolute_3d_coord.x);
				absolute_2d_coord.y = float(absolute_3d_coord.y);
				flight_path[i].position = absolute_2d_coord;
				flight_path[i].color    = sf::Color::Black;
				absolute_2d_coord.x -= 3.0f;
				absolute_2d_coord.y -= 3.0f;
				waypoint.setPosition(absolute_2d_coord);
				waypoint.setFillColor(sf::Color::Blue);
				window.draw(waypoint);
			}
			// Highlight the drone's current position.
			waypoint.setFillColor(sf::Color::Red);
			window.draw(waypoint);
			// Regenerate the line segments and points that display the flight path.
			window.draw(flight_path);
			// Display the drone's current altitude.
			if (last_command == "land") {
				absolute_3d_coord.z = 0;
			}
			altimeter.setString("Altitude: " + to_string(absolute_3d_coord.z));
			window.draw(altimeter);
			// Redraw the window with the updated waypoints and altitude.
			window.display();
		}
	}
	else {
		cout << endl << "Unable to load font file " << font_file_name << " - drone simulator not started" << endl;
	}
}


// Return whether a drone command has been sent from the main thread to the graphics rendering thread.
// If this has occurred then the first argument is set to the command name (such as "move" or "land").
// If the command is "move", the relative offset is returned thru the second argument.
// This member function should only be called by the rendering thread.

bool DroneSimulator::commandReceived(string& command, Coordinate3D& relative_3d_coord)
{
	// Wait for the main thread to yield access to the shared drone_command data member.
	lock_guard<mutex> lock(drone_command_mutex);

	bool command_received{ false };

	if (drone_command.length() > 0) {
		command_received = true;
		istringstream iss_command(drone_command);
		iss_command >> command;
		if (command == "move") {
			iss_command >> relative_3d_coord.x >> relative_3d_coord.y >> relative_3d_coord.z;
		}
		drone_command.clear();
	}

	return command_received;
}
