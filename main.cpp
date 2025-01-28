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
}

void client(int port)
{
	std::cout << "Started client\n";
	// servers address
	struct sockaddr_in serv_addr;

	const int fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(static_cast<short>(port));


	// gets local address
	char addrstr[NI_MAXHOST + NI_MAXSERV + 1];
	snprintf(addrstr, sizeof(addrstr), "127.0.0.1:%d", port);

	// convert the text version of the address to binary
	// and store it in the addr struct
	inet_pton(AF_INET, addrstr, &serv_addr.sin_addr);

	if(connect(fd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) == -1)
	{
		std::cerr << "Failed to connect to server\n";
		return;
	}

	std::cout << "Successfuly connected to server\n";
	std::cout << "Message: ";

	std::string message;

	std::cin >> message;

	char buf[1024];
	strcpy(buf, message.c_str());

	send(fd, buf, sizeof(buf), 0);

	close(fd);
}

void server()
{
	// struct that holds the port number, address and transportation protocol
	struct sockaddr_in serv_addr;

	// pointer to socket io file descriptor
	const int fd = socket(AF_INET, SOCK_STREAM, 0);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	// associate the socket (endpoint / file) with necessary transportation info
	if (bind(fd, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) == -1)
	{
		std::cerr << "Failed to bind to address\n";
		return;
	}

	socklen_t addr_len = sizeof(serv_addr);
	getsockname(fd, reinterpret_cast<struct sockaddr *>(&serv_addr), &addr_len);

	int port_number = static_cast<int>(ntohs(serv_addr.sin_port));
	printf("Connected to port %d\n", port_number);

	if (listen(fd, 0) == -1)
	{
		std::cerr << "Server failed to listen\n";
		return;
	}

	while (true)
	{
		struct sockaddr_storage cli_addr;
		socklen_t cli_addr_len = sizeof(cli_addr);
		const int cfd = accept(fd, reinterpret_cast<struct sockaddr *>(&cli_addr), &cli_addr_len);

		std::cout << "Accepted client request\n";

		if (cfd == -1)
		{
			std::cerr << "Failed to connect with client\n";
			return;
		}

		char buf[1024];
		recv(cfd, buf, sizeof(buf), 0);

		std::cout << "\"" << &buf[0] << "\"\n";

		close(cfd);
	}

	close(fd);
}

int main(int argc, char **argv)
{
	if (argc == 1)
	{
		server2();
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
