/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <xalwart.base/string_utils.h>
#include <xalwart.base/workers/threaded_worker.h>
#include <xalwart.base/utility.h>
#include <xalwart.server/http_server.h>


std::string make_content()
{
	std::string body = "<html><body>"
	                   "<h1>Hello, World!</h1>"
	                   "<form method=\"post\" enctype=\"multipart/form-data\">"
	                   "<input name=\"my-file\" type=\"file\" placeholder=\"My file...\"/>"
	                   "<input type=\"submit\"/>"
	                   "</form>"
	                   "</body></html>";
	auto now_string = xw::util::format_date(
		(time_t)xw::dt::Datetime::utc_now().timestamp(), false, true
	);
	return "HTTP/1.1 200 OK\r\n"
		   "Date: " + now_string + "\r\n"
	       "Server: Apache/2.2.14 (Win32)\r\n"
	       "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
		   "Content-Length: " + std::to_string(body.size()) + "\r\n"
	       "Content-Type: text/html\r\n"
	       "Connection: Closed\r\n\r\n" + body;
}

inline static const char* SERVER_ADDRESS = "127.0.0.1:1708";

inline static const size_t MAX_CONTENT_LENGTH = 2048;

std::function<xw::net::StatusCode(
	xw::net::RequestContext* context, const std::map<std::string, std::string>& environment
)> handler(xw::log::Logger& logger)
{
	return [&logger](auto* context, const auto& environment) -> auto
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
				logger.info("\n" + content);
			}
			else
			{
				logger.warning(
					"Content length is too large to print: " + std::to_string(context->content_size)
				);
			}
		}

		auto content = make_content();
		context->response_writer->write(content.c_str(), content.size());
		return 200;
	};
}

void run_server(xw::log::Logger& logger)
{
	xw::server::Context server_config{
		.logger = &logger,
		.max_headers_count = 100,
		.max_header_length = 65535,
		.timeout_seconds = 3,
		.timeout_microseconds = 0,
		.socket_creation_retries_count = 5,
		.worker = std::make_unique<xw::ThreadedWorker>(5),
		.handler = handler(logger)
	};
	auto server = xw::server::DevelopmentHTTPServer(std::move(server_config));
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
	catch (const xw::BaseException& exc)
	{
		logger.error(exc);
	}

	server.close();
}

int main()
{
	xw::InterruptException::initialize();

	auto logger_config = xw::log::Config{};
	logger_config.add_console_stream();
	logger_config.disable_all_levels();

	auto has_debug = logger_config.is_enabled(xw::log::Level::Debug);

	auto logger = xw::log::Logger(logger_config);
	logger.enable_colors();
	try
	{
		run_server(logger);
	}
	catch (const xw::BaseException& exc)
	{
		logger.error(exc);
	}

	std::this_thread::sleep_for(std::chrono::microseconds(1));
	return 0;
}
