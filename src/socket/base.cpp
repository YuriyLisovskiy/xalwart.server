/**
 * socket/base.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./base.h"

// C++ libraries.
#include <fcntl.h>
#include <netinet/tcp.h>
#include <stdexcept>

// Core libraries.
#include <xalwart.core/exceptions.h>


__SERVER_BEGIN__

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
	const char* address, uint16_t port, int family
) : address(address), port(port), family(family), _closed(false)
{
	if ((this->sock = socket(this->family, SOCK_STREAM, 0)) < 0)
	{
		throw SocketError(
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
		auto err = errno;
		switch (err)
		{
			case ENOPROTOOPT:
			case EINVAL:
				break;
			default:
				throw SocketError(
					err, "'setsockopt' call failed: " + std::to_string(err), _ERROR_DETAILS_
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
		throw SocketError(
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
		throw SocketError(
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

__SERVER_END__
