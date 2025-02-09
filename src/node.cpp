#include <poll.h>
#include <thread>
#include <arpa/inet.h>
#include <chrono>
#include <unordered_map>

#include "node.h"

namespace gnet
{

int sfd;

Node::Node(unsigned short port)
{
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// set to non blocking
	sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (sfd == -1)
	{
		perror("Failed to create server socket");
		return;
	}

	listenSock = std::make_unique<Socket>(sfd);

	if (listenSock->bind((struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
	{
		perror("Failed to bind server socket");
		return;
	}

	if (listenSock->listen(10) == -1)
	{
		perror("Failed to listen for server connections");
	}
}

void Node::run()
{
	std::thread _socketHandler(&Node::socketHandlerThread, this);

	_socketHandler.detach();

	peerConnectionHandlerThread();
}

void Node::peerConnectionHandlerThread()
{
	while (true)
	{
		std::cout << "Add peer (1)\nSend message (2)\n";
		int opt;
		std::cin >> opt;

		if (opt == 1)
		{
			std::cout << "Port: ";
			unsigned short port;
			std::cin >> port;

			struct sockaddr_in sockaddr;
			sockaddr.sin_family = AF_INET;
			sockaddr.sin_port = htons(port);
			sockaddr.sin_addr.s_addr = INADDR_ANY;

			int fd = socket(AF_INET, SOCK_STREAM, 0);

			if (fd == -1)
			{
				perror("Error creating socket");
				return;
			}

			auto sock = std::make_shared<Socket>(fd);

			if (sock->connect((struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
			{
				perror("Error connecting to peer");
				return;
			}

			Peer np(std::move(sock), sockaddr);

			{
				std::lock_guard<std::mutex> lock(peer_mutex);
				peers.push_back(np);
			}

			std::cout << "Connected to peer on port " << port << std::endl;
		} else if (opt == 2)
		{
			std::cout << "Message: ";
			std::string message;
			std::cin >> message;

			std::cout << "Peer index: ";
			int ind;
			std::cin >> ind;

			if (ind < 0 | ind >= peers.size())
			{
				std::cout << "Index out of range\n";
				continue;
			}

			{
				std::lock_guard<std::mutex> lock(peer_mutex);
				peers[ind].sendBuf += message;
			}
		}
	}
}

Socket::EventsPerSock Node::socketPollEvents(int timeout)
{
	size_t nfds = peers.size() + 1;
	struct pollfd pfds[nfds];

	pfds[0].fd = sfd;
	pfds[0].events = POLLIN;

	int i = 1;
	for (const auto &peer : peers)
	{
		pfds[i].fd = peer.sock->fd;	
		pfds[i].events = POLLIN;

		if (! peer.sendBuf.empty())
			pfds[i].events |= POLLOUT;

		i++;
	}

	int ret = poll(pfds, nfds, timeout);

	if (ret == -1)
	{
		perror("Error while polling sockets");
	}

	Socket::EventsPerSock eventsPerSock;

	Socket::Event listenEvents;
	if (pfds[0].revents & POLLIN)
	{
		listenEvents |= Socket::RECV;
	}

	eventsPerSock.emplace(listenSock, listenEvents);

	i = 1;
	for (const auto &peer : peers)
	{
		Socket::Event events;

		if (pfds[i].revents & POLLIN)
			events |= Socket::RECV;

		if (pfds[i].revents & POLLOUT)
			events |= Socket::SEND;

		eventsPerSock.emplace(peer.sock, events);

		i++;
	}

	return eventsPerSock;
}

void Node::socketServicePeers(const Socket::EventsPerSock &eventsPerSock)
{
	for (auto& peer : peers)
	{

		bool canRecv = false;
		bool canSend = false;

		{
			auto it = eventsPerSock.find(peer.sock);

			if (it != eventsPerSock.end())
			{
				canRecv = it->second & Socket::RECV;
				canSend = it->second & Socket::SEND;
			}
		}

		if (canRecv)
		{
			char buf[1024];
			int nBytes = peer.sock->recv(buf, sizeof(buf), 0);

			if (nBytes == 0)
			{
				std::cout << "Node disconnected\n";
			} else 
			{
				std::cout << "Received msg " << std::string(buf) << " from peer\n";
			}
			/*
			if (nBytes == 0)
			{
				printf("Node disconnected\n");
				peers.erase(peers.begin() + i);
				i--;
			} else if (nBytes == -1)
			{
				perror("Error receiving message from peer");
				return;
			} else 
			{
				printf("Received msg: %s from peer", buf);
			}
			*/
		}

		if (canSend)
		{
			int nBytes = peer.sock->send(&peer.sendBuf[0], sizeof(peer.sendBuf), 0);
			if (nBytes == 0)
			{
				/*
				printf("Node %d disconnected\n", i);
				peers.erase(peers.begin() + i);
				i--;
				*/
				continue;
			} else if (nBytes == -1)
			{
				perror("Error sending message to peer");
				return;
			} else 
			{
				peer.sendBuf.clear();
			}
		}

	}
}

void Node::socketAcceptConnections(const Socket::EventsPerSock &eventsPerSock)
{
	bool connectionRequest = false;

	{
		auto it = eventsPerSock.find(listenSock);

		if (it != eventsPerSock.end() && it->second & Socket::RECV)
			connectionRequest = true;
	}

	if (connectionRequest)
	{
		// filled in with address of peer socket
		struct sockaddr_in connectionAddr;
		socklen_t len = sizeof(connectionAddr);

		auto sock = listenSock->accept((struct sockaddr*)&connectionAddr, &len);

		/* TODO:
		if (cfd == -1)
		{
			perror("Error accepting incoming connection");
			// ?
			return;
		}
		*/

		std::cout << "Accepted connection to peer socket\n";

		// auto sock = std::make_shared<Socket>(cfd);
		Peer np(std::move(sock), connectionAddr);

		peers.push_back(np);
	}
}

void Node::socketHandlerThread()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::lock_guard<std::mutex> lock(peer_mutex);

		int timeout = 500;
		Socket::EventsPerSock eventsPerSock = socketPollEvents(timeout);

		socketServicePeers(eventsPerSock);
		socketAcceptConnections(eventsPerSock);
	}
}

} // namespace
