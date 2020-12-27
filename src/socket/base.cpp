/**
 * socket/base.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./base.h"

// C++ libraries.
#include <cerrno>


__SERVER_BEGIN__

BaseSocket::BaseSocket(
	const OnErrorFunc& onError, SocketType sockType, int socketId, bool useIPv6
) : sock(socketId), useIPv6(useIPv6)
{
	if (socketId < 0)
	{
		if ((this->sock = socket(AF_INET, sockType, 0)) < 0)
		{
			onError(errno, "Socket creating error.");
		}
	}
}

void BaseSocket::Close()
{
	if (this->isClosed)
	{
		return;
	}

	this->isClosed = true;
	close(this->sock);
}

std::string BaseSocket::RemoteAddress() const
{
	if (this->useIPv6)
	{
		char ip[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &(this->addr6.sin6_addr), ip, INET6_ADDRSTRLEN);
		return std::string(ip);
	}

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(this->addr.sin_addr), ip, INET_ADDRSTRLEN);
	return std::string(ip);
}

int BaseSocket::RemotePort() const
{
	if (this->useIPv6)
	{
		return ntohs(this->addr6.sin6_port);
	}

	return ntohs(this->addr.sin_port);
}

int BaseSocket::FileDescriptor() const
{
	return this->sock;
}

bool BaseSocket::IsIPv6() const
{
	return this->useIPv6;
}

__SERVER_END__
