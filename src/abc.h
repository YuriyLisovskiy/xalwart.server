/**
 * abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for server library.
 */

#pragma once

// Module definitions.
#include "./_def_.h"


__SERVER_ABC_BEGIN__

class ISocket
{
public:
	virtual ~ISocket() = default;

	virtual void set_options() = 0;

	virtual void bind() = 0;

	virtual void listen() const = 0;

	virtual void close() = 0;

	[[nodiscard]]
	virtual bool is_open() const = 0;

	[[nodiscard]]
	virtual Socket raw_socket() const = 0;
};

// TODO: docs for 'ISelector'
class ISelector
{
public:
	virtual ~ISelector() = default;

	virtual void register_read_event() = 0;

	virtual void register_write_event() = 0;

	virtual bool select(unsigned int timeout_seconds, unsigned int timeout_microseconds) = 0;
};

class IRequestHandler
{
public:
	virtual ~IRequestHandler() = default;

	virtual void handle() = 0;
};

__SERVER_ABC_END__
