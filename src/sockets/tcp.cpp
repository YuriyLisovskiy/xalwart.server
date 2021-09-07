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

// Server libraries.
#include "../exceptions.h"


__SERVER_BEGIN__

void TCPSocket::set_options()
{
//	int options = 1;
//	setsockopt(this->socket, IPPROTO_TCP, TCP_NODELAY, &options, sizeof(options));
	BaseSocket::set_options();
}

void TCPSocket::bind()
{
	sockaddr_in internet_socket_address{};
	if (::inet_pton(this->family, this->address.c_str(), &internet_socket_address.sin_addr) <= 0)
	{
		auto error_code = errno;
		throw SocketError(error_code, "'inet_pton' call failed: " + std::to_string(error_code), _ERROR_DETAILS_);
	}

	internet_socket_address.sin_family = this->family;
	internet_socket_address.sin_port = htons(this->port);
	if (::bind(this->socket, (const sockaddr*)&internet_socket_address, sizeof(internet_socket_address)))
	{
		auto error_code = errno;
		throw SocketError(error_code, "'bind' call failed: " + std::to_string(error_code), _ERROR_DETAILS_);
	}
}

__SERVER_END__
