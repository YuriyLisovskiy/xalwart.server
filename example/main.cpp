/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <xalwart.base/string_utils.h>
#include <xalwart.server/http_server.h>

inline const std::string CONTENT = "HTTP/1.1 200 OK\r\n"
                                   "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                                   "Server: Apache/2.2.14 (Win32)\r\n"
                                   "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
								   "Content-Length: 185\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Connection: Closed\r\n\r\n"
								   "<html><body>"
								   "<h1>Hello, World!</h1>"
								   "<form method=\"post\" enctype=\"multipart/form-data\">"
								   "<input name=\"number\" type=\"file\" placeholder=\"Number...\"/>"
								   "<input type=\"submit\"/>"
								   "</form>"
								   "</body></html>";

//inline const char* SERVER_ADDRESS = "/tmp/xw.sock";
inline const char* SERVER_ADDRESS = "127.0.0.1:1708";

int main()
{
	xw::InterruptException::initialize();
	auto logCfg = xw::log::Config{};
	logCfg.add_console_stream();
	auto logger = xw::log::Logger(logCfg);
	logger.use_colors(true);
	auto server = xw::server::HTTPServer(xw::server::Context(
		&logger, {{
			{xw::server::p::WORKERS_COUNT, "1"},
			{xw::server::p::MAX_REQUEST_SIZE, "1610611911"}, // 1.5 GB
			{xw::server::p::TIMEOUT_SECONDS, "3"},
			{xw::server::p::TIMEOUT_MICROSECONDS, "0"},
		}}
	), nullptr);
	server.setup_handler([](
		xw::net::RequestContext* ctx,
		const xw::collections::Dictionary<std::string, std::string>& env
	) -> uint
	{
		if (ctx->content_size > 0)
		{
			std::string buffer;

			auto part_1 = ctx->content_size - 50;
			ctx->body->read(buffer, part_1);
			std::cerr << "BUFFER 1:\n" << buffer << '\n';

			auto part_2 = 50;
			ctx->body->read(buffer, part_2);
			std::cerr << "BUFFER 2:\n" << buffer << '\n';
		}

		ctx->write(CONTENT.c_str(), CONTENT.size());
		return 200;
	});

	auto pair = xw::str::split(SERVER_ADDRESS, ':', -1);
	auto host = pair[0];
	uint16_t port = 0;
	if (pair.size() > 1)
	{
		port = std::stoi(pair[1]);
	}

	server.bind(host, port);
	try
	{
		auto msg = std::string(SERVER_ADDRESS);
		if (port)
		{
			msg = "http://" + msg;
		}

		server.listen("Server is started at " + msg);
	}
	catch (const xw::InterruptException& exc)
	{
		logger.warning(exc);
	}

	server.close();
	return 0;
}
