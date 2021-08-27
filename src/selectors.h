/**
 * selectors.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Request selectors implementations.
 */

#pragma once

// Base libraries.
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"

// Server errors.
#include "./abc.h"


__SERVER_BEGIN__

// TODO: docs for 'Selector'
class Selector : public abc::ISelector
{
public:
	explicit Selector(Socket socket, log::ILogger* logger);

	void register_read_event() override;

	void register_write_event() override;

	bool select(uint timeout_seconds, uint timeout_microseconds) override;

protected:
	log::ILogger* logger;
	fd_set readers{};
	fd_set writers{};
	Socket socket;
};

__SERVER_END__
