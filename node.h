#ifndef NODE_H
#define NODE_H

#include "socket.h"
#include "daemon.h"

/*
 *	Requirements:
 *
 *		-	socket
 *		-	input / output data streams
 *
 */
class Node
{
friend class Daemon;

private:

	Socket connection;

	// data streamed from the peer host
	std::string recvStream;

	// data streamed to the peer host
	std::string sendStream;

public:

	Node(Socket::Address &address);

	int getFd();
};

#endif
