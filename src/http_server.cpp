/**
 * http_server.cpp
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 */

#include "./http_server.h"

// Base libraries.
#include <xalwart.base/net/meta.h>
#include <xalwart.base/workers/threaded_worker.h>

// Server libraries.
#include "./utility.h"
#include "./exceptions.h"


__SERVER_BEGIN__

void HTTPServer::handle_event(AbstractWorker*, RequestTask& task)
{
	Measure measure;
	measure.start();

	auto socket_stream = this->context.create_stream(this->context, task.client.socket());
	auto request_handler = this->context.create_request_handler(
		this->context, std::move(socket_stream), this->environment
	);
	if (!request_handler)
	{
		throw NullPointerException("'request_handler' is nullptr", _ERROR_DETAILS_);
	}

	request_handler->handle();

	measure.end();
	this->context.logger->debug("Time elapsed: " + std::to_string(measure.elapsed()) + " milliseconds");
}

void HTTPServer::event_function(AbstractWorker* worker, RequestTask& task)
{
	try
	{
		this->handle_event(worker, task);
	}
	catch (const ServerError& exc)
	{
		this->_shutdown_client(task.client);
		this->context.logger->error(exc);
	}
	catch (const std::exception& exc)
	{
		this->_shutdown_client(task.client);
		this->context.logger->error(exc.what(), _ERROR_DETAILS_);
	}
}

HTTPServer::HTTPServer(Context context) : context(std::move(context))
{
	this->context.set_defaults();
	this->context.validate();
	this->_worker = std::make_unique<ThreadedWorker>(this->context.workers_count);
	this->_worker->add_task_listener<RequestTask>(
		[this](auto&& worker, auto&& task) {
			this->event_function(std::forward<decltype(worker)>(worker), std::forward<decltype(task)>(task));
		}
	);
}

void HTTPServer::bind(const std::string& address, uint16_t port)
{
	this->_socket = util::create_server_socket(
		address, port, this->context.socket_creation_retries_count, this->context.logger
	);
	this->_socket->set_options();
	this->host = address;
	this->server_port = port;
	this->server_name = util::get_fully_qualified_domain_name(this->host);
	this->initialize_environment();
}

void HTTPServer::listen(const std::string& message)
{
	this->_socket->listen();
	auto selector = this->context.create_selector(this->context, this->_socket->raw_socket());
	selector->register_read_event();
	if (!message.empty())
	{
		this->context.logger->print(message);
	}

	while (this->_socket->is_open())
	{
		if (selector->select(this->context.timeout_seconds, this->context.timeout_microseconds))
		{
			auto client = this->_accept_client();
			if (this->_socket->is_open() && client.is_valid())
			{
				this->_worker->inject_task<RequestTask>(client);
			}
		}
	}
}

void HTTPServer::close()
{
	this->_worker->stop();
	util::close_socket(this->_socket.get(), this->context.logger);
}

void HTTPServer::initialize_environment()
{
	this->environment.insert(std::make_pair(net::meta::SERVER_NAME, this->server_name));
	this->environment.insert(std::make_pair(net::meta::SERVER_PORT, std::to_string(this->server_port)));
}

Client HTTPServer::_accept_client() const
{
	auto client = Client{::accept(this->_socket->raw_socket(), nullptr, nullptr)};
	if (!client.is_valid())
	{
		auto error_code = errno;
		if (error_code == EBADF || error_code == EINVAL)
		{
			throw SocketError(
				error_code, "'accept' call failed: " + std::to_string(error_code), _ERROR_DETAILS_
			);
		}

		if (error_code == EMFILE)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			return client;
		}

		throw SocketError(
			error_code, "'accept' call failed while accepting a new connection: " + std::to_string(error_code),
			_ERROR_DETAILS_
		);
	}

	return client;
}

void HTTPServer::_shutdown_client(Client client) const
{
	if (shutdown(client.socket(), SHUT_RDWR))
	{
		this->context.logger->error("'shutdown' call failed: " + std::to_string(errno), _ERROR_DETAILS_);
	}

	::close(client.socket());
}

__SERVER_END__
