/**
 * request_context.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <map>

// Core libraries.
#include <xalwart.core/str.h>
#include <xalwart.core/collections/dict.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket/io.h"


__SERVER_BEGIN__

struct RequestContext
{
	// Major part of http protocol version.
	size_t major_v{};

	// Minor part of http protocol version.
	size_t minor_v{};

	// Request's path.
	std::string path;

	// Contains request's query.
	// If field is empty-string, request has not query.
	std::string query;

	// Hold http request's method type.
	std::string method;

	// Indicates whether request's connection is keep alive or not.
	bool keep_alive{};

	// Contains body of http request.
	xw::string content;

	// Accumulates request's headers.
	collections::Dict<std::string, std::string> headers;

	// Contains the size of request's content.
	unsigned long long content_size{};

	// Contains the size of request's chunk as std::string.
	// Used only for chunked requests.
	std::string chunk_size_str;

	// Contains the size of request's chunk.
	// Used only for chunked requests.
	unsigned long long chunk_size{};

	// Indicates whether request is chunked or not.
	bool chunked{};

	std::function<bool(const char* data, size_t n)> write;
};

__SERVER_END__
