/**
 * http_server.cpp
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 */

#include "./http_server.h"

// Base libraries.
#include <xalwart.base/net/meta.h>

// Server libraries.
#include "./util.h"
#include "./selectors.h"
#include "./sockets/io.h"
#include "./exceptions.h"


__SERVER_BEGIN__

void HTTPServer::handle_event(EventLoop& loop, RequestEvent& event)
{
	Measure measure;
	measure.start();

	timeval timeout{this->ctx.timeout_seconds, this->ctx.timeout_microseconds};
	auto socket_io = std::make_unique<SocketIO>(
		event.fd, timeout, std::make_unique<Selector>(this->ctx.logger)
	);
	HTTPRequestHandler handler(
		socket_io.get(), socket_io.get(), v::version.to_string(),
		this->ctx.max_request_size, this->ctx.logger, this->base_environ
	);
	handler.handle(this->_handler);

	measure.end();
	this->ctx.logger->debug("Time elapsed: " + std::to_string(measure.elapsed()) + " milliseconds");
}

void HTTPServer::event_function(EventLoop& loop, RequestEvent& event)
{
	try
	{
		this->handle_event(loop, event);
	}
	catch (const ServerError& exc)
	{
		this->_shutdown_request(event.fd);
		this->ctx.logger->error(exc);
	}
	catch (const std::exception& exc)
	{
		this->_shutdown_request(event.fd);
		this->ctx.logger->fatal(exc.what(), _ERROR_DETAILS_);
	}
}

HTTPServer::HTTPServer(Context ctx, const std::shared_ptr<dt::Timezone>& /* timezone */) : ctx(ctx)
{
	this->_event_loop = std::make_unique<EventLoop>(this->ctx.workers_count);
	this->_event_loop->add_event_listener<RequestEvent>(
		[this](auto&& loop, auto&& event) {
			this->event_function(std::forward<decltype(loop)>(loop), std::forward<decltype(event)>(event));
		}
	);
}

void HTTPServer::bind(const std::string& address, uint16_t port)
{
	if (!this->_handler)
	{
		throw NullPointerException("xw::server::HTTPServer: request handler is nullptr", _ERROR_DETAILS_);
	}

	this->_socket = util::create_socket(
		address, port, this->ctx.socket_creation_retries_count, this->ctx.logger
	);
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

	Selector selector(this->ctx.logger);
	selector.register_(this->_socket->fd(), EVENT_READ);
	while (!this->_socket->is_closed())
	{
		if (selector.select(this->ctx.timeout_seconds, this->ctx.timeout_microseconds))
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
	this->_event_loop->wait_for_threads();
	util::close_socket(this->_socket.get(), this->ctx.logger);
}

void HTTPServer::init_environ()
{
	this->base_environ.insert(std::make_pair(net::meta::SERVER_NAME, this->server_name));
	this->base_environ.insert(std::make_pair(net::meta::SERVER_PORT, std::to_string(this->server_port)));
}

int HTTPServer::_get_request()
{
	int new_sock;
	if ((new_sock = ::accept(this->_socket->fd(), nullptr, nullptr)) < 0)
	{
		auto err_code = errno;
		if (err_code == EBADF || err_code == EINVAL)
		{
			throw SocketError(err_code, "'accept' call failed: " + std::to_string(err_code), _ERROR_DETAILS_);
		}

		if (err_code == EMFILE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			return -1;
		}

		throw SocketError(
			err_code, "'accept' call failed while accepting a new connection: " + std::to_string(err_code),
			_ERROR_DETAILS_
		);
	}

	if (!this->_socket->is_closed() && new_sock >= 0)
	{
		return new_sock;
	}

	return -1;
}

void HTTPServer::_shutdown_request(int fd) const
{
	if (shutdown(fd, SHUT_RDWR))
	{
		this->ctx.logger->error("'shutdown' call failed: " + std::to_string(errno), _ERROR_DETAILS_);
	}

	::close(fd);
}

__SERVER_END__
