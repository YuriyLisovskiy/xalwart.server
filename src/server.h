/**
 * server.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/collections/dict.h>

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./base.h"
#include "./func.h"
#include "../../http/request.h"
#include "../../http/response.h"
#include "../../conf/settings.h"


__SERVER_BEGIN__

class DefaultServer : public HTTPServer
{
public:
	explicit DefaultServer(
		const Context& ctx,
		HttpHandlerFunc handler,
		const conf::Settings* settings
	);

	void init_environ() override;

protected:
	const conf::Settings* settings;

private:
	HttpHandlerFunc _http_handler;

private:
	HandlerFunc _make_handler();

	static std::shared_ptr<http::IHttpResponse> _from_error(const core::Error* err);

	std::shared_ptr<http::HttpRequest> _request(parsers::request_parser* parser);

	core::Error _send(http::IHttpResponse* response, const int& client);
	core::Error _send(http::StreamingHttpResponse* response, const int& client);

	void _start_response(
		const int& client,
		const http::HttpRequest* request,
		const core::Result<std::shared_ptr<http::IHttpResponse>>& response
	);

	core::Error _send_response(
		const http::HttpRequest* request,
		http::IHttpResponse* response,
		const int& client,
		core::ILogger* logger
	);
	static void _log_request(
		const std::string& info, unsigned short status_code, core::ILogger* logger
	);
};

__SERVER_END__
