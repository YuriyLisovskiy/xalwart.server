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
#include <xalwart.core/interfaces/server.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket.h"
#include "./parser.h"
#include "./context.h"


__SERVER_BEGIN__

typedef std::function<void(const int, internal::request_parser*, core::Error*)> HandlerFunc;

class HTTPServer : public BaseSocket, public core::IServer
{
private:
	std::shared_ptr<core::internal::ThreadPool> _threadPool;
	HandlerFunc _handler;

protected:
	Context ctx;

public:
	explicit HTTPServer(const Context& ctx, HandlerFunc handler);

	bool bind(uint16_t port, bool use_ipv6) override;
	bool bind(const char* host, uint16_t port, bool use_ipv6) override;

	bool listen(const std::string& startup_message) override;

	void close() override;

	core::Error send(int sock, const char* data) override;
	core::Error write(int sock, const char* data, size_t n) override;

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
	static core::Result<xw::string> _read_headers(size_t sock, xw::string& body_beginning);
	static core::Result<xw::string> _read_body(
		size_t sock, const xw::string& body_beginning, size_t body_length
	);
};

__SERVER_END__
