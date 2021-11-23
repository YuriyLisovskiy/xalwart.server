/**
 * socket/base.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./base.h"

// C++ libraries.
#include <fcntl.h>
#if defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#include <unistd.h>
#elif _WIN32
#include <winsock32.h>
#endif

// Server libraries.
#include "../exceptions.h"
#include "../utility.h"


__SERVER_BEGIN__

void BaseSocket::set_options()
{
	int options = 1;
	::setsockopt(this->socket, SOL_SOCKET, SO_REUSEADDR, &options, sizeof(options));
	if (::setsockopt(this->socket, SOL_SOCKET, SO_REUSEPORT, &options, sizeof(options)))
	{
		auto error_code = errno;
		switch (error_code)
		{
			case ENOPROTOOPT:
			case EINVAL:
				break;
			default:
				throw SocketError(
					error_code, "'setsockopt' call failed: " + std::to_string(error_code), _ERROR_DETAILS_
				);
		}
	}

	this->bind();
	if (!this->_set_blocking(false))
	{
		throw SocketError(
			-1, "unable to set socket to non-blocking mode", _ERROR_DETAILS_
		);
	}
}

void BaseSocket::listen() const
{
	if (::listen(this->socket, SOMAXCONN))
	{
		throw SocketError(errno, "'listen' call failed: " + std::to_string(errno), _ERROR_DETAILS_);
	}
}

void BaseSocket::close()
{
	if (this->_closed)
	{
		return;
	}

	this->_closed = true;
	if (::shutdown(this->socket, SHUT_RDWR))
	{
		throw SocketError(errno, "'shutdown' call failed: " + std::to_string(errno), _ERROR_DETAILS_);
	}

	::close(this->socket);
}

bool BaseSocket::_set_blocking(bool blocking) const
{
	if (!util::socket_is_valid(this->socket))
	{
		return false;
	}

#ifdef _WIN32
	unsigned long mode = blocking ? 0 : 1;
	return ioctlsocket(this->sock, FIONBIO, &mode) == 0;
#else
	int flags = fcntl(this->socket, F_GETFL, 0);
	if (flags == -1) return false;
	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
	return fcntl(this->socket, F_SETFL, flags) == 0;
#endif
}

BaseSocket::BaseSocket(const char* address, uint16_t port, int family) :
	address(address), port(port), family(family), _closed(false)
{
	this->socket = ::socket(this->family, SOCK_STREAM, 0);
	if (!util::socket_is_valid(this->socket))
	{
		auto error_code = errno;
		throw SocketError(error_code, "'socket' call failed: " + std::to_string(error_code), _ERROR_DETAILS_);
	}
}

__SERVER_END__
