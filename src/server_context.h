/**
 * server_context.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>

// Core libraries.
#include <xalwart.core/logger.h>

// Module definitions.
#include "./_def_.h"


__SERVER_BEGIN__

struct Context
{
	core::ILogger* logger;
	size_t max_body_size = 0;
	std::string media_root;
	size_t workers = 0;
	time_t timeout_sec = 0;
	time_t timeout_usec = 0;

	void normalize();
};

__SERVER_END__
