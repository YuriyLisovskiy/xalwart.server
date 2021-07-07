/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <xalwart.core/string_utils.h>
#include <xalwart.server/http_server.h>

inline const std::string CONTENT = "HTTP/1.1 200 OK\r\n"
                                   "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                                   "Server: Apache/2.2.14 (Win32)\r\n"
                                   "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                                   "Content-Length: 48\r\n"
                                   "Content-Type: text/html\r\n"
                                   "Connection: Closed\r\n\r\n"
                                   "<html><body><h1>Hello, World!</h1></body></html>";

inline const char* SERVER_ADDRESS = "/tmp/xw.sock";

int main()
{
	xw::InterruptException::initialize();
	auto logCfg = xw::log::Config{};
	logCfg.add_console_stream();
	auto logger = xw::log::Logger(logCfg);
	logger.use_colors(true);
	auto server = xw::server::HTTPServer::initialize(&logger, {{
		{"workers", "10"},
		{"max_body_size", "1610611911"}, // 1.5 GB
		{"timeout_sec", "3"},
		{"timeout_usec", "0"},
	}});
	server->setup_handler([](
		xw::net::RequestContext* ctx,
		const xw::collections::Dict<std::string, std::string>& env
	) -> unsigned int
	{
		ctx->write(CONTENT.c_str(), CONTENT.size());
		return 200;
	});

	try
	{
		auto pair = xw::str::split(SERVER_ADDRESS, ':', -1);
		auto host = pair[0];
		uint16_t port = 0;
		if (pair.size() > 1)
		{
			port = std::stoi(pair[1]);
		}

		server->bind(host, port);
		try
		{
			auto msg = std::string(SERVER_ADDRESS);
			if (port)
			{
				msg = "http://" + msg;
			}

			server->listen("Server is started at " + msg);
		}
		catch (const xw::InterruptException &exc)
		{
			// skip
		}

		server->close();
	}
	catch (const xw::SocketError& exc)
	{
		logger.error(exc);
	}
	return 0;
}
