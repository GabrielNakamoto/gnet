#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

class Socket
{
private:

		struct sockaddr_in fullAddr;

		int fileHandle;

		int port;
		int domain;
		int service;
		int protocol;

public:

	// transportation protocol
	enum class TP
	{
	};

	// creates new socket and populates address
	// domain??
	Socket(int domain, int service, unsigned long addr, int protocol = SOCK_STREAM, int port = INADDR_ANY);
	Socket(struct sockaddr_in addr);

	void bind();
	void connect();
	void listen(int backlog);

	std::unique_ptr<Socket> Socket::accept();

	void send(const std::string &data);
	void recv(const std::string &data);

	int getPort() const;

private:

	sockaddr *getSockaddr() const;
};

#endif
