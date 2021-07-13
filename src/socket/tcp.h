/**
 * socket/tcp.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TCP/IPv4 socket implementation.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

class TCPSocket : public BaseSocket
{
protected:
	void bind() override;

public:
	explicit inline TCPSocket(const char* address, uint16_t port) : BaseSocket(address, port, AF_INET)
	{
	}

	void set_options() override;
};

__SERVER_END__
