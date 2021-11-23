/**
 * selectors.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Request selectors implementations.
 */

#pragma once

// C++ libraries.
#if defined(__linux__) || defined(__APPLE__)
#include <sys/select.h>
#endif

// Base libraries.
#include <xalwart.base/interfaces/base.h>

// Module definitions.
#include "./_def_.h"

// Server errors.
#include "./interfaces.h"


__SERVER_BEGIN__

// TODO: docs for 'Selector'
class Selector : public ISelector
{
public:
	explicit Selector(Socket socket, xw::ILogger* logger);

	void register_read_event() override;

	void register_write_event() override;

	bool select(uint timeout_seconds, uint timeout_microseconds) override;

protected:
	xw::ILogger* logger;
	fd_set readers{};
	fd_set writers{};
	Socket socket;
};

__SERVER_END__
