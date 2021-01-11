/**
 * socket/tcp6.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./tcp6.h"

// C++ libraries.
#include <netinet/tcp.h>

// Core libraries.
#include <xalwart.core/exceptions.h>


__SERVER_BEGIN__

void TCP6Socket::bind()
{
	sockaddr_in6 addr{};
	if (inet_pton(this->family, this->address, &addr.sin6_addr) <= 0)
	{
		auto err = errno;
		throw core::SocketError(
			err, "'inet_pton' call failed: " + std::to_string(err), _ERROR_DETAILS_
		);
	}

	addr.sin6_family = this->family;
	addr.sin6_port = htons(this->port);

	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		auto err = errno;
		throw core::SocketError(
			err, "'bind' call failed: " + std::to_string(err), _ERROR_DETAILS_
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

__SERVER_END__
