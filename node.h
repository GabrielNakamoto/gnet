#ifndef G_NODE_H
#define G_NODE_H

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>

#include <vector>
#include <mutex>

namespace gnet
{

struct Peer
{
	struct sockaddr_in addr;
	int fd;
	std::string sendBuf;
};

class Node
{
private:

	std::vector<Peer> peers;
	mutable std::mutex peer_mutex;

	int sfd;

public:

	Node(unsigned short port);

	void run();

private:

	void peerConnectionHandlerThread();
	void socketHandlerThread();
};

} // namespace

#endif
