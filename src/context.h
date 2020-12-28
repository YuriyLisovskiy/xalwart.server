/**
 * context.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
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
	OnErrorFunc on_error;
	std::shared_ptr<core::ILogger> logger;
	size_t max_body_size = 0;
	std::string media_root;
	size_t threads_count = 0;
	bool verbose = false;
	size_t timeout_sec = 2;
	size_t timeout_us = 0;

	void normalize();
};

__SERVER_END__
