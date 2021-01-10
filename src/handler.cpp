/**
 * handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./handler.h"

// C++ libraries.
// TODO

// Core libraries.
#include <xalwart.core/net/status.h>

// Framework libraries.
// TODO


__SERVER_BEGIN__

HTTPRequestHandler::HTTPRequestHandler(
	int sock, const std::string& server_version, timeval timeout, core::ILogger* logger
) : logger(logger), server_version("SimpleHTTP/" + server_version), timeout(timeout), close_connection(false)
{
	this->socket_io = std::make_shared<SocketIO>(
		sock, timeout, std::make_shared<SelectSelector>(logger)
	);
}

void HTTPRequestHandler::handle_one_request()
{
	std::string raw_request_line;
	auto state = this->socket_io->read_line(raw_request_line, 65537);
	if (state != SocketIO::s_done)
	{
		switch (state)
		{
			case SocketIO::s_timed_out:
				this->logger->debug("Request timed out", _ERROR_DETAILS_);
				break;
			case SocketIO::s_conn_broken:
				this->logger->debug("Connection was broken", _ERROR_DETAILS_);
				break;
			case SocketIO::s_failed:
				this->logger->debug("Connection failed", _ERROR_DETAILS_);
				break;
			default:
				break;
		}

		this->close_connection = true;
		return;
	}

	if (raw_request_line.empty())
	{
		this->close_connection = true;
		return;
	}
	else if (raw_request_line.size() > 65536)
	{
		// TODO: send error: uri too long.
		return;
	}

	if (!this->parse_request())
	{
		// An error code has been sent, just exit.
		return;
	}

	this->handler_func(this->socket_io->fd(), , );
}

void HTTPRequestHandler::handle()
{
	this->close_connection = true;
	this->handle_one_request();
	while (!this->close_connection)
	{
		this->handle_one_request();
	}
}

void HTTPRequestHandler::send_error(
	int code, const std::string& message, const std::string& explain
)
{
	auto msg = net::HTTP_STATUS.get(code, std::pair<std::string, std::string>("???", "???"));
	if (!message.empty())
	{
		msg.first = message;
	}

	if (!explain.empty())
	{
		msg.second = explain;
	}

	// TODO: log error
	this->send_response(code, msg.first);
	this->send_header("Connection", "close");
	xw::string body;
	if (code >= 200 && code != 204 && code != 205 && code != 304)
	{
		// TODO: create content;
		xw::string content = "";
		body = content;
		this->send_header("Content-Type", this->error_content_type);
		this->send_header("Content-Length", std::to_string(body.size()));
	}

	this->end_headers();
	if (this->command != "HEAD" && !body.empty())
	{
		// TODO: write to socket.
	}
}

__SERVER_END__
