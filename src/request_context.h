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

// Module definitions.
#include "./_def_.h"

// Framework libraries.
// TODO


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
	std::map<std::string, std::string> headers;

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

	// Available content types.
	// Used only for POST, PUT or PATCH methods type.
	enum content_type_enum
	{
		ct_application_x_www_form_url_encoded,
		ct_application_json,
		ct_multipart_form_data,
		ct_other

	// Request's content type.
	} content_type{};
};

__SERVER_END__
