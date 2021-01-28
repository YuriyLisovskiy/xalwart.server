/**
 * main.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

//#include <iostream>

#include "../src/http_server.h"

std::string CONTENT = "HTTP/1.1 200 OK\r\n"
                     "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                     "Server: Apache/2.2.14 (Win32)\r\n"
                     "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                     "Content-Length: 48\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: Closed\r\n\r\n"
					 "<html><body><h1>Hello, World!</h1></body></html>";
//
//xw::string CONTENT_ERROR = "HTTP/1.1 500 Server Error\r\n"
//                     "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
//                     "Server: Apache/2.2.14 (Win32)\r\n"
//                     "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
//                     "Content-Length: 54\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Connection: Closed\r\n\r\n"
//                     "<html>\r\n"
//                     "<body>\r\n"
//                     "<h1>Hello, World!</h1>\r\n"
//                     "</body>\r\n"
//                     "</html>";
//
class TestServer
{
public:
	explicit TestServer(long port)
	{
		auto logger = xw::core::Logger::get_instance({}).get();
		auto server = xw::server::HTTPServer::initialize(logger, {});
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
			xw::core::InterruptException::initialize();
			try
			{
				server->listen("Server is started...\n");
			}
			catch (const xw::core::InterruptException &exc)
			{
				// skip
			}

			server->close();
		}
		catch (const xw::core::SocketError& exc)
		{
			logger->error(exc);
		}
	}
};

int main(int argc, char *argv[])
{
	TestServer(std::strtol(argv[1], nullptr, 10));

//	::testing::InitGoogleTest(&argc, argv);
//	return RUN_ALL_TESTS();
}
