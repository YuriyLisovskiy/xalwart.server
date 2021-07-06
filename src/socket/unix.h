/**
 * socket/unix.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Unix socket implementation.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

class UnixSocket : public BaseSocket
{
protected:
	void bind() override;

public:
	explicit UnixSocket(const char* address);
};

__SERVER_END__
