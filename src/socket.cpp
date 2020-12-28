/**
 * socket.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./socket.h"

// C++ libraries.
#include <cerrno>


__SERVER_BEGIN__

BaseSocket::BaseSocket(
	const OnErrorFunc& on_error, SocketType sock_type, int socket_id, bool use_ipv6
) : sock(socket_id), use_ipv6(use_ipv6)
{
	if (socket_id < 0)
	{
		if (this->use_ipv6)
		{
			if ((this->sock = socket(AF_INET6, sock_type, 0)) < 0)
			{
				on_error(errno, "IPv6 socket creating error");
			}
		}
		else
		{
			if ((this->sock = socket(AF_INET, sock_type, 0)) < 0)
			{
				on_error(errno, "IPv4 socket creating error");
			}
		}
	}
}

void BaseSocket::close()
{
	if (this->is_closed)
	{
		return;
	}

	this->is_closed = true;
	::close(this->sock);
}

std::string BaseSocket::remote_address() const
{
	if (this->use_ipv6)
	{
		char ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(this->addr6.sin6_addr), ip, INET6_ADDRSTRLEN);
		return std::string(ip);
	}

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr.sin_addr), ip, INET_ADDRSTRLEN);
	return std::string(ip);
}

int BaseSocket::remote_port() const
{
	if (this->use_ipv6)
	{
		return ntohs(this->addr6.sin6_port);
	}

	return ntohs(this->addr.sin_port);
}

int BaseSocket::file_descriptor() const
{
	return this->sock;
}

bool BaseSocket::is_ipv6() const
{
	return this->use_ipv6;
}

__SERVER_END__
