/**
 * handlers/http_handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./http_handler.h"

// C++ libraries.
// TODO

// Core libraries.
#include <xalwart.core/string_utils.h>


__SERVER_BEGIN__

HTTPRequestHandler::HTTPRequestHandler(
	int sock, const std::string& server_version,
	timeval timeout, core::ILogger* logger,
	const collections::Dict<std::string, std::string>& env
) : BaseHTTPRequestHandler(sock, server_version, timeout, logger, env)
{
}

bool HTTPRequestHandler::parse_request()
{
	this->parsed = BaseHTTPRequestHandler::parse_request();
	if (!this->parsed)
	{
		return false;
	}

	auto full_p = str::lsplit_one(this->full_path, '?');
	this->r_ctx.path = full_p.first;
	this->r_ctx.query = full_p.second;

	auto content_len = this->r_ctx.headers.get("Content-Length", "");
	if (!content_len.empty())
	{
		const char* s_content_len = content_len.c_str();
		this->r_ctx.content_size = std::stoi(s_content_len, nullptr, 10);
		if (!s_content_len)
		{
			this->send_error(
				400, "Bad request Content-Length header value (" + content_len + ")"
			);
			return false;
		}

		auto s_status = this->socket_io->read_all(this->r_ctx.content);
		if (s_status != SocketIO::s_done)
		{
			switch (s_status)
			{
				case SocketIO::s_timed_out:
				case SocketIO::s_conn_broken:
				case SocketIO::s_failed:
					this->log_socket_error(s_status);
					this->close_connection = true;
				default:
					break;
			}

			return false;
		}

		if (this->r_ctx.content_size != this->r_ctx.content.size())
		{
			this->send_error(400, "Bad request content");
			return false;
		}
	}

	// TODO: parse chunks if request is chunked!

	return true;
}

std::string HTTPRequestHandler::server_version() const
{
	return "HTTPServer/" + this->server_num_version;
}

__SERVER_END__
