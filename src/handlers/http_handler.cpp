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
	this->request_is_parsed = BaseHTTPRequestHandler::parse_request();
	if (!this->request_is_parsed)
	{
		return false;
	}

	auto full_path_parts = str::split(this->full_path, '?', 1);
	this->request_context.path = full_path_parts[0];
	if (full_path_parts.size() == 2)
	{
		this->request_context.query = full_path_parts[1];
	}

	auto content_length = this->request_context.headers.contains("Content-Length") ?
		this->request_context.headers.at("Content-Length") : "";
	if (!content_length.empty())
	{
		const char* content_length_string = content_length.c_str();
		this->request_context.content_size = std::stoi(content_length_string, nullptr, 10);
		if (!content_length_string)
		{
			this->send_error(400, "Bad request Content-Length header value (" + content_length + ")");
			return false;
		}

		auto transfer_encoding = this->request_context.headers.contains("Transfer-Encoding") ?
			this->request_context.headers.at("Transfer-Encoding") : "";
		if (!transfer_encoding.empty() && str::to_lower(transfer_encoding).find("chunked") != std::string::npos)
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
				this->request_context.chunked = true;

				// TODO: read and parse chunked request.
				this->send_error(400, "Chunked Transfer-Encoding is not supported by this server");
				this->close_connection = true;
				return false;
			}
		}
	}

	return true;
}

__SERVER_END__
