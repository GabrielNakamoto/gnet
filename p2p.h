#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <chrono>
#include <mutex>

#include <thread>
#include <vector>

void socketHandler(int sfd);
void peerConnectionHandler();

struct Peer
{
	struct sockaddr_in addr;
	int fd;
	std::string sendBuf;
};

std::vector<Peer> peers;

std::mutex peer_mutex;

void startNode(unsigned short port)
{
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// set to non blocking
	int sfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

	if (sfd == -1)
	{
		perror("Failed to create server socket");
		return;
	}

	if (bind(sfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		perror("Failed to bind server socket");
		return;
	}

	if (listen(sfd, 10) == -1)
	{
		perror("Failed to listen for server connections");
	}

	std::thread _socketHandler(socketHandler, sfd);

	_socketHandler.detach();

	peerConnectionHandler();
}

void peerConnectionHandler()
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

			if (connect(fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) == -1)
			{
				perror("Error connecting to peer");
				return;
			}

			Peer np;

			np.fd = fd;
			np.addr = sockaddr;

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

void socketHandler(int sfd)
{
	const int MAX_PEERS = 20;
	struct pollfd pfds[MAX_PEERS + 1];

	pfds[0].fd = sfd;
	pfds[0].events = POLLIN;

	// milliseconds to wait for event
	int timeout = 500;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		std::lock_guard<std::mutex> lock(peer_mutex);

		int peerLen = peers.size();
		int nfds = std::min(peerLen, MAX_PEERS) + 1;
		// update poll array
		for (int i = 0; i < nfds - 1; ++i)
		{
			if (pfds[i+1].fd == 0)
			{
				pfds[i+1].fd = peers[i].fd;
				pfds[i+1].events = POLLIN | POLLOUT;
			}
		}

		int ret = poll(pfds, nfds, timeout);

		if (ret == -1)
		{
			perror("Error while polling sockets");
			continue;
		}

		// service sockets
		for (int i = 0; i < nfds - 1; ++i)
		{
			if (pfds[i+1].revents & POLLIN)
			{
				char buf[1024];
				int nBytes = recv(peers[i].fd, buf, sizeof(buf), 0);

				if (nBytes == 0)
				{
					printf("Node %d disconnected\n", i);
					peers.erase(peers.begin() + i);
					i--;
					continue;
				} else if (nBytes == -1)
				{
					perror("Error receiving message from peer");
					return;
				} else 
				{
					printf("Received msg: %s from peer %d\n", buf, i);
				}
			}

			if (! peers[i].sendBuf.empty() && pfds[i+1].revents & POLLOUT)
			{
				int nBytes = send(peers[i].fd, &peers[i].sendBuf[0], sizeof(peers[i].sendBuf), 0);
				if (nBytes == 0)
				{
					printf("Node %d disconnected\n", i);
					peers.erase(peers.begin() + i);
					i--;
					continue;
				} else if (nBytes == -1)
				{
					perror("Error sending message to peer");
					return;
				} else 
				{
					peers[i].sendBuf.clear();
				}
			}
		}

		// accept incoming connections
		if (pfds[0].revents & POLLIN)
		{
			// filled in with address of peer socket
			struct sockaddr_in connectionAddr;
			socklen_t len = sizeof(connectionAddr);
			int cfd = accept(sfd, (struct sockaddr*)&connectionAddr, &len);

			if (cfd == -1)
			{
				perror("Error accepting incoming connection");
				// ?
				return;
			}

			std::cout << "Accepted connection to peer socket\n";
			Peer np;
			np.fd = cfd;
			np.addr  = connectionAddr;

			peers.push_back(np);
		}
	}
}
