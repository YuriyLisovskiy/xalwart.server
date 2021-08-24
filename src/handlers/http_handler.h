/**
 * handlers/http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 * Based on Python 3.6.9 HTTP server.
 *
 * Simple handler implementation.
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base_http_handler.h"


__SERVER_BEGIN__

// TODO: docs for 'HTTPRequestHandler'
class HTTPRequestHandler : public BaseHTTPRequestHandler
{
protected:
	bool parse_request() override;

	[[nodiscard]]
	inline std::string server_version() const override
	{
		return "HTTPServer/" + this->server_num_version;
	}

public:
	inline explicit HTTPRequestHandler(
		io::IReader* socket_reader, io::IWriter* socket_writer,
		const std::string& server_version,
		size_t max_request_size, log::ILogger* logger,
		const std::map<std::string, std::string>& env
	) : BaseHTTPRequestHandler(
		socket_reader, socket_writer, max_request_size, server_version, logger, env
	)
	{
	}

	void handle(net::HandlerFunc func) override;
};

__SERVER_END__
