/**
 * _def_.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: main module's definitions.
 */

#pragma once

#include <chrono>

#include <xalwart.core/_def_.h>


// xw::server
#define __SERVER_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace server {
#define __SERVER_END__ } __MAIN_NAMESPACE_END__

// xw::server::internal
#define __SERVER_INTERNAL_BEGIN__ __SERVER_BEGIN__ namespace internal {
#define __SERVER_INTERNAL_END__ } __SERVER_END__


__SERVER_BEGIN__

typedef std::function<void(int, const std::string&)> OnErrorFunc;

__SERVER_END__

template <typename TimeT = std::chrono::milliseconds>
class Measure
{
private:
	std::chrono::high_resolution_clock::time_point _begin;
	std::chrono::high_resolution_clock::time_point _end;

public:
	void start()
	{
		this->_begin = std::chrono::high_resolution_clock::now();
	}

	void end()
	{
		this->_end = std::chrono::high_resolution_clock::now();
	}

	double elapsed(bool reset = true)
	{
		auto result = std::chrono::duration_cast<TimeT>(this->_end - this->_begin).count();
		if (reset)
		{
			this->reset();
		}

		return result;
	}

	void reset()
	{
		this->_begin = {};
		this->_end = {};
	}
};
