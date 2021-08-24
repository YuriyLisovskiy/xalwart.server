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

namespace p
{
inline const char* MAX_REQUEST_SIZE = "MAX_REQUEST_SIZE";
inline const char* WORKERS_COUNT = "WORKERS_COUNT";
inline const char* TIMEOUT_SECONDS = "TIMEOUT_SECONDS";
inline const char* TIMEOUT_MICROSECONDS = "TIMEOUT_MICROSECONDS";
inline const char* SOCKET_CREATION_RETRIES_COUNT = "SOCKET_CREATION_RETRIES_COUNT";
}

// TODO: docs for 'Context'
class Context final
{
private:
	template <typename T>
	static inline T _convert_to(const std::string& parameter, T default_value)
	{
		try
		{
			return util::as<T>(parameter.c_str());
		}
		catch (const std::invalid_argument&)
		{
		}
		catch (const std::range_error&)
		{
		}

		return default_value;
	}

	template <typename T>
	static inline T _get_parameter(
		const std::map<std::string, std::string>& parameters, const char* key, T default_value
	)
	{
		if (parameters.contains(key))
		{
			return _convert_to<T>(parameters.at(key), default_value);
		}

		return default_value;
	}

	inline void _from_map_parameters(const std::map<std::string, std::string>& parameters)
	{
		this->workers_count = _get_parameter<size_t>(parameters, p::WORKERS_COUNT, 3);
		this->max_request_size = _get_parameter<size_t>(parameters, p::MAX_REQUEST_SIZE, 2621440);
		this->timeout_seconds = _get_parameter<unsigned int>(parameters, p::TIMEOUT_SECONDS, 5);
		this->timeout_microseconds = _get_parameter<unsigned int>(parameters, p::TIMEOUT_MICROSECONDS, 0);
		this->socket_creation_retries_count = _get_parameter<size_t>(
			parameters, p::SOCKET_CREATION_RETRIES_COUNT, 5
		);
	}

public:
	log::ILogger* logger = nullptr;

	size_t max_request_size = 0;
	size_t workers_count = 0;
	time_t timeout_seconds = 0;
	time_t timeout_microseconds = 0;
	size_t socket_creation_retries_count = 0;

	Context() = default;

	inline explicit Context(
		log::ILogger* logger, const std::map<std::string, std::string>& parameters
	) : logger(logger)
	{
		this->_from_map_parameters(parameters);
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

	void handle_event(EventLoop& loop, RequestEvent& event);

	void event_function(EventLoop& loop, RequestEvent& event);

public:
	explicit HTTPServer(Context ctx, const std::shared_ptr<dt::Timezone>& /* timezone */);

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
