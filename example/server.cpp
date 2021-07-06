/**
 * server.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./server.h"

HelloWorldServer::HelloWorldServer(long port)
{
	auto logCfg = xw::log::Config{};
	logCfg.add_console_stream();
	auto logger = xw::log::Logger(logCfg);
	logger.use_colors(true);
	auto server = xw::server::HTTPServer::initialize(&logger, {{{"workers", "10"}}});
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
		server->bind("127.0.0.1", port);
		xw::InterruptException::initialize();
		try
		{
			server->listen("Server is started...\n");
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
