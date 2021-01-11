/**
 * socket/unix.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./unix.h"

// C++ libraries.
#include <stdexcept>


__SERVER_BEGIN__

void UnixSocket::bind()
{
	// TODO: UnixSocket::bind()
	throw std::runtime_error("Unix socket is not supported");
}

UnixSocket::UnixSocket(const char* address)
	: BaseSocket(address, 0, AF_UNIX)
{
	// TODO: UnixSocket::UnixSocket(const char* address)
	throw std::runtime_error("Unix socket is not supported");
}

__SERVER_END__
