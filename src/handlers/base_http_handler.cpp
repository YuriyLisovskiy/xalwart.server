/**
 * handlers/base_http_handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./base_http_handler.h"

// Base libraries.
#include <xalwart.base/net/status.h>
#include <xalwart.base/encoding.h>
#include <xalwart.base/string_utils.h>
#include <xalwart.base/html.h>


__SERVER_BEGIN__

std::string BaseHTTPRequestHandler::default_error_message(
	int code, const std::string& message, const std::string& explain
) const
{
	auto str_code = std::to_string(code);
	return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"
		"        \"http://www.w3.org/TR/html4/strict.dtd\">\n"
		"<html>\n"
		"    <head>\n"
		"        <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n"
		"        <title>Error response</title>\n"
		"    </head>\n"
		"    <body>\n"
		"        <h1>Error response</h1>\n"
		"        <p>Error code: " + str_code + "</p>\n"
		"        <p>Message: " + message + ".</p>\n"
		"        <p>Error code explanation: " + str_code + " - " + explain + ".</p>\n"
		"    </body>\n"
		"</html>";
}

void BaseHTTPRequestHandler::log_socket_error(SocketIO::State state) const
{
	switch (state)
	{
		case SocketIO::State::TimedOut:
			this->logger()->debug("Request timed out", _ERROR_DETAILS_);
			break;
		case SocketIO::State::ConnectionBroken:
			this->logger()->debug("Connection was broken", _ERROR_DETAILS_);
			break;
		case SocketIO::State::Failed:
			this->logger()->debug("Connection failed", _ERROR_DETAILS_);
			break;
		default:
			break;
	}
}

void BaseHTTPRequestHandler::log_parse_headers_error(parser::ParseHeadersStatus status) const
{
	switch (status)
	{
		case parser::ParseHeadersStatus::TimedOut:
			this->logger()->debug("Request timed out", _ERROR_DETAILS_);
			break;
		case parser::ParseHeadersStatus::ConnectionBroken:
			this->logger()->debug("Connection was broken", _ERROR_DETAILS_);
			break;
		case parser::ParseHeadersStatus::Failed:
			this->logger()->debug("Connection failed", _ERROR_DETAILS_);
			break;
		default:
			break;
	}
}

void BaseHTTPRequestHandler::log_request(uint code, const std::string& info) const
{
	log::Logger::Color color = log::Logger::Color::GREEN;
	if (code >= 400)
	{
		color = log::Logger::Color::YELLOW;
	}
	else if (code >= 500)
	{
		color = log::Logger::Color::RED;
	}

	std::string msg;
	if (this->parsed)
	{
		msg = this->request_ctx.method + " " + this->full_path + " " + this->request_version;
	}
	else
	{
		msg = info;
	}

	this->logger()->print(
		"[" + dt::Datetime::now().strftime("%d/%b/%Y %T") + "] \"" + msg + "\" " + std::to_string(code), color
	);
}

void BaseHTTPRequestHandler::cleanup_headers()
{
	// HTTP/1.1 requires support for persistent connections. Send 'close' if
	// the content length is unknown to prevent clients from reusing the
	// connection.
	if (!this->request_ctx.headers.contains("Content-Length"))
	{
		this->request_ctx.headers.set("Connection", "close");
	}

	// Mark the connection for closing if it's set as such above or if the
	// application sent the header.
	if (str::lower(this->request_ctx.headers.get("Connection")) == "close")
	{
		this->close_connection = true;
	}
}

bool BaseHTTPRequestHandler::parse_request()
{
	this->request_version = this->default_request_version;
	auto version = this->default_request_version;
	this->close_connection = true;
	auto req_line = str::rtrim(encoding::encode_iso_8859_1(this->raw_request_line, encoding::STRICT), "\r\n");
	this->request_line = req_line;

	std::string path;

	// command, path, version
	auto words = str::split(req_line);
	if (words.size() == 3)
	{
		this->request_ctx.method = words[0];
		path = words[1];
		version = words[2];
		if (!version.starts_with("HTTP/"))
		{
			this->send_error(400, "Bad request version (" + version + ")");
			return false;
		}

		auto base_version_number = str::split(version, '/')[1];
		auto version_number = str::split(base_version_number, '.');

		// RFC 2145 section 3.1 says there can be only one "." and
		//   - major and minor numbers MUST be treated as
		//      separate integers;
		//   - HTTP/2.4 is a lower version than HTTP/2.13, which in
		//      turn is lower than HTTP/12.3;
		//   - Leading zeros MUST be ignored by recipients.
		if (version_number.size() != 2)
		{
			this->send_error(400, "Bad request version (" + version + ")");
			return false;
		}

		const char* s_v_major = version_number[0].c_str();
		const char* s_v_minor = version_number[1].c_str();
		this->request_ctx.major_v = std::stoi(s_v_major, nullptr, 10);
		this->request_ctx.minor_v = std::stoi(s_v_minor, nullptr, 10);
		if (!s_v_major || !s_v_minor)
		{
			this->send_error(400, "Bad request version (" + version + ")");
			return false;
		}

		if (this->request_ctx.proto_v_gte(1, 1) && this->protocol_version >= "HTTP/1.1")
		{
			this->close_connection = false;
		}

		if (this->request_ctx.proto_v_gte(2, 0))
		{
			// HTTP Version Not Supported.
			this->send_error(505, "Invalid HTTP version (" + base_version_number + ")");
			return false;
		}
	}
	else if (words.size() == 2)
	{
		this->request_ctx.method = words[0];
		path = words[1];
		this->close_connection = true;
		if (this->request_ctx.method != "GET")
		{
			this->send_error(400, "Bad HTTP/0.9 request type (" + this->request_ctx.method + ")");
			return false;
		}
	}
	else if (words.empty())
	{
		return false;
	}
	else
	{
		this->send_error(400, "Bad request syntax (" + req_line + ")");
		return false;
	}

	this->full_path = path;
	this->request_version = version;

	// Examine the headers and look for a Connection directive.
	auto p_status = parser::parse_headers(this->request_ctx.headers, this->socket_io.get());
	if (p_status != parser::ParseHeadersStatus::Done)
	{
		switch (p_status)
		{
			case parser::ParseHeadersStatus::LineTooLong:
				// Request Header Fields Too Large.
				this->send_error(
					431, "Line too long",
					"The server is unwilling to process the request because its header fields are too large"
				);
				return false;
			case parser::ParseHeadersStatus::MaxHeadersReached:
				this->send_error(
					431, "Too many headers",
					"The server is unwilling to process the request because its header fields are too large"
				);
				return false;
			case parser::ParseHeadersStatus::TimedOut:
			case parser::ParseHeadersStatus::ConnectionBroken:
			case parser::ParseHeadersStatus::Failed:
				this->log_parse_headers_error(p_status);
				this->close_connection = true;
				return false;
			default:
				break;
		}
	}

	auto conn_type = str::lower(this->request_ctx.headers.get("Connection", ""));
	if (conn_type == "close")
	{
		this->close_connection = true;
	}
	else if (conn_type == "keep-alive" && this->protocol_version >= "HTTP/1.1")
	{
		this->close_connection = false;
		this->request_ctx.keep_alive = true;
	}

	// Examine the headers and look for expect directive.
	auto expect = str::lower(this->request_ctx.headers.get("Expect", ""));
	if (
		expect == "100-continue" && this->protocol_version >= "HTTP/1.1" && this->request_version >= "HTTP/1.1"
	)
	{
		if (!this->handle_expect_100())
		{
			return false;
		}
	}

	return true;
}

bool BaseHTTPRequestHandler::handle_expect_100()
{
	// Request received, please continue.
	int code = 100;
	this->send_response_only(code);
	this->end_headers();
	this->log_request(code, "");
	return true;
}

void BaseHTTPRequestHandler::handle_one_request()
{
	auto state = this->socket_io->read_line(this->raw_request_line, 65537);
	if (state != SocketIO::State::Done)
	{
		this->log_socket_error(state);
		this->close_connection = true;
		return;
	}

	if (this->raw_request_line.empty())
	{
		this->close_connection = true;
		return;
	}
	else if (this->raw_request_line.size() > 65536)
	{
		// Request-URI Too Long.
		this->send_error(414);
		return;
	}

	this->parsed = this->parse_request();
	if (!this->parsed)
	{
		// An error code has been sent, just exit.
		return;
	}

	this->cleanup_headers();
	this->request_ctx.write = [this](const char* data, size_t n) -> bool {
		auto status = this->socket_io->write(data, n);
		bool success = status == SocketIO::State::Done;
		if (!success)
		{
			this->log_socket_error(status);
		}

		return success;
	};
	this->log_request(this->handler_func(&this->request_ctx, this->env), "");
}

void BaseHTTPRequestHandler::handle(net::HandlerFunc func)
{
	this->handler_func = std::move(func);
	this->close_connection = true;
	this->handle_one_request();
	while (!this->close_connection)
	{
		this->handle_one_request();
	}

	if (this->socket_io->shutdown(SHUT_RDWR))
	{
		this->logger()->error("'shutdown(SHUT_RDWR)' call failed: " + std::to_string(errno), _ERROR_DETAILS_);
	}
}

void BaseHTTPRequestHandler::send_error(int code, const std::string& message, const std::string& explain)
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

	this->send_response(code, msg.first);
	this->send_header("Connection", "close");
	std::string body;
	if (code >= 200 && code != 204 && code != 205 && code != 304)
	{
		// HTML encode to prevent Cross Site Scripting attacks.
		std::string content = this->default_error_message(
			code, html::escape(msg.first, false), html::escape(msg.second, false)
		);
		body = encoding::encode_utf_8(content, encoding::REPLACE);
		this->send_header("Content-Type", this->error_content_type);
		this->send_header("Content-Length", std::to_string(body.size()));
	}

	this->end_headers();
	if (this->command != "HEAD" && !body.empty())
	{
		auto status = this->socket_io->write(body.c_str(), body.size());
		if (status != SocketIO::State::Done)
		{
			this->log_socket_error(status);
		}
	}

	this->log_request(code, message);
}

void BaseHTTPRequestHandler::send_response(int code, const std::string& message)
{
	this->send_response_only(code, message);
	this->send_header("Server", this->version_string());
	this->send_header("Date", this->datetime_string());
}

void BaseHTTPRequestHandler::send_response_only(int code, const std::string& message)
{
	auto msg = message;
	if (this->request_version != "HTTP/0.9")
	{
		if (msg.empty() && net::HTTP_STATUS.contains(code))
		{
			msg = net::HTTP_STATUS.get(code).first;
		}

		this->headers_buffer += encoding::encode_iso_8859_1(
			this->protocol_version + " " + std::to_string(code) + " " + msg + "\r\n", encoding::STRICT
		);
	}
}

void BaseHTTPRequestHandler::send_header(const std::string& keyword, const std::string& value)
{
	if (this->request_version != "HTTP/0.9")
	{
		this->headers_buffer += encoding::encode_iso_8859_1(keyword + ": " + value + "\r\n", encoding::STRICT);
	}

	if (str::lower(keyword) == "connection")
	{
		auto val_lower = str::lower(value);
		if (val_lower == "close")
		{
			this->close_connection = true;
		}
		else if (val_lower == "keep-alive")
		{
			this->close_connection = false;
		}
	}
}

void BaseHTTPRequestHandler::end_headers()
{
	if (this->request_version != "HTTP/0.9")
	{
		this->headers_buffer += "\r\n";
		this->flush_headers();
	}
}

__SERVER_END__
