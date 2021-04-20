/**
 * server_context.cpp
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 */

#include "./server_context.h"


__SERVER_BEGIN__

void Context::normalize()
{
	if (!this->logger)
	{
		throw NullPointerException("logger must be instantiated");
	}

	if (!this->timeout_sec)
	{
		this->timeout_sec = 3;
	}
}

__SERVER_END__
