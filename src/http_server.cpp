/**
 * http_server.cpp
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 */

#include "./http_server.h"

// Core libraries.
#include <xalwart.core/net/meta.h>

// Server libraries.
#include "./util.h"


__SERVER_BEGIN__

std::shared_ptr<net::abc::IServer> HTTPServer::initialize(
	log::ILogger* logger,
	const collections::Dict<std::string, std::string>& kwargs
)
{
	Context ctx{};
	ctx.logger = logger;
	char* end_ptr = nullptr;
	auto workers = kwargs.get("workers", "3");
	ctx.workers = strtol(workers.c_str(), &end_ptr, 10);
	if (workers.c_str() == end_ptr)
	{
		throw ArgumentError("unable to read 'workers' parameter: " + workers, _ERROR_DETAILS_);
	}

	end_ptr = nullptr;
	auto max_body_size = kwargs.get("max_body_size", "2621440");
	ctx.max_body_size = strtol(max_body_size.c_str(), &end_ptr, 10);
	if (max_body_size.c_str() == end_ptr)
	{
		throw ArgumentError("unable to read 'max_body_size' parameter: " + max_body_size, _ERROR_DETAILS_);
	}

	end_ptr = nullptr;
	auto timeout_sec = kwargs.get("timeout_sec", "5");
	ctx.timeout_sec = strtol(timeout_sec.c_str(), &end_ptr, 10);
	if (timeout_sec.c_str() == end_ptr)
	{
		throw ArgumentError("unable to read 'timeout_sec' parameter: " + timeout_sec, _ERROR_DETAILS_);
	}

	end_ptr = nullptr;
	auto timeout_usec = kwargs.get("timeout_usec", "0");
	ctx.timeout_usec = strtol(timeout_usec.c_str(), &end_ptr, 10);
	if (timeout_usec.c_str() == end_ptr)
	{
		throw ArgumentError("unable to read 'timeout_usec' parameter: " + timeout_usec, _ERROR_DETAILS_);
	}

	return std::shared_ptr<net::abc::IServer>(new HTTPServer(ctx));
}

void HTTPServer::bind(const std::string& address, uint16_t port)
{
	if (!this->_handler)
	{
		throw NullPointerException(
			"request handler is not specified", _ERROR_DETAILS_
		);
	}

	this->_socket = util::create_socket(address, port, 5, this->ctx.logger);
	this->_socket->set_options();
	this->host = address;
	this->server_port = port;
	this->server_name = util::fqdn(this->host);
	this->init_environ();
}

void HTTPServer::listen(const std::string& message)
{
	this->_socket->listen();
	if (!message.empty())
	{
		this->ctx.logger->print(message);
	}

	SelectSelector selector(this->ctx.logger);
	selector.register_(this->_socket->fd(), EVENT_READ);
	while (!this->_socket->is_closed())
	{
		auto ready = selector.select(this->ctx.timeout_sec, this->ctx.timeout_usec);
		if (ready)
		{
			auto conn = this->_get_request();
			if (conn >= 0)
			{
				this->_handle(conn);
			}
		}
	}
}

void HTTPServer::close()
{
	this->_thread_pool->close();
	util::close_socket(this->_socket, this->ctx.logger);
}

void HTTPServer::init_environ()
{
	this->base_environ[net::meta::SERVER_NAME] = this->server_name;
	this->base_environ[net::meta::SERVER_PORT] = std::to_string(this->server_port);
}

HTTPServer::HTTPServer(Context ctx) : ctx(std::move(ctx))
{
	this->ctx.normalize();

	// TODO: replace ThreadPool with EventLoop.
	this->_thread_pool = std::make_shared<ThreadPool>(
		"server", this->ctx.workers
	);
}

int HTTPServer::_get_request()
{
	int new_sock;
	if ((new_sock = ::accept(this->_socket->fd(), nullptr, nullptr)) < 0)
	{
		if (errno == EBADF || errno == EINVAL)
		{
			throw SocketError(
				errno, "'accept' call failed: " + std::to_string(errno), _ERROR_DETAILS_
			);
		}

		if (errno == EMFILE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			return -1;
		}

		throw SocketError(
			errno,
			"'accept' call failed while accepting a new connection: " + std::to_string(errno),
			_ERROR_DETAILS_
		);
	}

	if (!this->_socket->is_closed() && new_sock >= 0)
	{
		return new_sock;
	}

	return -1;
}

void HTTPServer::_handle(const int& sock)
{
	this->_thread_pool->push([this, sock](){
		try
		{
			Measure measure;
			measure.start();

			timeval timeout{
				this->ctx.timeout_sec,
				this->ctx.timeout_usec
			};
			HTTPRequestHandler(
				sock, XW_SERVER_VERSION, timeout, this->ctx.max_body_size, this->ctx.logger, this->base_environ
			).handle(this->_handler);

			measure.end();
			this->ctx.logger->debug(
				"Time elapsed: " + std::to_string(measure.elapsed()) + " milliseconds"
			);
		}
		catch (const ParseError& exc)
		{
			this->_shutdown_request(sock);
			this->ctx.logger->error(exc);
		}
		catch (const std::exception& exc)
		{
			this->_shutdown_request(sock);
			this->ctx.logger->fatal(exc.what(), _ERROR_DETAILS_);
		}
	});
}

void HTTPServer::_shutdown_request(int sock) const
{
	if (shutdown(sock, SHUT_RDWR))
	{
		this->ctx.logger->error(
			"'shutdown' call failed: " + std::to_string(errno), _ERROR_DETAILS_
		);
	}

	::close(sock);
}

__SERVER_END__
