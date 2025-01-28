#include "socket.h"

#include <memory>

// TODO: error handling

Socket::Socket(int domain, int protocl, int port, int domain = AF_INET, int service = SOCK_STREAM, unsigned long addr = INADDR_ANY)
	:	domain(domain)
	,	service(service)
	,	protocol(protocol)
{
	fileHandle = socket(domain, service, protocol);

	fullAddr.sin_family = domain;

	// host to network byte order conversions
	fullAddr.sin_port = htons(port);
	fullAddr.sin_addr.s_addr = htonl(addr);
}

Socket::Socket(const int fileHandle, struct sockaddr_in addr)
	:	fullAddr(addr)
	,	fileHandle(fileHandle)
{
}

void Socket::bind()
{
	bind(fileHandle, reinterpret_cast<struct sockaddr *>(&fullAddr), sizeof(fullAddr));
}

void Socket::connect()
{
	connect(fileHandle, reinterpret_cast<struct sockaddr *?>(&fullAddr), sizeof(fullAddr));
}

void Socket::listen(int backlog)
{
	listen(fileHandle, backlog);
}

std::unique_ptr<Socket> Socket::accept()
{
	struct sockaddr_storage connectionAddr;
	socklen_t addrLen = sizeof(connectionAddr);

	const int connectionFileHandle = accept(fileHandle, reinterpret_cast<struct sockaddr *>(&connectionAddr), &addrLen);

	return std::make_unique(Socket(static_cast<sockaddr_in>(connectionAddr), connectionFileHandle);
}

void Socket::send(const std::string &data, size_t bufferSize = 1024)
{
	char buf[bufferSize];
	strcpy(buf, data.c_str());

	send(fileHandle, buf, sizeof(buf), 0);
}

void Socket::recv(const std::string &data, size_t bufferSize = 1024)
{
	char buf[bufferSize];
	strcpy(buf, data.c_str());

	recv(fileHandle, buf, sizeof(buf), 0);
}

int Socket::getPort() const
{
	socklen_t addrLen = sizeof(fullAddr);
	getsockname(fileHandle, getSockaddr(), &addrLen);

	return static_cast<int>(ntohs(fullAddr.sin_port));
}

sockaddr *Socket::getSockaddr() const
{
	return reinterpret_cast<struct sockaddr *>(&fullAddr);
}
