/**
 * tcp_server.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./tcp_server.h"


__SERVER_BEGIN__

TCPServer::TCPServer(
	const OnErrorFunc& onError
) : BaseSocket(onError, TCP, -1)
{
	int opt = 1;
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
}

bool TCPServer::Bind(int port, const OnErrorFunc& onError)
{
	return this->Bind("0.0.0.0", port, onError);
}

bool TCPServer::Bind6(int port, const OnErrorFunc& onError)
{
	this->useIPv6 = true;
	return this->Bind6("::1", port, onError);
}

bool TCPServer::Bind(const char* address, uint16_t port, const OnErrorFunc& onError)
{
	if (inet_pton(AF_INET, address, &this->addr.sin_addr) <= 0)
	{
		onError(errno, "Invalid address. Address type not supported.");
		return false;
	}

	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);

	if (bind(this->sock, (const sockaddr *)&this->addr, sizeof(this->addr)) < 0)
	{
		onError(errno, "Cannot bind the socket.");
		return false;
	}

	return true;
}

bool TCPServer::Bind6(const char* address, uint16_t port, const OnErrorFunc& onError)
{
	this->useIPv6 = true;
	if (inet_pton(AF_INET6, address, &this->addr6.sin6_addr) <= 0)
	{
		onError(errno, "Invalid address. Address type not supported.");
		return false;
	}

	this->addr6.sin6_family = AF_INET6;
	this->addr6.sin6_port = htons(port);

	if (bind(this->sock, (const sockaddr *)&this->addr6, sizeof(this->addr6)) < 0)
	{
		onError(errno, "Cannot bind the socket.");
		return false;
	}

	return true;
}

bool TCPServer::Listen(const OnErrorFunc& onError)
{
	if (listen(this->sock, SOMAXCONN) < 0)
	{
		onError(errno, "Error: Server can't listen the socket.");
		return false;
	}

	return this->_accept(onError);
}

void TCPServer::Close()
{
	shutdown(this->sock, SHUT_RDWR);
	BaseSocket::Close();
}

bool TCPServer::_accept(const OnErrorFunc& onError)
{
	if (this->useIPv6)
	{
		sockaddr_in6 newSocketInfo{};
		socklen_t newSocketInfoLength = sizeof(newSocketInfo);

		int newSock;
		while (!this->isClosed)
		{
			while ((newSock = ::accept(this->sock, (sockaddr *)&newSocketInfo, &newSocketInfoLength)) < 0)
			{
				if (errno == EBADF || errno == EINVAL)
				{
					return false;
				}

				onError(errno, "Error while accepting a new connection.");
				return false;
			}

			if (!this->isClosed && newSock >= 0)
			{
				auto newSocket = std::make_shared<TCPSocket>(
					[](int e, const std::string& er) { FDR_UNUSED(e); FDR_UNUSED(er); }, newSock, this->useIPv6
				);
				newSocket->setAddress6Struct(newSocketInfo);

				this->onNewConnection(newSocket);
			}
		}
	}
	else
	{
		sockaddr_in newSocketInfo{};
		socklen_t newSocketInfoLength = sizeof(newSocketInfo);

		int newSock;
		while (!this->isClosed)
		{
			while ((newSock = ::accept(this->sock, (sockaddr *)&newSocketInfo, &newSocketInfoLength)) < 0)
			{
				if (errno == EBADF || errno == EINVAL)
				{
					return false;
				}

				onError(errno, "Error while accepting a new connection.");
				return false;
			}

			if (!this->isClosed && newSock >= 0)
			{
				auto newSocket = std::make_shared<TCPSocket>(
					[](int e, const std::string& er) { FDR_UNUSED(e); FDR_UNUSED(er); }, newSock
				);
				newSocket->setAddressStruct(newSocketInfo);

				this->onNewConnection(newSocket);
			}
		}
	}

	return true;
}

__SERVER_END__
