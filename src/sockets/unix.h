/**
 * socket/unix.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Unix socket implementation.
 */

#pragma once

#ifdef __unix__

// C++ libraries.
#include <sys/socket.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

// TODO: docs for 'UnixSocket'
class UnixSocket : public BaseSocket
{
public:
	inline explicit UnixSocket(const char* path) : BaseSocket(path, 0, AF_UNIX)
	{
	}

	void close() override;

protected:
	void bind() override;
};

__SERVER_END__

#endif // __unix__
