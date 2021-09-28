/**
 * handlers/http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 * Based on Python 3.6.9 HTTP server.
 *
 * Simple handler implementation.
 */

#pragma once

// C++ libraries.
#include <memory>
#include <string>
#include <map>

// Base libraries.
#include <xalwart.base/io.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base_http_handler.h"


__SERVER_BEGIN__

// TODO: docs for 'HTTPRequestHandler'
class HTTPRequestHandler : public BaseHTTPRequestHandler
{
public:
	inline explicit HTTPRequestHandler(
		std::unique_ptr<io::ILimitedBufferedStream> stream,
		const std::string& server_version,
		size_t max_header_length, size_t max_headers_count,
		xw::abc::ILogger* logger, const std::map<std::string, std::string>& environment,
		HandlerFunction handler_function
	) : BaseHTTPRequestHandler(
			std::move(stream), max_header_length, max_headers_count, server_version,
			logger, environment, std::move(handler_function)
		)
	{
		require_non_null(this->stream.get(), "'socket_stream' is nullptr", _ERROR_DETAILS_);
	}

	inline void handle() override
	{
		this->close_connection = true;
		this->handle_one_request();
		this->close_io();
	}

protected:
	bool parse_request() override;

	[[nodiscard]]
	inline std::string server_version() const override
	{
		return "HTTPServer/" + this->server_version_number;
	}
};

__SERVER_END__
