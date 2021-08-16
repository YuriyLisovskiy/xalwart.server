/**
 * handlers/base_http_handler.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 * Based on Python 3.6.9 HTTP server.
 *
 * Reads and parses only HTTP headers, performs basic checks
 * of an incoming request, returns error in case of invalid
 * data and calls provided handler of HandlerFunc type with
 * parsed context and local server's environment.
 */

#pragma once

// C++ libraries.
#include <string>
#include <memory>

// Base libraries.
#include <xalwart.base/sys.h>
#include <xalwart.base/net/request_context.h>
#include <xalwart.base/datetime.h>
#include <xalwart.base/utility.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../sockets/io.h"
#include "../parser.h"


__SERVER_BEGIN__

// TODO: docs for 'BaseHTTPRequestHandler'
class BaseHTTPRequestHandler
{
protected:
	std::shared_ptr<log::ILogger> logger;

	net::HandlerFunc handler_func;

	net::RequestContext request_ctx;

	std::unique_ptr<SocketIO> socket_io;

	std::string sys_version = sys::compiler + "/" + sys::compiler_version;

	// The server software number version.
	std::string server_num_version;

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

	std::string headers_buffer;

	bool parsed;

	std::map<std::string, std::string> env;

protected:
	[[nodiscard]]
	virtual std::string default_error_message(int code, const std::string& message, const std::string& explain) const;

	void log_socket_error(SocketIO::State state) const;

	void log_parse_headers_error(parser::ParseHeadersStatus status) const;

	virtual void log_request(uint code, const std::string& info) const;

	virtual void cleanup_headers();

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
	void send_error(int code, const std::string& message="", const std::string& explain="");

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

	inline void flush_headers()
	{
		this->socket_io->write(this->headers_buffer.c_str(), this->headers_buffer.size());
		this->headers_buffer = "";
	}

	// Return the server software version string.
	[[nodiscard]]
	inline std::string version_string() const
	{
		return this->server_version() + " " + this->sys_version;
	}

	// Return the current date and time formatted for a message header.
	[[nodiscard]]
	inline virtual std::string datetime_string() const
	{
		return util::format_date((time_t)dt::Datetime::utc_now().timestamp(), false, true);
	}

	// The server software version.
	[[nodiscard]]
	inline virtual std::string server_version() const
	{
		return "BaseHTTP/" + this->server_num_version;
	}

public:
	BaseHTTPRequestHandler(
		int sock, std::string server_version, timeval timeout,
		log::ILogger* logger, std::map<std::string, std::string> env
	) : logger(std::move(logger)),
		server_num_version(std::move(server_version)), close_connection(false), parsed(false), env(std::move(env))
	{
		if (!this->logger)
		{
			throw NullPointerException("'logger' is nullptr", _ERROR_DETAILS_);
		}

		this->socket_io = std::make_unique<SocketIO>(sock, timeout, std::make_unique<SelectSelector>(logger));
	}

	// Handle multiple requests if necessary.
	virtual void handle(net::HandlerFunc func);
};

__SERVER_END__
