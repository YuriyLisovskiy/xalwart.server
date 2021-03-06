/**
 * handlers/http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
// TODO

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base_http_handler.h"


__SERVER_BEGIN__

class HTTPRequestHandler : public BaseHTTPRequestHandler
{
protected:
	bool parse_request() override;

	[[nodiscard]]
	std::string server_version() const override;

public:
	explicit HTTPRequestHandler(
		int sock, const std::string& server_version,
		timeval timeout, log::ILogger* logger,
		const collections::Dict<std::string, std::string>& env
	);

	void handle(net::HandlerFunc func) override;
};

__SERVER_END__
