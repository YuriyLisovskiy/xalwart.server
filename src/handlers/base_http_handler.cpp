/**
 * handlers/base_http_handler.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./base_http_handler.h"

// Base libraries.
#include <xalwart.base/net/status.h>
#include <xalwart.base/net/utility.h>
#include <xalwart.base/encoding.h>
#include <xalwart.base/string_utils.h>
#include <xalwart.base/html.h>
#include <xalwart.base/datetime.h>

// Server libraries.
#include "../exceptions.h"


__SERVER_BEGIN__

void BaseHTTPRequestHandler::handle()
{
	this->close_connection = true;
	this->handle_one_request();
	while (!this->close_connection)
	{
		this->handle_one_request();
	}

	this->close_io();
}

std::string BaseHTTPRequestHandler::default_error_message(
	unsigned int code, const std::string& phrase, const std::string& description
) const
{
	return "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n"
		"        \"http://www.w3.org/TR/html4/strict.dtd\">\n"
		"<html>\n"
		"    <head>\n"
		"        <meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8\">\n"
		"        <title>Error response</title>\n"
		"    </head>\n"
		"    <body>\n"
		"        <h1>Error response</h1>\n"
		"        <p>Error code: " + std::to_string(code) + "</p>\n"
		"        <p>Message: " + phrase + ".</p>\n"
		"        <p>Explanation: " + description + ".</p>\n"
		"    </body>\n"
		"</html>";
}

void BaseHTTPRequestHandler::log_request(uint code, const std::string& info) const
{
	log::Logger::Color text_color = log::Logger::Color::Green;
	if (code >= 400)
	{
		text_color = log::Logger::Color::Yellow;
	}
	else if (code >= 500)
	{
		text_color = log::Logger::Color::Red;
	}

	std::string message;
	if (this->request_is_parsed)
	{
		message = this->request_context.method + " " + this->full_path + " " + this->request_version;
	}
	else
	{
		message = info;
	}

	this->logger->print(
		"[" + dt::Datetime::now().strftime("%d/%b/%Y %T") + "] \"" + message + "\" " + std::to_string(code),
		text_color
	);
}

void BaseHTTPRequestHandler::cleanup_headers()
{
	// HTTP/1.1 requires support for persistent connections. Send 'close' if
	// the content length is unknown to prevent clients from reusing the
	// connection.
	if (!this->request_context.headers.contains("Content-Length"))
	{
		this->request_context.headers.insert(std::make_pair("Connection", "close"));
	}

	// Mark the connection for closing if it's set as such above or if the
	// application sent the header.
	if (
		this->request_context.headers.contains("Connection") &&
		str::to_lower(this->request_context.headers.at("Connection")) == "close"
	)
	{
		this->close_connection = true;
	}
}

bool BaseHTTPRequestHandler::parse_request()
{
	this->request_version = this->default_request_version;
	auto http_version = this->default_request_version;
	this->close_connection = true;
	auto request_line = str::rtrim(
		encoding::encode_iso_8859_1(this->raw_request_line, encoding::Mode::Strict), "\r\n"
	);
	std::string request_path;

	// command, path, version
	auto request_line_parts = str::split(request_line);
	if (request_line_parts.size() == 3)
	{
		this->request_context.method = request_line_parts[0];
		request_path = request_line_parts[1];
		http_version = request_line_parts[2];
		if (!http_version.starts_with("HTTP/"))
		{
			this->send_error(400, "Bad request version (" + http_version + ")");
			return false;
		}

		auto base_version_number = str::split(http_version, '/')[1];
		auto version_number = str::split(base_version_number, '.');

		// RFC 2145 section 3.1 says there can be only one "." and
		//   - major and minor numbers MUST be treated as
		//      separate integers;
		//   - HTTP/2.4 is a lower version than HTTP/2.13, which in
		//      turn is lower than HTTP/12.3;
		//   - Leading zeros MUST be ignored by recipients.
		if (version_number.size() != 2)
		{
			this->send_error(400, "Bad request version (" + http_version + ")");
			return false;
		}

		const char* major_version_string = version_number[0].c_str();
		const char* minor_version_string = version_number[1].c_str();

		this->request_context.protocol_version = {
			.major = std::stoul(major_version_string, nullptr, 10),
			.minor = std::stoul(minor_version_string, nullptr, 10)
		};
		if (!major_version_string || !minor_version_string)
		{
			this->send_error(400, "Bad request version (" + http_version + ")");
			return false;
		}

		if (
			this->request_context.protocol_version >= net::ProtocolVersion{1, 1} &&
			this->protocol_version >= "HTTP/1.1"
		)
		{
			this->close_connection = false;
		}

		if (this->request_context.protocol_version >= net::ProtocolVersion{2, 0})
		{
			// HTTP Version Not Supported.
			this->send_error(505, "Invalid HTTP version (" + base_version_number + ")");
			return false;
		}
	}
	else if (request_line_parts.size() == 2)
	{
		this->request_context.method = request_line_parts[0];
		request_path = request_line_parts[1];
		this->close_connection = true;
		if (this->request_context.method != "GET")
		{
			this->send_error(400, "Bad HTTP/0.9 request type (" + this->request_context.method + ")");
			return false;
		}
	}
	else if (request_line_parts.empty())
	{
		return false;
	}
	else
	{
		this->send_error(400, "Bad request syntax (" + request_line + ")");
		return false;
	}

	this->full_path = request_path;
	this->request_version = http_version;

	// Examine the headers and look for a Connection directive.
	if (!this->parse_headers())
	{
		return false;
	}

	auto connection_type = this->request_context.headers.contains("Connection") ?
		str::to_lower(this->request_context.headers.at("Connection")) : "";
	if (connection_type == "close")
	{
		this->close_connection = true;
	}
	else if (connection_type == "keep-alive" && this->protocol_version >= "HTTP/1.1")
	{
		this->close_connection = false;
		this->request_context.keep_alive = true;
	}

	// Examine the headers and look for expect directive.
	auto expect = this->request_context.headers.contains("Expect") ?
		str::to_lower(this->request_context.headers.at("Expect")) : "";
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
	if (!this->read_line(this->raw_request_line))
	{
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

	this->total_bytes_read_count += this->raw_request_line.size();
	this->request_is_parsed = this->parse_request();
	if (!this->request_is_parsed)
	{
		// An error code has been sent, just exit.
		return;
	}

	this->cleanup_headers();
	this->request_context.write = [this](const char* data, size_t n) -> bool {
		return this->write(data, (ssize_t)n);
	};
	this->request_context.body = this->socket_stream;
	this->log_request(this->handler_function(&this->request_context, this->environment), "");
}

bool BaseHTTPRequestHandler::read_line(std::string& destination)
{
	try
	{
		this->socket_stream->read_line(destination);
	}
	catch (const IOError& exc)
	{
		this->logger->error(exc);
		return false;
	}

	return true;
}

bool BaseHTTPRequestHandler::write(const char* content, ssize_t count)
{
	try
	{
		this->socket_stream->write(content, count);
	}
	catch (const IOError& exc)
	{
		this->logger->error(exc);
		return false;
	}

	return true;
}

bool BaseHTTPRequestHandler::parse_headers()
{
	try
	{
		this->total_bytes_read_count += net::parse_headers(
			this->request_context.headers, this->socket_stream.get(), this->max_header_length, this->max_headers_count
		);
		return true;
	}
	catch (const LineTooLongError& exc)
	{
		// Request Header Fields Too Large.
		this->send_error(
			431, "Line too long",
			"The server is unwilling to process the request because its header fields are too large"
		);
	}
	catch (const TooMuchHeadersError& exc)
	{
		this->send_error(
			431, "Too many headers",
			"The server is unwilling to process the request because its header fields are too large"
		);
	}
	catch (const IOError& exc)
	{
		this->close_connection = true;
	}
	catch (const ParseError& exc)
	{
		this->logger->error(exc);
		this->close_connection = true;
	}

	return false;
}

void BaseHTTPRequestHandler::send_error(unsigned int code, const std::string& message, const std::string& explain)
{
	auto [status, _] = net::get_status_by_code(code);
	if (!message.empty())
	{
		status.phrase = message;
	}

	if (!explain.empty())
	{
		status.description = explain;
	}

	this->send_response(code, status.phrase);
	this->send_header("Connection", "close");
	std::string body;
	if (code >= 200 && code != 204 && code != 205 && code != 304)
	{
		// Encode HTML to prevent Cross Site Scripting attacks.
		std::string content = this->default_error_message(
			code,
			html::escape(status.phrase, false),
			html::escape(status.description, false)
		);
		body = encoding::encode_utf_8(content, encoding::Mode::Replace);
		this->send_header("Content-Type", this->error_content_type);
		this->send_header("Content-Length", std::to_string(body.size()));
	}

	this->end_headers();
	if (this->command != "HEAD" && !body.empty())
	{
		if (this->write(body.c_str(), (ssize_t)body.size()))
		{
			this->log_request(code, message);
		}
	}
}

void BaseHTTPRequestHandler::send_response(unsigned int code, const std::string& message)
{
	this->send_response_only(code, message);
	this->send_header("Server", this->version_string());
	this->send_header("Date", this->datetime_string());
}

void BaseHTTPRequestHandler::send_response_only(unsigned int code, std::string message)
{
	if (this->request_version != "HTTP/0.9")
	{
		auto [status, is_found] = net::get_status_by_code(code);
		if (message.empty() && is_found)
		{
			message = status.phrase;
		}

		this->headers_buffer += encoding::encode_iso_8859_1(
			this->protocol_version + " " + std::to_string(code) + " " + message + "\r\n", encoding::Mode::Strict
		);
	}
}

void BaseHTTPRequestHandler::send_header(const std::string& keyword, const std::string& value)
{
	if (this->request_version != "HTTP/0.9")
	{
		this->headers_buffer += encoding::encode_iso_8859_1(
			keyword + ": " + value + "\r\n", encoding::Mode::Strict
		);
	}

	if (str::to_lower(keyword) == "connection")
	{
		auto value_lower = str::to_lower(value);
		if (value_lower == "close")
		{
			this->close_connection = true;
		}
		else if (value_lower == "keep-alive")
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
