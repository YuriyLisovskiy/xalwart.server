/**
 * socket/tcp6.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TCP/IPv6 socket implementation.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

class TCP6Socket : public BaseSocket
{
protected:
	void bind() override;

public:
	explicit inline TCP6Socket(const char* address, uint16_t port) : BaseSocket(address, port, AF_INET6)
	{
	}

	void set_options() override;
};

__SERVER_END__
