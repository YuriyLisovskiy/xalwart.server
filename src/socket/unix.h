/**
 * socket/unix.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Unix socket implementation.
 */

#pragma once

#include <xalwart.core/sys.h>

#ifdef __unix__

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
	explicit inline UnixSocket(const char* path) : BaseSocket(path, 0, AF_UNIX)
	{
	}

	void close() override;
};

__SERVER_END__

#endif // __unix__
