/**
 * handlers/http_handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./http_handler.h"

// Base libraries.
#include <xalwart.base/string_utils.h>


__SERVER_BEGIN__

bool HTTPRequestHandler::parse_request()
{
	this->parsed = BaseHTTPRequestHandler::parse_request();
	if (!this->parsed)
	{
		return false;
	}

	auto full_p = str::split(this->full_path, '?', 1);
	this->request_ctx.path = full_p[0];
	if (full_p.size() == 2)
	{
		this->request_ctx.query = full_p[1];
	}

	auto content_len = this->request_ctx.headers.contains("Content-Length") ?
		this->request_ctx.headers.at("Content-Length") : "";
	if (!content_len.empty())
	{
		const char* s_content_len = content_len.c_str();
		this->request_ctx.content_size = std::stoi(s_content_len, nullptr, 10);
		if (!s_content_len)
		{
			this->send_error(400, "Bad request Content-Length header value (" + content_len + ")");
			return false;
		}

		if (this->total_bytes_read_count + this->request_ctx.content_size > this->max_request_size)
		{
			this->send_error(413);
			return false;
		}

		auto transfer_enc = this->request_ctx.headers.contains("Transfer-Encoding") ?
			this->request_ctx.headers.at("Transfer-Encoding") : "";
		if (!transfer_enc.empty() && str::lower(transfer_enc).find("chunked") != std::string::npos)
		{
			if (this->protocol_version < "HTTP/1.1")
			{
				this->send_error(
					501, "Chunked Transfer-Encoding is not supported by " + this->protocol_version + " protocol"
				);
				this->close_connection = true;
				return false;
			}
			else if (this->request_version < "HTTP/1.1")
			{
				this->send_error(400, "Chunked Transfer-Encoding is not supported by request");
				this->close_connection = true;
				return false;
			}
			else
			{
				this->request_ctx.chunked = true;

				// TODO: read and parse chunked request.
				this->send_error(400, "Chunked Transfer-Encoding is not supported by this server");
				this->close_connection = true;
				return false;
			}
		}
	}

	return true;
}

void HTTPRequestHandler::handle(net::HandlerFunc func)
{
	this->handler_func = std::move(func);
	this->close_connection = true;
	this->handle_one_request();
	this->close_io();
}

__SERVER_END__
