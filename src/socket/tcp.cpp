/**
 * socket/tcp.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./tcp.h"

// C++ libraries.
#include <cstring>


__SERVER_BEGIN__

TCPSocket::TCPSocket(const OnErrorFunc& onError, int socketId, bool useIPv6)
	: BaseSocket(onError, TCP, socketId, useIPv6)
{
}

int TCPSocket::Send(const std::string& message)
{
	return this->Send(message.c_str(), message.length());
}

int TCPSocket::Send(const char* bytes, size_t len)
{
	if (this->isClosed)
	{
		return -1;
	}

	int sent;
	if ((sent = send(this->sock, bytes, len, 0)) < 0)
	{
		perror("send");
	}

	return sent;
}

void TCPSocket::Connect(
	const char* host, uint16_t port,
	const std::function<void()>& onConnected,
	const OnErrorFunc& onError
)
{
	if (this->useIPv6)
	{
		this->addr6.sin6_family = AF_INET;
		this->addr6.sin6_port = htons(port);
		::inet_pton(PF_INET6, host, (void*) &this->addr6.sin6_addr.s6_addr);

		// Try to connect.
		if (connect(this->sock, (const sockaddr*) &this->addr6, sizeof(sockaddr_in6)) < 0)
		{
			onError(errno, "Connection failed to the host.");
			return;
		}
	}
	else
	{
		this->addr.sin_family = AF_INET;
		this->addr.sin_port = htons(port);
		::inet_pton(AF_INET, host, (void*) &this->addr.sin_addr.s_addr);

		// Try to connect.
		if (connect(this->sock, (const sockaddr*) &this->addr, sizeof(sockaddr_in)) < 0)
		{
			onError(errno, "Connection failed to the host.");
			return;
		}
	}

	// Connected to the server, fire the event.
	onConnected();

	// Start listening from server:
	this->Listen();
}

void TCPSocket::Listen()
{
	// Start listening the socket from thread.
	std::thread receiveListening(Receive, this);
	receiveListening.detach();
}

void TCPSocket::setAddressStruct(sockaddr_in addr)
{
	this->addr = addr;
}

void TCPSocket::setAddress6Struct(sockaddr_in6 addr)
{
	this->addr6 = addr;
}

void TCPSocket::Receive(TCPSocket* socket)
{
	char tempBuffer[socket->BUFFER_SIZE];
	int messageLength;

	while ((messageLength = recv(socket->sock, tempBuffer, socket->BUFFER_SIZE, 0)) > 0)
	{
		tempBuffer[messageLength] = '\0';
		if (socket->onMessageReceived)
		{
			socket->onMessageReceived(std::string(tempBuffer).substr(0, messageLength));
		}

		if (socket->onRawMessageReceived)
		{
			socket->onRawMessageReceived(tempBuffer, messageLength);
		}
	}

	socket->Close();
	if (socket->onSocketClosed)
	{
		socket->onSocketClosed();
	}
}

__SERVER_END__
