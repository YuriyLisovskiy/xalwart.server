/**
 * socket/base.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
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

// Module definitions.
#include "../_def_.h"


__SERVER_BEGIN__

class BaseSocket
{
private:
	bool _closed;

private:
	bool _set_blocking(bool blocking);

public:
	int sock;

protected:
	int family;
	const char* address;
	uint16_t port;

protected:
	explicit BaseSocket(const char* address, uint16_t port, int family);
	virtual void bind() = 0;

public:
	// Overridden method must call BaseSocket::set_options()
	virtual void set_options();

	void listen();

	// Overridden method must call BaseSocket::close()
	virtual void close();

	[[nodiscard]]
	bool is_closed() const;

	[[nodiscard]]
	int fd() const;
};

__SERVER_END__
