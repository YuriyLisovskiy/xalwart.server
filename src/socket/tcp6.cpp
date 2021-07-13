/**
 * socket/tcp6.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./tcp6.h"

// C++ libraries.
#include <netinet/tcp.h>

// Base libraries.
#include <xalwart.base/exceptions.h>


__SERVER_BEGIN__

void TCP6Socket::bind()
{
	sockaddr_in6 addr{};
	if (inet_pton(this->family, this->address.c_str(), &addr.sin6_addr) <= 0)
	{
		auto err = errno;
		throw SocketError(
			err, "'inet_pton' call failed: " + std::to_string(err), _ERROR_DETAILS_
		);
	}

	addr.sin6_family = this->family;
	addr.sin6_port = htons(this->port);

	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		auto err = errno;
		throw SocketError(
			err, "'bind' call failed: " + std::to_string(err), _ERROR_DETAILS_
		);
	}
}

void TCP6Socket::set_options()
{
	int opt = 1;
	setsockopt(this->sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	BaseSocket::set_options();
}

__SERVER_END__
