/**
 * main.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

#include <iostream>

#include "../src/server.h"

xw::string CONTENT = "HTTP/1.1 200 OK\n"
                     "Date: Mon, 27 Jul 2009 12:28:53 GMT\n"
                     "Server: Apache/2.2.14 (Win32)\n"
                     "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n"
                     "Content-Length: 88\n"
                     "Content-Type: text/html\n"
                     "Connection: Closed\n"
					 "<html>\n"
                     "<body>\n"
                     "<h1>Hello, World!</h1>\n"
                     "</body>\n"
                     "</html>\n";

class TestServer
{
public:
	TestServer()
	{
		xw::server::Context ctx{};
		ctx.threads_count = 10;
		xw::server::HTTPServer server(ctx, [&](
			const int socket, xw::server::internal::request_parser* parser, xw::core::Error* err
		) {
			if (err)
			{
				std::cout << err << std::endl;
			}
			else
			{
				std::cout << parser->method << ": " << parser->path << std::endl;
			}

			xw::core::Error fail;
			if ((fail = server.send(socket, CONTENT.c_str())))
			{
				std::cout << fail << std::endl;
			}
		});

		if (server.bind("127.0.0.1", 3000, false))
		{
			server.listen("Server is started...\n");
		}

		server.close();
	}
};

int main(int argc, char *argv[])
{
//	TestServer();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
