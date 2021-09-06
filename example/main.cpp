/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include <xalwart.base/string_utils.h>
#include <xalwart.server/http_server.h>

inline const std::string CONTENT = "HTTP/1.1 200 OK\r\n"
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

//inline const char* SERVER_ADDRESS = "/tmp/xw.sock";
inline const char* SERVER_ADDRESS = "127.0.0.1:1708";

void run_server(xw::log::ILogger* logger)
{
	xw::InterruptException::initialize();
	auto server = xw::server::HTTPServer({
		.logger = logger,
//		.max_request_size = 1610611911,
		.workers_count = 1,
		.timeout_seconds = 3,
		.timeout_microseconds = 0,
		.socket_creation_retries_count = 5,
		.handler = [](
			xw::net::RequestContext* ctx, const std::map<std::string, std::string>& env
		) -> xw::net::StatusCode
		{
			if (ctx->content_size > 0)
			{
				std::string buffer;
				auto remaining_bytes = (long long int)ctx->content_size;
				long long int result_bytes_count = 0;
				long long int current_bytes_read = 0;

				std::cerr << "CONTENT LENGTH: " << ctx->content_size;

				for (size_t i = 0; remaining_bytes > 0; i++)
				{
//					std::cerr << "BUFFER " << i + 1 << ":\n" << buffer << '\n';

//					auto read_size = std::min(content_size, chunk_length);
//					if (read_size < chunk_length)
//					{
//						read_size = (long long int)ctx->content_size - result_bytes_count;
//					}

					current_bytes_read = ctx->body->read(buffer, remaining_bytes);
					result_bytes_count += current_bytes_read;

//					step = std::min((long long int)buffer.size(), step);

//					std::cerr << "CONTENT LENGTH: " << ctx->content_size;
//					std::cerr << ", CURRENT BYTES READ: ";
//					std::cerr << current_bytes_read << ", TOTAL BYTES: " << result_bytes_count;
//					std::cerr << ", NEXT READ TARGET: " << remaining_bytes - current_bytes_read;
//					std::cerr << ", REMAINING BYTES: " << remaining_bytes << '\n';
//					std::cerr << "CONTENT:\n===============================\n" << buffer;
//					std::cerr << "\n===============================\n";
//					if (content_size < chunk_length)
//					{
//						break;
//					}

					remaining_bytes -= current_bytes_read;
				}

//				auto part_1 = ctx->content_size - 50;
//				ctx->body->read(buffer, part_1);
//				std::cerr << "BUFFER 1:\n" << buffer << '\n';
//
//				auto part_2 = 50;
//				ctx->body->read(buffer, part);
//				std::cerr << "BUFFER 2:\n" << buffer << '\n';
			}

			ctx->write(CONTENT.c_str(), CONTENT.size());
			return 200;
		}
	});

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
		logger->warning(exc);
	}
	catch (const xw::BaseException& exc)
	{
		logger->error(exc);
	}

	server.close();
}

int main()
{
	auto logCfg = xw::log::Config{};
	logCfg.add_console_stream();
	auto logger = xw::log::Logger(logCfg);
	logger.use_colors(true);

	try
	{
		run_server(&logger);
	}
	catch (const xw::BaseException& exc)
	{
		logger.error(exc);
	}

	std::this_thread::sleep_for(std::chrono::microseconds (1));
	return 0;
}
