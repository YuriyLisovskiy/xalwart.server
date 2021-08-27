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
#include <map>
#include <memory>

// Base libraries.
#include <xalwart.base/sys.h>
#include <xalwart.base/net/request_context.h>
#include <xalwart.base/utility.h>
#include <xalwart.base/logger.h>
#include <xalwart.base/io.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../abc.h"


__SERVER_BEGIN__

using HandlerFunction = std::function<net::StatusCode(
	net::RequestContext* /* context */, const std::map<std::string, std::string>& /* environment */
)>;

// TODO: docs for 'BaseHTTPRequestHandler'
class BaseHTTPRequestHandler : public abc::IRequestHandler
{
public:
	BaseHTTPRequestHandler(
		std::unique_ptr<io::IStream> socket_stream,
		size_t max_request_size, std::string server_version, log::ILogger* logger,
		std::map<std::string, std::string> environment,
		HandlerFunction handler_function
	) : logger(logger),
	    socket_stream(std::move(socket_stream)),
	    max_request_size(max_request_size),
	    server_version_number(std::move(server_version)),
	    close_connection(false),
	    request_is_parsed(false),
	    environment(std::move(environment)),
	    total_bytes_read_count(0),
		handler_function(std::move(handler_function))
	{
		if (!this->handler_function)
		{
			throw NullPointerException("'handler_function' is nullptr", _ERROR_DETAILS_);
		}
	}

	// Handle multiple requests if necessary.
	void handle() override;

protected:
	log::ILogger* logger;

	HandlerFunction handler_function;

	net::RequestContext request_context;

	std::unique_ptr<io::IStream> socket_stream;

	// The server software number version.
	std::string server_version_number;

	// The default request version. This only affects responses up until
	// the point where the request line is parsed, so it mainly decides what
	// the client gets back when sending a malformed request line.
	// Most web servers default to HTTP 0.9, i.e. don't send a status line.
	std::string default_request_version = "HTTP/0.9";

	const std::string error_content_type = "text/html;charset=utf-8";

	const std::string protocol_version = "HTTP/1.1";

	bool close_connection;

	std::string raw_request_line;
	std::string request_version;
	std::string command;
	std::string full_path;

	size_t max_request_size;
	size_t total_bytes_read_count;

	std::string headers_buffer;

	bool request_is_parsed;

	std::map<std::string, std::string> environment;

	[[nodiscard]]
	virtual std::string default_error_message(
		unsigned int code, const std::string& phrase, const std::string& description
	) const;

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
	void send_error(unsigned int code, const std::string& message="", const std::string& explain="");

	// Add the response header to the headers buffer and log the
	// response code.
	//
	// Also send two standard headers with the server software
	// version and the current date.
	void send_response(unsigned int code, const std::string& message="");

	// Send the response header only.
	void send_response_only(unsigned int code, std::string message="");

	// Send a MIME header to the headers buffer.
	void send_header(const std::string& keyword, const std::string& value);

	// Send the blank line ending the MIME headers.
	void end_headers();

	inline void flush_headers()
	{
		this->socket_stream->write(this->headers_buffer.c_str(), (ssize_t)this->headers_buffer.size());
		this->headers_buffer = "";
	}

	// Return the server software version string.
	[[nodiscard]]
	inline std::string version_string() const
	{
		return this->server_version() + " " + sys::compiler + "/" + sys::compiler_version;
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
		return "BaseHTTPServer/" + this->server_version_number;
	}

	virtual bool read_line(std::string& destination);

	virtual bool write(const char* content, ssize_t count);

	virtual bool parse_headers();

	virtual inline void close_io() const
	{
		if (!this->socket_stream->close_reader())
		{
			this->logger->error("failed to close socket reader: " + std::to_string(errno), _ERROR_DETAILS_);
		}

		if (!this->socket_stream->close_writer())
		{
			this->logger->error("failed to close socket writer: " + std::to_string(errno), _ERROR_DETAILS_);
		}
	}
};

__SERVER_END__
