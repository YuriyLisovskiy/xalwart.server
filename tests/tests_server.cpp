/**
 * tests_server.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

//#include <iostream>
//
//#include <gtest/gtest.h>
//
//#include "../src/server.h"
//
//using namespace std;
//
//
//class TestServer
//{
//public:
//	int sizeReceived = 0;
//
//	TestServer()
//	{
//		xw::server::HTTPServer server({}, [](
//			const int socket, xw::server::internal::request_parser* parser, xw::core::Error* err
//		) {
//			if (err)
//			{
//				std::cout << err << endl;
//			}
//			else
//			{
//				std::cout << parser->method << ": " << parser->path << endl;
//			}
//		});
//
//		if (server.bind(3000, false))
//		{
//			server.listen("Server is started...");
//		}
//
//		server.close();
//	}
//};
//
//TEST(ServerTestCase, server)
//{
//	TestServer();
//}
