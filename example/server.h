/**
 * server.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#pragma once

#include <xalwart.server/http_server.h>

inline const std::string CONTENT = "HTTP/1.1 200 OK\r\n"
                      "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
                      "Server: Apache/2.2.14 (Win32)\r\n"
                      "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
                      "Content-Length: 48\r\n"
                      "Content-Type: text/html\r\n"
                      "Connection: Closed\r\n\r\n"
                      "<html><body><h1>Hello, World!</h1></body></html>";
class HelloWorldServer
{
public:
	explicit HelloWorldServer(const char* address);
};
