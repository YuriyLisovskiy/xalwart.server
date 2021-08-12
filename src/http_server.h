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
#include <map>

// Base libraries.
#include <xalwart.base/kwargs.h>
#include <xalwart.base/event_loop.h>
#include <xalwart.base/net/abc.h>
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./sockets/base.h"
#include "./handlers/http_handler.h"


__SERVER_BEGIN__

// TODO: docs for 'Context'
class Context
{
private:
	log::ILogger* _logger = nullptr;

public:
	size_t max_body_size = 0;
	size_t workers = 0;
	time_t timeout_sec = 0;
	time_t timeout_usec = 0;
	size_t retries_count = 0;

	inline explicit Context(log::ILogger* logger) : _logger(logger)
	{
	}

	[[nodiscard]]
	inline log::ILogger* logger() const
	{
		if (!this->_logger)
		{
			throw NullPointerException("xw::server::Context: logger is nullptr", _ERROR_DETAILS_);
		}

		return this->_logger;
	}
};

// TODO: docs for 'HTTPServer'
class HTTPServer : public net::abc::IServer
{
private:
	std::unique_ptr<EventLoop> _event_loop;
	net::HandlerFunc _handler;
	std::shared_ptr<BaseSocket> _socket;

private:
	explicit HTTPServer(Context ctx);

	int _get_request();

	inline void _handle(const int& fd)
	{
		this->_event_loop->inject_event<RequestEvent>(fd);
	}

	void _shutdown_request(int fd) const;

protected:
	void init_environ() override;

protected:
	std::string host;
	std::string server_name;
	uint16_t server_port = 0;
	Context ctx;
	std::map<std::string, std::string> base_environ;

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
	static std::unique_ptr<net::abc::IServer> initialize(
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
	inline collections::Dictionary<std::string, std::string> environ() const override
	{
		return this->base_environ;
	}
};

__SERVER_END__
