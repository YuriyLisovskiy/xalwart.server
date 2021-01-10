/**
 * sock.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./sock.h"

// C++ libraries.
#include <fcntl.h>
#include <netinet/tcp.h>
#include <stdexcept>

// Core libraries.
#include <xalwart.core/exceptions.h>


__SERVER_BEGIN__

// BaseSocket
bool BaseSocket::_set_blocking(bool blocking)
{
	if (this->sock < 0)
	{
		return false;
	}

#ifdef _WIN32
	unsigned long mode = blocking ? 0 : 1;
	return ioctlsocket(this->sock, FIONBIO, &mode) == 0;
#else
	int flags = fcntl(this->sock, F_GETFL, 0);
	if (flags == -1) return false;
	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	return fcntl(this->sock, F_SETFL, flags) == 0;
#endif
}

BaseSocket::BaseSocket(
	const char *address, uint16_t port, int family
) : address(address), port(port), family(family), _closed(false)
{
	if ((this->sock = socket(this->family, SOCK_STREAM, 0)) < 0)
	{
		throw core::SocketError(
			errno, "'socket' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}
}

void BaseSocket::set_options()
{
	int opt = 1;
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)))
	{
		switch (errno)
		{
			case ENOPROTOOPT:
			case EINVAL:
				break;
			default:
				throw core::SocketError(
					errno, "'setsockopt' call failed: " + std::to_string(errno), _ERROR_DETAILS_
				);
		}
	}

	this->bind();
	this->_set_blocking(false);
}

void BaseSocket::listen()
{
	if (::listen(this->sock, SOMAXCONN))
	{
		throw core::SocketError(
			errno, "'listen' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}
}

void BaseSocket::close()
{
	if (this->_closed)
	{
		return;
	}

	this->_closed = true;
	if (shutdown(this->sock, SHUT_RDWR))
	{
		throw core::SocketError(
			errno, "'shutdown' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}

	::close(this->sock);
}

bool BaseSocket::is_closed() const
{
	return this->_closed;
}

int BaseSocket::fd() const
{
	return this->sock;
}

// TCPSocket
void TCPSocket::bind()
{
	sockaddr_in addr{};
	if (inet_pton(this->family, this->address, &addr.sin_addr) <= 0)
	{
		throw core::SocketError(
			errno, "'inet_pton' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}

	addr.sin_family = this->family;
	addr.sin_port = htons(this->port);

	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		throw core::SocketError(
			errno, "'bind' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}
}

TCPSocket::TCPSocket(const char* address, uint16_t port)
	: BaseSocket(address, port, AF_INET)
{
}

void TCPSocket::set_options()
{
	int opt = 1;
//	setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	BaseSocket::set_options();
}

// TCP6Socket
void TCP6Socket::bind()
{
	sockaddr_in6 addr{};
	if (inet_pton(this->family, this->address, &addr.sin6_addr) <= 0)
	{
		throw core::SocketError(
			errno, "'inet_pton' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}

	addr.sin6_family = this->family;
	addr.sin6_port = htons(this->port);

	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		throw core::SocketError(
			errno, "'bind' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}
}

TCP6Socket::TCP6Socket(const char* address, uint16_t port)
	: BaseSocket(address, port, AF_INET6)
{
}

void TCP6Socket::set_options()
{
	int opt = 1;
	setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	BaseSocket::set_options();
}

// UnixSocket
void UnixSocket::bind()
{
	// TODO: UnixSocket::bind()
	throw std::runtime_error("Unix socket is not supported");
}

UnixSocket::UnixSocket(const char* address)
	: BaseSocket(address, 0, AF_UNIX)
{
	// TODO: UnixSocket::UnixSocket(const char* address)
	throw std::runtime_error("Unix socket is not supported");
}

__SERVER_END__
