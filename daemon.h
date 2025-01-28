#ifndef DAEMON_H
#define DAEMON_H

#include <memory>
#include <thread>
#include <mutex>
#include <vector>

#include "socket.h"
#include "node.h"

class Daemon
{
private:

	Socket serverSocket;

	std::mutex peer_lock;
	std::vector<std::unique_ptr<Node> > peers;

public:

	Daemon(unsigned long ip, unsigned short port);
	Daemon(Socket::Address &address);

	~Daemon();

	void startThreads();

	void peerDiscoveryThread();
	void socketHandlerThread();
	// void messageHandlerThread();

};

#endif
