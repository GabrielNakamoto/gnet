#include <sys/select.h>

#include "daemon.h"


Daemon::Daemon()
{
}

Daemon::~Daemon()
{
}

void Daemon::peerDiscoverThread()
{
	// for now just poll stdin for port numbers
}

/*
 * 	Handles socket i/o and serialization to / from node
 * 	objects and user
 */
void Daemon::socketHandlerThread()
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

		/*
		if (! peer.sendStream.empty())
			FD_SET(peer.getFd(), &peerSendFS, NULL);
		*/
	}

	// blocks until one or more sockets are ready for i/o
	int status = select(maxFd, &peerRecvFS, &peerSendFS, NULL, timeout);


	// read from ready peer sockets
	for (auto &peer : peers)
	{
		if (! FD_ISSET(peer.getFd(), &peerRecvFS))
			continue;

		std::cout << "Peer on port " << peer.connection.getPort() << " says " << peer.connection.Recv();
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
