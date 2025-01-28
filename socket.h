#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <memory>


class Socket
{
public:
	struct Address
	{
		private:

			unsigned int ip;
			unsigned short port;

		public:

			Address(unsigned int ip, unsigned short port)
				:	ip(ip)
					,	port(port)
		{
		};

			Address(struct sockaddr_in addr)
			{
				port = ntohs(addr.sin_port);
				ip = ntohl(addr.sin_addr.s_addr);
			}

			struct sockaddr_in getSockAddr() const
			{
				struct sockaddr_in addr;
				addr.sin_family = AF_INET;
				addr.sin_port = htons(port);
				addr.sin_addr.s_addr = htonl(ip);

				return addr;
			};
	};

private:

	Address address;

	int fileHandle = -1;

public:

	// acquire socket resource
	Socket(Address &address);
	Socket(unsigned int ip, unsigned short port);
	Socket(const int fileHandle, struct sockaddr_in address);

	// release socket resource
	~Socket();

	Socket(Socket &&socket) = default;
	Socket& operator=(Socket &&socket) = default;

	Socket(Socket &socket) = delete;
	Socket& operator=(Socket &socket) = delete;

	// sys call wrappers
	void Bind();
	void Connect();
	void Listen(int backlog);

	std::unique_ptr<Socket> Accept();

	// TODO: use templated streams
	void Send(std::string &data, size_t bufferSize = 1024);
	std::string Recv(size_t bufferSize = 1024);

	// returns current address port in host byte order
	int getPort();
};

#endif
