/**
 * socket/base.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Base wrapper for socket file descriptor.
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
	bool _set_blocking(bool blocking) const;

protected:
	int sock;
	int family;
	std::string address;
	uint16_t port;

protected:
	explicit BaseSocket(const char* address, uint16_t port, int family);
	virtual void bind() = 0;

public:
	// Overridden method must call BaseSocket::set_options()
	virtual void set_options();

	virtual void listen() const;

	// Overridden method must call BaseSocket::close()
	virtual void close();

	[[nodiscard]]
	virtual inline bool is_closed() const
	{
		return this->_closed;
	}

	[[nodiscard]]
	virtual inline int fd() const
	{
		return this->sock;
	}
};

__SERVER_END__
