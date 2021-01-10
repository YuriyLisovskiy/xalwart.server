/**
 * handler.h
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
#include "./_def_.h"

// Server libraries.
#include "./socket/io.h"
#include "./request_context.h"


__SERVER_BEGIN__

typedef std::function<void(const int, RequestContext*, core::Error*)> HandlerFunc;

class HTTPRequestHandler
{
protected:
	HandlerFunc handler_func;

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

	timeval timeout;

	std::shared_ptr<SocketIO> socket_io;

	bool close_connection;

	std::string request_line;
	std::string request_version;
	std::string command;

	xw::string headers_buffer;

public:
	HTTPRequestHandler(
		int sock, const std::string& server_version, timeval timeout, core::ILogger* logger
	);

	// Handle a single HTTP request.
	void handle_one_request();

	// Handle multiple requests if necessary.
	void handle();

	// This sends an error response (so it must be called before any
	// output has been generated), logs the error, and finally sends
	// a piece of HTML explaining the error to the user.
	void send_error(
		int code, const std::string& message="", const std::string& explain=""
	);

	void send_response(int code, const std::string& message="");
	void send_response_only(int code, const std::string& message="");
	void send_header(const std::string& keyword, const std::string& value);
	void end_headers();
	void flush_headers();
};

__SERVER_END__
