/**
 * http_server.h
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
#include <xalwart.core/net/server.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./server_context.h"
#include "./socket/base.h"
#include "./handlers/http_handler.h"


__SERVER_BEGIN__

class HTTPServer : public net::IServer
{
public:
	explicit HTTPServer(Context ctx, HandlerFunc handler);

	void bind(const std::string& address, uint16_t port) override;

	void listen(const std::string& message) override;

	void close() override;

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
	std::shared_ptr<core::ThreadPool> _thread_pool;
	HandlerFunc _handler;
	std::shared_ptr<BaseSocket> _socket;

private:
	int _get_request();

	void _handle(const int& sock);

	void _shutdown_request(int sock);
};

__SERVER_END__
