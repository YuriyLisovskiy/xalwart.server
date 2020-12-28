/**
 * socket.h
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
#include "./_def_.h"

#define MAX_BUFF_SIZE 8192 * 8 - 1  // 65535 bytes.

// 4K per header value.
// 35 main headers and 3 additional.
// 403 bytes is a length of all headers names.
#define MAX_HEADERS_SIZE 4096 * 38 + 403    // 156051 bytes.


__SERVER_BEGIN__

class BaseSocket
{
protected:
	bool use_ipv6;
	int sock = 0;
	sockaddr_in addr{};
	sockaddr_in6 addr6{};
	bool is_closed = false;

public:
	const uint16_t BUFFER_SIZE = 0xFFFF;
	enum SocketType
	{
		TCP = SOCK_STREAM,
		UDP = SOCK_DGRAM
	};

	explicit BaseSocket(
		const OnErrorFunc& on_error, SocketType sock_type = TCP, int socket_id = -1, bool use_ipv6 = false
	);

	virtual void close();

	[[nodiscard]]
	std::string remote_address() const;

	[[nodiscard]]
	int remote_port() const;

	[[nodiscard]]
	int file_descriptor() const;

	[[nodiscard]]
	bool is_ipv6() const;
};

__SERVER_END__
