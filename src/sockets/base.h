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

// Server libraries.
#include "../abc.h"


__SERVER_BEGIN__

// TODO: docs for 'BaseSocket'
class BaseSocket : public abc::ISocket
{
public:
	// Overridden method must call `BaseSocket::set_options()`
	void set_options() override;

	void listen() const override;

	// Overridden method must call `BaseSocket::close()`
	void close() override;

	[[nodiscard]]
	inline bool is_open() const override
	{
		return !this->_closed;
	}

	[[nodiscard]]
	inline Socket raw_socket() const override
	{
		return this->socket;
	}

protected:
	Socket socket;
	int family;
	std::string address;
	uint16_t port;

	explicit BaseSocket(const char* address, uint16_t port, int family);

private:
	bool _closed;

	[[nodiscard]]
	bool _set_blocking(bool blocking) const;
};

__SERVER_END__
