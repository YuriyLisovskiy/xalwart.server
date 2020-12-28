/**
 * context.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./context.h"

// C++ libraries.
#include <iostream>


__SERVER_BEGIN__

void Context::normalize()
{
	if (!this->on_error)
	{
		this->on_error = [](int err, const std::string& msg)
		{
			std::cerr << "error " << err << ": " << msg << std::endl;
		};
	}

	if (!this->logger)
	{
		this->logger = core::Logger::get_instance({});
	}
}

__SERVER_END__
