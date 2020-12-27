/**
 * socket/base.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#if defined(__linux__) || defined(__APPLE__)

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#elif _WIN32
#include <winsock32.h>
#endif

#include <string>
#include <functional>

// Module definitions.
#include "../_def_.h"

typedef std::function<void(int, const std::string&)> OnErrorFunc;

#define FDR_UNUSED(expr){ (void)(expr); }
#define FDR_ON_ERROR const OnErrorFunc& onError = [](int errorCode, const std::string& errorMessage){FDR_UNUSED(errorCode); FDR_UNUSED(errorMessage)}


__SERVER_BEGIN__

class BaseSocket
{
protected:
	bool useIPv6;
	int sock = 0;
	sockaddr_in addr{};
	sockaddr_in6 addr6{};
	bool isClosed = false;

public:
	const uint16_t BUFFER_SIZE = 0xFFFF;
	enum SocketType
	{
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM
	};

	explicit BaseSocket(
		FDR_ON_ERROR, SocketType sockType = TCP, int socketId = -1, bool useIPv6 = false
	);

	virtual void Close();

	[[nodiscard]]
	std::string RemoteAddress() const;

	[[nodiscard]]
	int RemotePort() const;

	[[nodiscard]]
	int FileDescriptor() const;

	[[nodiscard]]
	bool IsIPv6() const;
};

__SERVER_END__
