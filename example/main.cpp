/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <xalwart.base/string_utils.h>
#include <xalwart.server/http_server.h>

inline static const std::string CONTENT = "HTTP/1.1 200 OK\r\n"
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

inline static const char* SERVER_ADDRESS = "127.0.0.1:1708";

inline static const size_t MAX_CONTENT_LENGTH = 2048;

inline static auto LOGGER = xw::log::Logger({});

inline static auto SERVER_CONFIG = xw::server::Context{
	.logger = &LOGGER,
	.max_headers_count = 100,
	.max_header_length = 65535,
	.workers_count = 1,
	.timeout_seconds = 3,
	.timeout_microseconds = 0,
	.socket_creation_retries_count = 5
};

xw::net::StatusCode handler(
	xw::net::RequestContext* context, const std::map<std::string, std::string>& environment
)
{
	if (context->content_size > 0)
	{
		std::string content, buffer;
		auto remaining_bytes = (ssize_t)context->content_size;
		for (size_t i = 0; remaining_bytes > 0; i++)
		{
			remaining_bytes -= context->body->read(buffer, remaining_bytes);
			content += buffer;
		}

		if (context->content_size <= MAX_CONTENT_LENGTH)
		{
			LOGGER.info("\n" + content);
		}
		else
		{
			LOGGER.warning("Content length is too large to print: " + std::to_string(context->content_size));
		}
	}

	context->write(CONTENT.c_str(), CONTENT.size());
	return 200;
}

void configure()
{
	auto logger_config = xw::log::Config{};
	logger_config.add_console_stream();
	LOGGER.set_config(logger_config);
	LOGGER.use_colors(true);

	SERVER_CONFIG.handler = handler;
}

void run_server()
{
	auto server = xw::server::HTTPServer(SERVER_CONFIG);
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
		LOGGER.warning(exc);
	}
	catch (const xw::BaseException& exc)
	{
		LOGGER.error(exc);
	}

	server.close();
}

int main()
{
	xw::InterruptException::initialize();
	try
	{
		configure();
		run_server();
	}
	catch (const xw::BaseException& exc)
	{
		LOGGER.error(exc);
	}

	std::this_thread::sleep_for(std::chrono::microseconds(1));
	return 0;
}
