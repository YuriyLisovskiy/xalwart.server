/**
 * _def_.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Purpose: main module's definitions.
 */

#pragma once

#include <chrono>

#include <xalwart.core/_def_.h>

// xw::server
#define __SERVER_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace server {
#define __SERVER_END__ } __MAIN_NAMESPACE_END__

// xw::server::util
#define __SERVER_UTIL_BEGIN__ __SERVER_BEGIN__ namespace util {
#define __SERVER_UTIL_END__ } __SERVER_END__

// xw::server::parser
#define __SERVER_PARSER_BEGIN__ __SERVER_BEGIN__ namespace parser {
#define __SERVER_PARSER_END__ } __SERVER_END__

__SERVER_BEGIN__

namespace v
{
const uint major = 0;
const uint minor = 0;
const uint patch = 0;

const std::string version = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
};

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
