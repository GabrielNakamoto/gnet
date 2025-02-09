#ifndef G_NODE_H
#define G_NODE_H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

#include <vector>
#include <memory>
#include <mutex>

#include "socket.h"

namespace gnet
{

struct Peer
{
	bool connected;
	struct sockaddr_in addr;
	// shared to prevent multi
	// thread access problems
	std::shared_ptr<Socket> sock;
	std::string sendBuf;

	Peer(std::shared_ptr<Socket> sock, struct sockaddr_in addr)
		:	connected(true)
		,	addr(addr)
		,	sock(sock)
	{
	}

	void disconnect()
	{
		connected = false;
		// handle anything that needs to be cleaned up
		// socket automatically closes
	}
};

class Node
{
private:

	std::shared_ptr<Socket> listenSock;
	std::vector<Peer> peers;
	std::vector<int> disconnects;

	mutable std::mutex peer_mutex;
	// int sfd;

public:

	Node(unsigned short port);

	void run();

private:

	void peerConnectionHandlerThread();
	void socketHandlerThread();

	void socketDisconnectPeers();

	Socket::EventsPerSock socketPollEvents(int timeout);
	void socketServicePeers(const Socket::EventsPerSock &eventsPerSock);
	void socketAcceptConnections(const Socket::EventsPerSock &eventsPerSock);
};

} // namespace

#endif
