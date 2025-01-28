#include <iostream>
#include <cstring>

// #include "node.h"
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


void server()
{
	Socket::Address serverAddress(INADDR_ANY, -1);

	Socket handlerSocket(serverAddress);

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

void client(int port)
{
	try
	{
		Socket::Address serverAddress(INADDR_ANY, port);

		Socket connectionSocket(serverAddress);

		connectionSocket.Connect();

		std::cout << "Connected to server\n";

		std::cout << "Message: ";

		std::string message;

		std::cin >> message;

		connectionSocket.Send(message);

	} catch (const std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		server();
	} else if (argc == 2)
	{
		client(strtol(argv[1], NULL, 10));
	} else
	{
		std::cout << "Usage: " << argv[0] << " <optional port number>\n";
		return 0;
	}

	return 0;
}
