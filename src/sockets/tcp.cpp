/**
 * socket/tcp.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./tcp.h"

// C++ libraries.
#if defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#endif

// Base libraries.
#include <xalwart.base/exceptions.h>


__SERVER_BEGIN__

void TCPSocket::bind()
{
	sockaddr_in addr{};
	if (inet_pton(this->family, this->address.c_str(), &addr.sin_addr) <= 0)
	{
		auto err = errno;
		throw SocketError(err, "'inet_pton' call failed: " + std::to_string(err), _ERROR_DETAILS_);
	}

	addr.sin_family = this->family;
	addr.sin_port = htons(this->port);
	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		auto err = errno;
		throw SocketError(err, "'bind' call failed: " + std::to_string(err), _ERROR_DETAILS_);
	}
}

void TCPSocket::set_options()
{
	int opt = 1;
//	setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	BaseSocket::set_options();
}

__SERVER_END__
