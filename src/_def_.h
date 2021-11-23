/**
 * _def_.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Definitions of main module.
 */

#pragma once

// C++ libraries.
#include <chrono>

// Module definitions.
#include <xalwart.base/_def_.h>

// xw::server::util
#define __SERVER_UTIL_BEGIN__ __SERVER_BEGIN__ namespace util {
#define __SERVER_UTIL_END__ } __SERVER_END__


__SERVER_BEGIN__

namespace v
{
const inline auto version = Version("0.0.0");
};

using Socket = int;

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
