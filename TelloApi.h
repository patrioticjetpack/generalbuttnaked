#ifndef TELLO_API_H
#define TELLO_API_H


#include <string>
#include <winsock2.h>


// Tello API class version 1.3

// A very basic class to communicate with and control a Tello drone.


class Tello
{
public:			// member functions intended to be used by clients of the class

	Tello();	// constructor
	~Tello();	// destructor

	bool canInitialize();
	bool sendCommand(const std::string& command);
	
private:		// member functions not intended to be used by clients of the class

	void receiveThread();

private:		// data members should always have private scope

	const int      MAX_TIMEOUT{ 5 };	// maximum number of seconds allowed for Tello responses
	const unsigned BUFFER_SIZE{ 256 };	// Tello send and receive buffer sizes

	char* send_buffer{ nullptr };		// used for sending commands to the Tello
	char* receive_buffer{ nullptr };	// used for receiving responses from the Tello

	WSADATA wsa_data;					// used by WSAStartup to indicate the use of sockets
	SOCKET  socket_tello;				// socket for connecting to Tello

	sockaddr_in client_addr{};			// structure for generating client internet address
	sockaddr_in server_addr{};			// structure for generating server internet address
	sockaddr_in tello_addr{};			// used by receiveThread()'s recvfrom() call
};


#endif // TELLO_API_H
