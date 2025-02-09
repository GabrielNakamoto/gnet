#ifndef G_SOCKET_H
#define G_SOCKET_H

#include <sys/socket.h>
#include <memory>
#include <unordered_map>

namespace gnet
{

/**
 *
 * RAII wrapper for a socket
 *
 */

class Socket
{
public:

	explicit Socket(unsigned int fd);

	~Socket();

	Socket(Socket &sock) = delete;
	Socket &operator=(Socket &sock) = delete;

	[[nodiscard]] int bind(const sockaddr *addr, socklen_t addrlen) const;
	[[nodiscard]] int connect(const sockaddr *addr, socklen_t addrlen) const;
	[[nodiscard]] int listen(int backlog) const;

	size_t recv(void *data, size_t len, int flags) const;
	size_t send(const void *data, size_t len, int flags) const;


	[[nodiscard]] std::unique_ptr<Socket> accept(sockaddr *addr, socklen_t *addrlen) const;

	using Event = uint16_t;

	static constexpr Event RECV = 0x0001;
	static constexpr Event SEND = 0x0010;

	using EventsPerSock = std::unordered_map<std::shared_ptr<const Socket>, Event>;

//TODO: setup event / poll functionality
// private:

unsigned int fd;
};

}

#endif
