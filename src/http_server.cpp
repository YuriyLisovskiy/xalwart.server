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

HTTPServer::HTTPServer(Context ctx, HandlerFunc handler)
	: ctx(std::move(ctx)), _handler(std::move(handler))
{
	this->ctx.normalize();
	this->_thread_pool = std::make_shared<core::ThreadPool>(
		"server", this->ctx.workers
	);
	if (!this->_handler)
	{
		throw core::NullPointerException(
			"request handler is not specified", _ERROR_DETAILS_
		);
	}
}

void HTTPServer::bind(const std::string& address, uint16_t port)
{
	this->_socket = util::create_socket(address, port, 5, this->ctx.logger.get());
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

	SelectSelector selector(this->ctx.logger.get());
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
	util::close_socket(this->_socket, this->ctx.logger.get());
}

void HTTPServer::init_environ()
{
	this->base_environ[net::meta::SERVER_NAME] = this->server_name;
	this->base_environ[net::meta::SERVER_PORT] = std::to_string(this->server_port);
}

collections::Dict<std::string, std::string> HTTPServer::environ() const
{
	return this->base_environ;
}

int HTTPServer::_get_request()
{
	int new_sock;
	if ((new_sock = ::accept(this->_socket->fd(), nullptr, nullptr)) < 0)
	{
		if (errno == EBADF || errno == EINVAL)
		{
			throw core::SocketError(
				errno, "'accept' call failed: " + std::to_string(errno), _ERROR_DETAILS_
			);
		}

		if (errno == EMFILE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			return -1;
		}

		throw core::SocketError(
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
				sock, "0.1", timeout, this->ctx.logger.get(), this->base_environ
			).handle(this->_handler);

			this->_shutdown_request(sock);
			measure.end();
			this->ctx.logger->debug(
				"Time elapsed: " + std::to_string(measure.elapsed()) + " milliseconds"
			);
		}
		catch (const core::ParseError& exc)
		{
			this->_shutdown_request(sock);
			this->ctx.logger->error(exc);
		}
	});
}

void HTTPServer::_shutdown_request(int sock)
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
