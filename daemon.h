#ifndef DAEMON_H
#define DAEMON_H

#include <memory>
#include <thread>

#include "socket.h"
#include "node.h"

class Daemon
{
private:

	Socket serverSocket;

	std::mutex peer_lock;
	std::vector<std::unique_ptr<Node> > peers;

public:

	Daemon();
	~Daemon();

	void peerDiscoveryThread();
	void socketHandlerThread();
	void messageHandlerThread();

};

#endif
