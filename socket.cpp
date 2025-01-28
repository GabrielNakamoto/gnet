#include <cstring>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

#include "socket.h"

// TODO: error handling

Socket::Socket(int protocol, int port, int domain, int service, unsigned long addr)
	:	domain(domain)
	,	service(service)
	,	protocol(protocol)
{
	fileHandle = socket(domain, service, protocol);

	fullAddr.sin_family = domain;

	// host to network byte order conversions
	if (port != -1)
	{
		fullAddr.sin_port = htons(port);
	}

	fullAddr.sin_addr.s_addr = htonl(addr);
}

Socket::Socket(const int fileHandle, struct sockaddr_in addr)
	:	fullAddr(addr)
	,	fileHandle(fileHandle)
{
}

Socket::~Socket()
{
	if (fileHandle != -1)
	{
		close(fileHandle);
	}
}

void Socket::Bind()
{
	bind(fileHandle, getSockaddr(), sizeof(fullAddr));
}

void Socket::Connect()
{
	connect(fileHandle, getSockaddr(), sizeof(fullAddr));
}

void Socket::Listen(int backlog)
{
	listen(fileHandle, backlog);
}

std::unique_ptr<Socket> Socket::Accept()
{
	struct sockaddr_storage connectionAddr;
	socklen_t addrLen = sizeof(connectionAddr);

	const int connectionFileHandle = accept(fileHandle, reinterpret_cast<struct sockaddr *>(&connectionAddr), &addrLen);

	return std::make_unique<Socket>(connectionFileHandle, *reinterpret_cast<sockaddr_in *>(&connectionAddr));
}

void Socket::Send(std::string &data, size_t bufferSize)
{
	// just get size of data?
	char buf[bufferSize];
	strcpy(buf, data.c_str());

	send(fileHandle, buf, sizeof(buf), 0);
}

std::string Socket::Recv(size_t bufferSize)
{
	char buf[bufferSize];

	recv(fileHandle, buf, sizeof(buf), 0);

	return std::string(buf);
}

int Socket::getPort()
{
	socklen_t addrLen = sizeof(fullAddr);
	getsockname(fileHandle, getSockaddr(), &addrLen);

	return static_cast<int>(ntohs(fullAddr.sin_port));
}

sockaddr *Socket::getSockaddr()
{
	return reinterpret_cast<struct sockaddr *>(&fullAddr);
}
