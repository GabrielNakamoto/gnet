#include <cstring>
#include <string>
#include <unistd.h>
#include <stdexcept>

#include "socket.h"

// TODO: error handling

Socket::Socket(Address &address)
	:	address(address)
{
	fileHandle = socket(AF_INET, SOCK_STREAM, 0);

	if (fileHandle == -1)
	{
		throw(std::runtime_error("Failed to acquire socket"));
	}
}

Socket::Socket(unsigned int ip, unsigned short port)
	:	address(ip, port)
{
	fileHandle = socket(AF_INET, SOCK_STREAM, 0);

	if (fileHandle == -1)
	{
		throw(std::runtime_error("Failed to acquire socket"));
	}
}

Socket::Socket(const int fileHandle, struct sockaddr_in addr)
	:	address(address)
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
	struct sockaddr_in sockaddr = address.getSockAddr();
	int res = bind(fileHandle, reinterpret_cast<struct sockaddr*>(&sockaddr), sizeof(sockaddr));

	if (res == -1)
	{
		throw(std::runtime_error("Failed to bind to socket"));
	}
}

void Socket::Connect()
{
	struct sockaddr_in sockaddr = address.getSockAddr();

	int res = connect(fileHandle, reinterpret_cast<struct sockaddr*>(&sockaddr), sizeof(sockaddr));

	if (res == -1)
	{
		throw(std::runtime_error("Failed to connect to socket"));
	}
}

void Socket::Listen(int backlog)
{
	int res = listen(fileHandle, backlog);
	if (res == -1)
	{
		throw(std::runtime_error("Socket failed to listen for connections"));
	}
}

std::unique_ptr<Socket> Socket::Accept()
{
	struct sockaddr_storage connectionAddr;
	socklen_t addrLen = sizeof(connectionAddr);

	const int connectionFileHandle = accept(fileHandle, reinterpret_cast<struct sockaddr *>(&connectionAddr), &addrLen);

	if (connectionFileHandle == -1)
	{
		throw(std::runtime_error("Failed to accept socket connection"));
	}

	return std::make_unique<Socket>(connectionFileHandle, *reinterpret_cast<sockaddr_in *>(&connectionAddr));
}

// make send and recv return booleans or something?
void Socket::Send(std::string &data, size_t bufferSize)
{
	// just get size of data?
	char buf[bufferSize];
	strcpy(buf, data.c_str());

	int res = send(fileHandle, buf, sizeof(buf), 0);

	if (res == -1)
	{
		throw(std::runtime_error("Error sending data to socket"));
	}
}

std::string Socket::Recv(size_t bufferSize)
{
	char buf[bufferSize];

	int res = recv(fileHandle, buf, sizeof(buf), 0);

	if (res == -1)
	{
		throw(std::runtime_error("Error receiving data from socket"));
	}

	return std::string(buf);
}

int Socket::getPort()
{
	struct sockaddr_in sockaddr = address.getSockAddr();

	socklen_t addrLen = sizeof(sockaddr);
	getsockname(fileHandle, reinterpret_cast<struct sockaddr*>(&sockaddr), &addrLen);

	return static_cast<int>(ntohs(sockaddr.sin_port));
}
