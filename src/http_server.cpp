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


__SERVER_BEGIN__

std::shared_ptr<net::abc::IServer> HTTPServer::initialize(
	log::ILogger* logger, const Kwargs& kwargs, std::shared_ptr<dt::Timezone>
)
{
	Context ctx{};
	ctx.logger = logger;
	ctx.workers = kwargs.get<unsigned long int>("workers", 3);
	ctx.max_body_size = kwargs.get<unsigned long int>("max_body_size", 2621440);
	ctx.timeout_sec = (time_t)kwargs.get<unsigned long int>("timeout_sec", 5);
	ctx.timeout_usec = (time_t)kwargs.get<unsigned long int>("timeout_usec", 0);
	ctx.retries_count = kwargs.get<unsigned long int>("retries_count", 5);
	return std::shared_ptr<net::abc::IServer>(new HTTPServer(ctx));
}

void HTTPServer::bind(const std::string& address, uint16_t port)
{
	if (!this->_handler)
	{
		throw NullPointerException("server::HTTPServer: request handler is nullptr", _ERROR_DETAILS_);
	}

	this->_socket = util::create_socket(address, port, this->ctx.retries_count, this->ctx.logger);
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
	this->_event_loop->wait_for_threads();
	util::close_socket(this->_socket, this->ctx.logger);
}

void HTTPServer::init_environ()
{
	this->base_environ[net::meta::SERVER_NAME] = this->server_name;
	this->base_environ[net::meta::SERVER_PORT] = std::to_string(this->server_port);
}

HTTPServer::HTTPServer(Context ctx) : ctx(ctx)
{
	this->ctx.normalize();
	this->_event_loop = std::make_shared<EventLoop>(this->ctx.workers);
	this->_event_loop->add_event_listener<RequestEvent>([this](auto&, auto& event) {
		try
		{
			Measure measure;
			measure.start();

			timeval timeout{
				this->ctx.timeout_sec,
				this->ctx.timeout_usec
			};
			HTTPRequestHandler(
				event.fd, v::version.to_string(), timeout,
				this->ctx.max_body_size, this->ctx.logger, this->base_environ
			).handle(this->_handler);

			measure.end();
			this->ctx.logger->debug(
				"Time elapsed: " + std::to_string(measure.elapsed()) + " milliseconds"
			);
		}
		catch (const ParseError& exc)
		{
			this->_shutdown_request(event.fd);
			this->ctx.logger->error(exc);
		}
		catch (const std::exception& exc)
		{
			this->_shutdown_request(event.fd);
			this->ctx.logger->fatal(exc.what(), _ERROR_DETAILS_);
		}
	});
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
