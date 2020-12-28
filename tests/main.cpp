/**
 * main.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include <gtest/gtest.h>

//#include <iostream>

//#include "../src/server.h"

//xw::string CONTENT = "HTTP/1.1 200 OK\r\n"
//                     "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
//                     "Server: Apache/2.2.14 (Win32)\r\n"
//                     "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
//                     "Content-Length: 54\r\n"
//                     "Content-Type: text/html\r\n"
//                     "Connection: Closed\r\n\r\n"
//					 "<html>\r\n"
//                     "<body>\r\n"
//                     "<h1>Hello, World!</h1>\r\n"
//                     "</body>\r\n"
//                     "</html>";
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
//class TestServer
//{
//public:
//	explicit TestServer(long port)
//	{
//		xw::server::Context ctx{};
//		ctx.threads_count = 30;
//		ctx.verbose = true;
//		ctx.normalize();
//		xw::server::HTTPServer server(ctx, [&](
//			const int socket, xw::server::internal::request_parser* parser, xw::core::Error* err
//		) {
//			xw::core::Error fail;
//			if (err)
//			{
//				ctx.logger->error(err ->msg);
//				if ((fail = server.send(socket, CONTENT_ERROR.c_str())))
//				{
//					ctx.logger->error(fail.msg);
//				}
//			}
//			else
//			{
//				if ((fail = server.send(socket, CONTENT.c_str())))
//				{
//					ctx.logger->error(fail.msg);
//				}
//
//				ctx.logger->info(parser->method + ": " + parser->path);
//			}
//		});
//
//		if (server.bind("127.0.0.1", port, false))
//		{
//			xw::core::InterruptException::initialize();
//			try
//			{
//				server.listen("Server is started...\n");
//			}
//			catch (const xw::core::InterruptException& exc)
//			{
//				// skip
//			}
//		}
//
//		server.close();
//	}
//};

int main(int argc, char *argv[])
{
//	TestServer(std::strtol(argv[1], nullptr, 10));

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
