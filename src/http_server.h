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
#include <xalwart.base/interfaces/server.h>
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./interfaces.h"
#include "./context.h"


__SERVER_BEGIN__

// TESTME: Client
// TODO: docs for 'Client'
class Client final
{
public:
	inline Client() : _socket(-1)
	{
	}

	explicit inline Client(Socket socket) : _socket(socket)
	{
	}

	[[nodiscard]]
	inline bool is_valid() const
	{
		return this->_socket >= 0;
	}

	[[nodiscard]]
	inline Socket socket() const
	{
		return this->_socket;
	}

private:
	Socket _socket;
};

// TESTME: DevelopmentHTTPServer
// TODO: docs for 'DevelopmentHTTPServer'
class DevelopmentHTTPServer : public IServer
{
public:
	explicit DevelopmentHTTPServer(Context context);

	void bind(const std::string& address, uint16_t port) override;

	void listen(const std::string& message) override;

	void close() override;

	[[nodiscard]]
	inline std::map<std::string, std::string> get_environment() const override
	{
		return this->environment;
	}

	[[nodiscard]]
	inline bool is_development() const override
	{
		return true;
	}

protected:
	std::string host;
	std::string server_name;
	uint16_t server_port = 0;
	std::map<std::string, std::string> environment;
	Context context;

struct RequestTask : public AbstractWorker::Task
	{
		Client client;

		explicit inline RequestTask(Client client) : client(client)
		{
		}
	};

	void handle_event(AbstractWorker* worker, RequestTask& task);

	void event_function(AbstractWorker* worker, RequestTask& task);

	void initialize_environment() override;

private:
	std::unique_ptr<ISocket> _socket;

	[[nodiscard]]
	Client _accept_client() const;

	void _shutdown_client(Client client) const;
};

__SERVER_END__
