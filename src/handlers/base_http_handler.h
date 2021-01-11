/**
 * handlers/base_http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>

// Core libraries.
#include <xalwart.core/sys.h>
#include <xalwart.core/str.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../socket/io.h"
#include "../request_context.h"
#include "../parser.h"


__SERVER_BEGIN__

typedef std::function<uint(
	const int, RequestContext*, collections::Dict<std::string, std::string>
)> HandlerFunc;

class BaseHTTPRequestHandler
{
protected:
	HandlerFunc handler_func;

	RequestContext r_ctx;

	std::shared_ptr<SocketIO> socket_io;

	core::ILogger* logger;

	std::string sys_version = sys::compiler + "/" + sys::compiler_version;

	// The server software version.
	std::string server_version;

	// The default request version. This only affects responses up until
	// the point where the request line is parsed, so it mainly decides what
	// the client gets back when sending a malformed request line.
	// Most web servers default to HTTP 0.9, i.e. don't send a status line.
	std::string default_request_version = "HTTP/0.9";

	const std::string error_content_type = "text/html;charset=utf-8";

	const std::string protocol_version = "HTTP/1.1";

	bool close_connection;

	std::string raw_request_line;
	std::string request_line;
	std::string request_version;
	std::string command;
	std::string full_path;

	xw::string headers_buffer;

	bool parsed;

	collections::Dict<std::string, std::string> env;

protected:
	[[nodiscard]]
	virtual std::string default_error_message(
		int code, const std::string& message, const std::string& explain
	) const;

	void log_socket_error(SocketIO::state st) const;

	void log_parse_headers_error(parser::parse_headers_status st) const;

	void log_request(int code, const std::string& info="") const;

	// Parse a request.
	//
	// The request should be stored in this->request_line; the results
	// are in this->command, this->path, this->request_version and
	// this->headers.
	//
	// Return true for success, false for failure; on failure, an
	// error is sent back.
	virtual bool parse_request();

	// Decide what to do with an "Expect: 100-continue" header.
	//
	// If the client is expecting a 100 Continue response, we must
	// respond with either a 100 Continue or a final response before
	// waiting for the request body. The default is to always respond
	// with a 100 Continue. You can behave differently (for example,
	// reject unauthorized requests) by overriding this method.
	//
	// This method should either return true (possibly after sending
	// a 100 Continue response) or send an error response and return
	// false.
	virtual bool handle_expect_100();

	// Handle a single HTTP request.
	void handle_one_request();

	// This sends an error response (so it must be called before any
	// output has been generated), logs the error, and finally sends
	// a piece of HTML explaining the error to the user.
	void send_error(
			int code, const std::string& message="", const std::string& explain=""
	);

	// Add the response header to the headers buffer and log the
	// response code.
	//
	// Also send two standard headers with the server software
	// version and the current date.
	void send_response(int code, const std::string& message="");

	// Send the response header only.
	void send_response_only(int code, const std::string& message="");

	// Send a MIME header to the headers buffer.
	void send_header(const std::string& keyword, const std::string& value);

	// Send the blank line ending the MIME headers.
	void end_headers();

	void flush_headers();

	// Return the server software version string.
	[[nodiscard]]
	std::string version_string() const;

	// Return the current date and time formatted for a message header.
	[[nodiscard]]
	std::string datetime_string() const;

public:
	BaseHTTPRequestHandler(
		int sock, const std::string& server_version, timeval timeout, core::ILogger* logger,
		collections::Dict<std::string, std::string> env
	);

	// Handle multiple requests if necessary.
	void handle(HandlerFunc func);
};

__SERVER_END__
