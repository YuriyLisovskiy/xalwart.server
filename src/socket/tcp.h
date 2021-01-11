/**
 * socket/tcp.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
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
	explicit TCPSocket(const char* address, uint16_t port);
	void set_options() override;
};

__SERVER_END__
