#include <iostream>
#include <string>

#include "p2p.h"

/*
 * 	Where do sockets come into play in a P2P network?
 *
 * 	Each node simply has a socket for each of its peers
 * 	in order to:
 *
 * 	-	transmits a message
 * 	-	receives a message
 *
 * 	So it either uses a read() / write() sys call on it's
 * 	peers sockets
 *
 *
 * 	But each node also has to have it's own socket?
 *
 * 	Does a client implicitly have its own when it calls connect() with
 * 	the server socket?
 */


int main(int argc, char **argv)
{
	if (argc == 2)
	{
		unsigned short port = strtol(argv[1], NULL, 10);

		startNode(port);
	} else
	{
		std::cout << "Usage: " << argv[0] << " <host port>\n";
		return 0;
	}

	return 0;
}

