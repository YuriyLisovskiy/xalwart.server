/**
 * socket/tcp.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TCP/IPv4 socket implementation.
 */

#pragma once

// C++ libraries.
#if defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#elif _WIN32
#include <winsock32.h>
#endif

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

// TODO: docs for 'TCPSocket'
class TCPSocket : public BaseSocket
{
public:
	inline explicit TCPSocket(const char* address, uint16_t port) : BaseSocket(address, port, AF_INET)
	{
	}

	void set_options() override;

protected:
	void bind() override;
};

__SERVER_END__
