/**
 * socket/tcp6.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TCP/IPv6 socket implementation.
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

// TODO: docs for 'TCP6Socket'
class TCP6Socket : public BaseSocket
{
protected:
	void bind() override;

public:
	inline explicit TCP6Socket(const char* address, uint16_t port) : BaseSocket(address, port, AF_INET6)
	{
	}

	void set_options() override;
};

__SERVER_END__
