/**
 * base.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <functional>
#include <memory>

// Core libraries.
#include <xalwart.core/thread_pool.h>
#include <xalwart.core/result.h>
#include <xalwart.core/server.h>

// Module definitions.
#include "./_def_.h"

// Framework libraries.
#include "./sock.h"
#include "./context.h"
#include "./parser.h"

#define MAX_BUFF_SIZE 8192 * 8 - 1  // 65535 bytes.

// 4K per header value.
// 35 main headers and 3 additional.
// 403 bytes is a length of all headers names.
#define MAX_HEADERS_SIZE 4096 * 38 + 403    // 156051 bytes.


__SERVER_BEGIN__

typedef std::function<void(const int, parser::request_parser*, core::Error*)> HandlerFunc;

class HTTPServer : public core::IServer
{
public:
	explicit HTTPServer(Context ctx, HandlerFunc handler);

	void bind(const std::string& address, uint16_t port) override;

	void listen(const std::string& message) override;

	void close() override;

	core::Error send(int sock, const char* data) override;

	core::Error write(int sock, const char* data, size_t n) override;

	void init_environ() override;

	[[nodiscard]]
	collections::Dict<std::string, std::string> environ() const override;

protected:
	std::string host;
	std::string server_name;
	uint16_t server_port = 0;
	Context ctx;
	collections::Dict<std::string, std::string> base_environ;

private:
	std::shared_ptr<core::ThreadPool> _threadPool;
	HandlerFunc _handler;
	std::shared_ptr<BaseSocket> _socket;

	enum read_result_enum
	{
		rr_continue = -1, rr_break = -2, rr_none = -3
	};

private:
	int _get_request();

	void _handle_request(int sock);

	void _handle(const int& sock);

	void _shutdown_request(int sock);

	core::Result<xw::string> _read_headers(size_t sock, xw::string& body_beginning);

	core::Result<xw::string> _read_body(
		size_t sock, const xw::string& body_beginning, size_t body_length
	);

	static int _error();
};

__SERVER_END__
