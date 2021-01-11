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
#include <xalwart.core/utility.h>
#include <xalwart.core/encoding.h>
#include <xalwart.core/string_utils.h>


__SERVER_BEGIN__

std::string HTTPRequestHandler::default_error_message(
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

void HTTPRequestHandler::log_socket_error(SocketIO::state st) const
{
	switch (st)
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
}

void HTTPRequestHandler::log_request(int code, const std::string& info) const
{
	core::Logger::Color color = core::Logger::Color::GREEN;
	if (code >= 400)
	{
		color = core::Logger::Color::YELLOW;
	}
	else if (code >= 500)
	{
		color = core::Logger::Color::RED;
	}

	std::string msg;
	if (this->parsed)
	{
		msg = this->r_ctx.method + " " + this->full_path + " " + this->request_version;
	}
	else
	{
		msg = info;
	}

	this->logger->print(
		"[" + dt::Datetime::now().strftime("%d/%b/%Y %T") + "] \"" +
		msg + "\" " + std::to_string(code),
		color
	);
}

HTTPRequestHandler::HTTPRequestHandler(
	int sock, const std::string& server_version, timeval timeout, core::ILogger* logger
) : logger(logger),
	server_version("SimpleHTTP/" + server_version),
	timeout(timeout),
	close_connection(false),
	parsed(false)
{
	this->socket_io = std::make_shared<SocketIO>(
		sock, timeout, std::make_shared<SelectSelector>(logger)
	);
}

bool HTTPRequestHandler::parse_request()
{
	this->request_version = this->default_request_version;
	auto version = this->default_request_version;
	this->close_connection = true;
	auto req_line = encoding::encode_iso_8859_1(this->raw_request_line, encoding::STRICT);
	str::rtrim(req_line, "\r\n");
	this->request_line = req_line;

	std::string path;

	// command, path, version
	auto words = str::split(req_line);
	if (words.size() == 3)
	{
		this->r_ctx.method = words[0];
		path = words[1];
		version = words[2];
		if (!str::starts_with(version, "HTTP/"))
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
		this->r_ctx.major_v = std::stoi(s_v_major, nullptr, 10);
		this->r_ctx.minor_v = std::stoi(s_v_minor, nullptr, 10);
		if (!s_v_major || !s_v_minor)
		{
			this->send_error(400, "Bad request version (" + version + ")");
			return false;
		}

		if (this->r_ctx.major_v >= 1 && this->r_ctx.minor_v >= 1 && this->protocol_version >= "HTTP/1.1")
		{
			this->close_connection = false;
		}

		if (this->r_ctx.major_v >= 2 && this->r_ctx.minor_v >= 0)
		{
			// HTTP Version Not Supported.
			this->send_error(505, "Invalid HTTP version (" + base_version_number + ")");
			return false;
		}
	}
	else if (words.size() == 2)
	{
		this->r_ctx.method = words[0];
		path = words[1];
		this->close_connection = true;
		if (this->r_ctx.method != "GET")
		{
			this->send_error(400, "Bad HTTP/0.9 request type (" + this->r_ctx.method + ")");
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
	// TODO: parse headers

	auto conn_type = str::lower(this->r_ctx.headers.get("Connection", ""));
	if (conn_type == "close")
	{
		this->close_connection = true;
	}
	else if (conn_type == "keep-alive" && this->protocol_version >= "HTTP/1.1")
	{
		this->close_connection = false;
	}

	// Examine the headers and look for an Expect directive.
	auto expect = str::lower(this->r_ctx.headers.get("Expect", ""));
	if (
		expect == "100-continue" &&
		this->protocol_version >= "HTTP/1.1" &&
		this->request_version >= "HTTP/1.1"
	)
	{
		if (!this->handle_expect_100())
		{
			return false;
		}
	}

	return true;
}

bool HTTPRequestHandler::handle_expect_100()
{
	// Request received, please continue.
	this->send_response_only(100);
	this->end_headers();
	return true;
}

void HTTPRequestHandler::handle_one_request()
{
	auto state = this->socket_io->read_line(this->raw_request_line, 65537);
	if (state != SocketIO::s_done)
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

	this->handler_func(this->socket_io->fd(), &this->r_ctx);
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

	this->send_response(code, msg.first);
	this->send_header("Connection", "close");
	xw::string body;
	if (code >= 200 && code != 204 && code != 205 && code != 304)
	{
		// HTML encode to prevent Cross Site Scripting attacks.
		xw::string content = this->default_error_message(
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
		if (status != SocketIO::s_done)
		{
			this->log_socket_error(status);
		}
	}
}

void HTTPRequestHandler::send_response(int code, const std::string& message)
{
	this->log_request(code, message);
	this->send_response_only(code, message);
	this->send_header("Server", this->version_string());
	this->send_header("Date", this->datetime_string());
}

void HTTPRequestHandler::send_response_only(int code, const std::string& message)
{
	auto msg = message;
	if (this->request_version != "HTTP/0.9")
	{
		if (msg.empty() && net::HTTP_STATUS.contains(code))
		{
			msg = net::HTTP_STATUS.get(code).first;
		}

		this->headers_buffer += encoding::encode_iso_8859_1(
			this->protocol_version + " " + std::to_string(code) + " " + msg + "\r\n",
			encoding::STRICT
		);
	}
}

void HTTPRequestHandler::send_header(const std::string& keyword, const std::string& value)
{
	if (this->request_version != "HTTP/0.9")
	{
		this->headers_buffer += encoding::encode_iso_8859_1(
			keyword + ": " + value + "\r\n", encoding::STRICT
		);
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

void HTTPRequestHandler::end_headers()
{
	if (this->request_version != "HTTP/0.9")
	{
		this->headers_buffer += "\r\n";
		this->flush_headers();
	}
}

void HTTPRequestHandler::flush_headers()
{
	this->socket_io->write(this->headers_buffer.c_str(), this->headers_buffer.size());
	this->headers_buffer = "";
}

std::string HTTPRequestHandler::version_string() const
{
	return this->server_version + " " + this->sys_version;
}

std::string HTTPRequestHandler::datetime_string() const
{
	return utility::format_date(dt::Datetime::utc_now().timestamp(), false, true);
}

__SERVER_END__
