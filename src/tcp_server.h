/**
 * tcp_server.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <functional>
#include <thread>
#include <memory>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket/tcp.h"


__SERVER_BEGIN__

class TCPServer : public BaseSocket
{
public:
	// Event Listeners:
	std::function<void(const std::shared_ptr<TCPSocket>&)> onNewConnection = [](const std::shared_ptr<TCPSocket>& sock) { FDR_UNUSED(sock); };

	explicit TCPServer(const OnErrorFunc& onError);

	// Binding the server.
	bool Bind(int port, const OnErrorFunc& onError);
	bool Bind6(int port, const OnErrorFunc& onError);
	bool Bind(const char* address, uint16_t port, const OnErrorFunc& onError);
	bool Bind6(const char* address, uint16_t port, const OnErrorFunc& onError);

	// Start listening the server.
	bool Listen(const OnErrorFunc& onError);

	// Overriding Close to add shutdown():
	void Close() override;

private:
	bool _accept(const OnErrorFunc& onError);
};

__SERVER_END__
