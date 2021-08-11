/**
 * socket/base.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Base wrapper for socket file descriptor.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "../_def_.h"


__SERVER_BEGIN__

// TODO: docs for 'BaseSocket'
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
	// Overridden method must call `BaseSocket::set_options()`
	virtual void set_options();

	virtual void listen() const;

	// Overridden method must call `BaseSocket::close()`
	virtual void close();

	[[nodiscard]]
	inline virtual bool is_closed() const
	{
		return this->_closed;
	}

	[[nodiscard]]
	inline virtual int fd() const
	{
		return this->sock;
	}
};

__SERVER_END__
