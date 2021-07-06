/**
 * server.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./server.h"
#include <xalwart.core/string_utils.h>

HelloWorldServer::HelloWorldServer(const char* address)
{
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
		auto pair = xw::str::split(address, ':', -1);
		auto host = pair[0];
		uint16_t port = 0;
		if (pair.size() > 1)
		{
			port = std::stoi(pair[1]);
		}

		server->bind(host, port);
		try
		{
			auto msg = std::string(address);
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
}
