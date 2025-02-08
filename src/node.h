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
	struct sockaddr_in addr;
	// shared to prevent multi
	// thread access problems
	std::shared_ptr<Socket> sock;
	// int fd;
	std::string sendBuf;

	Peer(std::shared_ptr<Socket> sock, struct sockaddr_in addr)
		:	addr(addr)
		,	sock(sock)
	{
	}
};

class Node
{
private:

	std::unique_ptr<Socket> listenSock;
	std::vector<Peer> peers;

	mutable std::mutex peer_mutex;
	// int sfd;

public:

	Node(unsigned short port);

	void run();

private:

	void peerConnectionHandlerThread();
	void socketHandlerThread();
};

} // namespace

#endif
