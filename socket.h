#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>

#include <memory>

// TODO: exceptions / error handling
class Socket
{
private:

		struct sockaddr_in fullAddr;

		int fileHandle = -1;

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
	Socket(int protocol, int port = -1, int domain = AF_INET, int service = SOCK_STREAM, unsigned long addr = INADDR_ANY);
	Socket(const int fileHandle, struct sockaddr_in addr);

	~Socket();

	void Bind();
	void Connect();
	void Listen(int backlog);

	std::unique_ptr<Socket> Accept();

	// TODO: use templated streams
	void Send(std::string &data, size_t bufferSize = 1024);
	std::string Recv(size_t bufferSize = 1024);

	int getPort();

private:

	sockaddr *getSockaddr();
};

#endif
