/**
 * tcp_server.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <functional>
#include <thread>
#include <memory>

// Core libraries.
#include <xalwart.core/thread_pool.h>
#include <xalwart.core/result.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket.h"
#include "./parser.h"
#include "./context.h"


__SERVER_BEGIN__

typedef std::function<void(const int, internal::request_parser*, core::Error*)> HandlerFunc;

class HTTPServer : public BaseSocket
{
private:
	std::shared_ptr<core::internal::ThreadPool> _threadPool;
	Context _ctx;
	HandlerFunc _handler;

public:
	explicit HTTPServer(const Context& ctx, HandlerFunc handler);

	// Binding the server.
	bool bind(uint16_t port, bool useIPv6);
	bool bind(const char* host, uint16_t port, bool useIPv6);

	// Start listening the server.
	bool listen(const std::string& startupMessage = "");

	// Overriding Close to add shutdown():
	void close() override;

	core::Error send(int sock, const char* data);
	core::Error write(int sock, const char* data, size_t bytes_to_write);

private:
	bool _bind(uint16_t port);
	bool _bind(const char* host, uint16_t port);
	bool _bind6(uint16_t port);
	bool _bind6(const char* host, uint16_t port);
	static bool _accept(HTTPServer* s);

	enum read_result_enum
	{
		rr_continue = -1, rr_break = -2, rr_none = -3
	};
	void _handleConnection(const int& sock);

	static int _error();
	static core::Result<xw::string> _read_headers(const int& sock, xw::string& body_beginning);
	static core::Result<xw::string> _read_body(
		const int& sock, const xw::string& body_beginning, size_t body_length
	);
};

__SERVER_END__
