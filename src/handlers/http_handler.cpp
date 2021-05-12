/**
 * handlers/http_handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./http_handler.h"

// Core libraries.
#include <xalwart.core/string_utils.h>


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

	auto content_len = this->request_ctx.headers.get("Content-Length", "");
	if (!content_len.empty())
	{
		const char* s_content_len = content_len.c_str();
		this->request_ctx.content_size = std::stoi(s_content_len, nullptr, 10);
		if (!s_content_len)
		{
			this->send_error(
				400, "Bad request Content-Length header value (" + content_len + ")"
			);
			return false;
		}

		auto transfer_enc = this->request_ctx.headers.get("Transfer-Encoding", "");
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
			}
		}
		else
		{
			auto s_status = this->socket_io->read_bytes(
				this->request_ctx.content, this->request_ctx.content_size
			);
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
		}

		if (this->request_ctx.content_size != this->request_ctx.content.size())
		{
			this->send_error(400, "Bad request content");
			return false;
		}
	}

	return true;
}

std::string HTTPRequestHandler::server_version() const
{
	return "HTTPServer/" + this->server_num_version;
}

HTTPRequestHandler::HTTPRequestHandler(
	int sock, const std::string& server_version,
	timeval timeout, log::ILogger* logger,
	const collections::Dict<std::string, std::string>& env
) : BaseHTTPRequestHandler(sock, server_version, timeout, logger, env)
{
}

void HTTPRequestHandler::handle(net::HandlerFunc func)
{
	this->handler_func = std::move(func);
	this->close_connection = true;
	this->handle_one_request();
	if (this->socket_io->shutdown(SHUT_RDWR))
	{
		this->logger->error(
			"'shutdown(SHUT_WR)' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}
}

__SERVER_END__
