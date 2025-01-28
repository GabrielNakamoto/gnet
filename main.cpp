#include <iostream>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>

#include "socket.h"

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

void server2()
{
	Socket handlerSocket(0);

	handlerSocket.Bind();

	std::cout << "Connected to port " << handlerSocket.getPort() << std::endl;

	handlerSocket.Listen(1);

	while (true)
	{
		std::unique_ptr<Socket> connectionSocket = handlerSocket.Accept();	

		std::cout << "Accepted client connection\n";

		const std::string message = connectionSocket->Recv();
		std::cout << "\"" << message << "\"\n";
	}
}

void client2(int port)
{
	Socket connectionSocket(0, port);

	connectionSocket.Connect();

	std::cout << "Connected to server\n";

	std::cout << "Message: ";

	std::string message;

	std::cin >> message;

	connectionSocket.Send(message);
}

/*
	// gets local address
	char addrstr[NI_MAXHOST + NI_MAXSERV + 1];
	snprintf(addrstr, sizeof(addrstr), "127.0.0.1:%d", port);

	// convert the text version of the address to binary
	// and store it in the addr struct
	inet_pton(AF_INET, addrstr, &serv_addr.sin_addr);
*/

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		server2();
	} else if (argc == 2)
	{
		client2(strtol(argv[1], NULL, 10));
	} else
	{
		std::cout << "Usage: " << argv[0] << " <optional port number>\n";
		return 0;
	}

	return 0;
}
