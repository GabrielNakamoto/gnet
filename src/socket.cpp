#include <unistd.h>

#include "socket.h"

namespace gnet
{

Socket::Socket(unsigned int fd)
	:	fd(fd)
{
}

Socket::~Socket()
{
	::close(fd);
}

int Socket::bind(const sockaddr *addr, socklen_t addrlen) const
{
	return ::bind(fd, addr, addrlen);
}

int Socket::connect(const sockaddr *addr, socklen_t addrlen) const
{
	return ::connect(fd, addr, addrlen);
}

int Socket::listen(int backlog) const
{
	return ::listen(fd, backlog);
}


size_t Socket::recv(void *data, size_t len, int flags) const
{
	return ::recv(fd, data, len, flags);
}

size_t Socket::send(const void *data, size_t len, int flags) const
{
	return ::send(fd, data, len, flags);
}

std::unique_ptr<Socket> Socket::accept(sockaddr *addr, socklen_t *addrlen) const
{
	int cfd = ::accept(fd, addr, addrlen);

	return std::make_unique<Socket>(cfd);
}


} // namespace
