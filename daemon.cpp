#include <sys/select.h>
#include <chrono>

#include "daemon.h"

using std::chrono;

Daemon::Daemon()
{
}

Daemon::~Daemon()
{
}

void Daemon::peerDiscoverThread()
{
	while (true)
	{
		// for now just poll stdin for port numbers
		//
		// also poll for messages ig
		std::cout << "\tNew message (0)\n\tNew peer (1)";

		int opt;

		std::cin >> opt;

		if (opt == 0)
		{
			std::cout << "Port number: ";

			int port;

			std::cin >> port;

			bool found = false;

			std::unique_ptr peerTo;
			for (auto &peer : peers)
			{
				if (peer.connection.getPort() == port)
				{
					found = true;
					peerTo = peer;
					break;
				}
			}

			if (! found)
			{
				std::cout << "Failed to find peer at port " << port << std::endl;
				continue;
			}

			std::cout << "Message for peer: ";

			std::string message;

			std::cin >> message;

			peerTo.sendStream += message;

		} else if(opt == 1)
		{
			std::cout << "Port number: ";
			int port;

			std::cin >> port;
			try
			{
				auto nPeer = std::make_unique<Node>({INADDR_ANY, port});
				peers.push_back(nPeer);

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
		struct fd_set peerRecvFS;
		struct fd_set peerSendFS;

		int maxFd = 0;

		FD_ZERO(&peerRecvFS);
		FD_ZERO(&peerSendFS);

		struct timeval timeout;

		timeout.tv_sec = 0;

		// polling frequency in microseconds
		timeout.tv_usec = 50000;

		// add peers to fd sets
		for (auto &peer : peers)
		{
			FD_SET(peer.getFd(), &peerRecvFS);

			maxFd = std::max(maxFd, peer.getFd());

			if (! peer.sendStream.empty())
				FD_SET(peer.getFd(), &peerSendFS, NULL);
		}

		// blocks until one or more sockets are ready for i/o
		int status = select(maxFd, &peerRecvFS, &peerSendFS, NULL, timeout);


		// read / write to ready peer sockets
		for (auto &peer : peers)
		{
			if (FD_ISSET(peer.getFd(), &peerRecvFS))
			{
				std::cout << "Peer on port " << peer.connection.getPort() << " says " << peer.connection.Recv();
			}

			if (FD_ISSET(peer.getFd(), &peerSendFS))
			{
				peer.connection.Send(peer.sendStream);
			}
		}

		std::this_thread::sleep_for(chrono::seconds(10));
	}
}



/*
 *	Handles messages from peers (ex. block / transaction
 *	transmissions) and to peers (ex. new transaction by
 *	user)
 *
 */
void Daemon::messageHandlerThread()
{

}
