/**
 * context.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Server context.
 */

#pragma once

// Base libraries.
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"


__SERVER_BEGIN__

struct Context
{
	log::ILogger* logger = nullptr;
	size_t max_body_size = 0;
	size_t workers = 0;
	time_t timeout_sec = 0;
	time_t timeout_usec = 0;
	size_t retries_count = 0;

	inline void normalize()
	{
		if (!this->logger)
		{
			throw NullPointerException("logger is nullptr", _ERROR_DETAILS_);
		}

		if (!this->timeout_sec)
		{
			this->timeout_sec = 3;
		}
	}
};

__SERVER_END__
