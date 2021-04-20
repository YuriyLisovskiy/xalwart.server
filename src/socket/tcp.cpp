/**
 * socket/tcp.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./tcp.h"

// Core libraries.
#include <xalwart.core/exceptions.h>


__SERVER_BEGIN__

void TCPSocket::bind()
{
	sockaddr_in addr{};
	if (inet_pton(this->family, this->address, &addr.sin_addr) <= 0)
	{
		auto err = errno;
		throw SocketError(
			err, "'inet_pton' call failed: " + std::to_string(err), _ERROR_DETAILS_
		);
	}

	addr.sin_family = this->family;
	addr.sin_port = htons(this->port);

	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		auto err = errno;
		throw SocketError(
			err, "'bind' call failed: " + std::to_string(err), _ERROR_DETAILS_
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

__SERVER_END__
