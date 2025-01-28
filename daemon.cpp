#include <sys/select.h>
#include <functional>
#include <chrono>

#include "daemon.h"

Daemon::Daemon(unsigned long ip, unsigned short port)
	:	serverSocket(ip, port)
{
	serverSocket.Bind();
	serverSocket.Listen(20);
}

Daemon::Daemon(Socket::Address &address)
	:	serverSocket(address)
{
}

Daemon::~Daemon()
{
}

void Daemon::startThreads()
{
	std::thread discover(std::bind(&Daemon::peerDiscoveryThread, this));
	std::thread sockets(std::bind(&Daemon::socketHandlerThread, this));

	discover.join();
	sockets.join();
}

void Daemon::peerDiscoveryThread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::lock_guard<std::mutex> guard(peer_lock);
		// for now just poll stdin for port numbers
		//
		// also poll for messages ig
		std::cout << "\tNew message (0)\n\tAdd peer (1)\n\tPoll sockets (2)\n";

		int opt;

		std::cin >> opt;

		if (opt == 2)
		{
			continue;
		} else if (opt == 0)
		{
			std::cout << "Port number: ";

			int port;

			std::cin >> port;

			bool found = false;

			for (auto &peer : peers)
			{
				if (peer->connection.getPort() == port)
				{
					found = true;

					std::cout << "Message for peer: ";

					std::string message;

					std::cin >> message;

					peer->sendStream += message;

					// break;
				}
			}

			if (! found)
			{
				std::cout << "Failed to find peer at port " << port << std::endl;
				continue;
			}

		} else if(opt == 1)
		{
			std::cout << "Port number: ";
			int port;

			std::cin >> port;
			try
			{
				Socket connection(INADDR_ANY, port);
				auto nPeer = std::make_unique<Node>(connection);
				nPeer->connection.Connect();

				peers.push_back(std::move(nPeer));

				std::cout << "Successfully connected to peer on port: " << port << std::endl;
			} catch (std::exception &e)
			{
				std::cout << e.what();
				continue;
			}
		}
	}
}

/*
 * 	Handles socket i/o and serialization to / from node
 * 	objects and user
 */
void Daemon::socketHandlerThread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

		std::lock_guard<std::mutex> guard(peer_lock);

		std::cout << "Handling sockets\n";
		fd_set peerRecvFS;
		fd_set peerSendFS;

		int maxFd = 0;

		FD_ZERO(&peerRecvFS);
		FD_ZERO(&peerSendFS);

		FD_SET(serverSocket.fileHandle, &peerRecvFS);

		maxFd = std::max(maxFd, serverSocket.fileHandle);

		struct timeval timeout;

		timeout.tv_sec = 0;

		// polling frequency in microseconds
		timeout.tv_usec = 50000;

		// add peers to fd sets
		for (auto &peer : peers)
		{
			FD_SET(peer->getFd(), &peerRecvFS);

			maxFd = std::max(maxFd, peer->getFd());

			if (! peer->sendStream.empty())
				FD_SET(peer->getFd(), &peerSendFS);
		}

		// blocks until one or more sockets are ready for i/o
		int status = select(maxFd, &peerRecvFS, &peerSendFS, NULL, &timeout);


		// read / write to ready peer sockets
		for (auto &peer : peers)
		{
			if (FD_ISSET(peer->getFd(), &peerRecvFS))
			{
				std::cout << "Peer on port " << peer->connection.getPort() << " says " << peer->connection.Recv();
			}

			if (FD_ISSET(peer->getFd(), &peerSendFS))
			{
				peer->connection.Send(peer->sendStream);
			}
		}

		// accept new peer connections
		if (FD_ISSET(serverSocket.fileHandle, &peerRecvFS))
		{
			std::cout << "Received connection from peer\n";
			auto nConnection = serverSocket.Accept();
			auto nPeer = std::make_unique<Node>(*nConnection);

			peers.push_back(std::move(nPeer));
		}
	}
}



/*
 *	Handles messages from peers (ex. block / transaction
 *	transmissions) and to peers (ex. new transaction by
 *	user)
 *
 */

/*
void Daemon::messageHandlerThread()
{

}
*/
