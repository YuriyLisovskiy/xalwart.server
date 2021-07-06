/**
 * handlers/http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 * Based on Python 3 HTTP server.
 *
 * Simple handler implementation.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base_http_handler.h"


__SERVER_BEGIN__

class HTTPRequestHandler : public BaseHTTPRequestHandler
{
protected:
	size_t max_body_size;

protected:
	bool parse_request() override;

	[[nodiscard]]
	std::string server_version() const override
	{
		return "HTTPServer/" + this->server_num_version;
	}

public:
	explicit inline HTTPRequestHandler(
		int sock, const std::string& server_version,
		timeval timeout, size_t max_body_size, log::ILogger* logger,
		const collections::Dict<std::string, std::string>& env
	) : BaseHTTPRequestHandler(sock, server_version, timeout, logger, env), max_body_size(max_body_size)
	{
	}

	void handle(net::HandlerFunc func) override;
};

__SERVER_END__
