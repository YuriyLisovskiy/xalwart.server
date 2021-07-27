/**
 * http_server.h
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 *
 * Simple HTTP server implementation for development purposes.
 */

#pragma once

// C++ libraries.
#include <string>
#include <functional>
#include <memory>

// Base libraries.
#include <xalwart.base/kwargs.h>
#include <xalwart.base/event_loop.h>
#include <xalwart.base/result.h>
#include <xalwart.base/net/abc.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./context.h"
#include "./socket/base.h"
#include "./handlers/http_handler.h"


__SERVER_BEGIN__

class HTTPServer : public net::abc::IServer
{
private:
	std::shared_ptr<EventLoop> _event_loop;
	net::HandlerFunc _handler;
	std::shared_ptr<BaseSocket> _socket;

private:
	explicit HTTPServer(Context ctx);

	int _get_request();

	inline void _handle(const int& sock)
	{
		this->_event_loop->inject_event<RequestEvent>(sock);
	}

	void _shutdown_request(int sock) const;

protected:
	void init_environ() override;

protected:
	std::string host;
	std::string server_name;
	uint16_t server_port = 0;
	Context ctx;
	collections::Dict<std::string, std::string> base_environ;

protected:
	struct RequestEvent : public Event
	{
		int fd;
		explicit RequestEvent(int fd) : fd(fd) {}
	};

public:

	// Accepts parameters in kwargs:
	//
	// - workers: threads count;
	// - max_body_size: maximum size of request body (in bytes);
	// - timeout_sec: timeout seconds;
	// - timeout_usec: timeout microseconds.
	static std::shared_ptr<net::abc::IServer> initialize(
		log::ILogger* logger, const Kwargs& kwargs, std::shared_ptr<dt::Timezone> tz
	);

	inline void setup_handler(net::HandlerFunc handler) override
	{
		this->_handler = std::move(handler);
	}

	void bind(const std::string& address, uint16_t port) override;

	void listen(const std::string& message) override;

	void close() override;

	[[nodiscard]]
	inline collections::Dict<std::string, std::string> environ() const override
	{
		return this->base_environ;
	}
};

__SERVER_END__
