/**
 * util.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Server utilities.
 */

#pragma once

// C++ libraries.
#include <memory>
#include <string>
#if defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#elif _WIN32
#include <winsock32.h>
#endif

// Base libraries.
#include <xalwart.base/abc/base.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./abc.h"


__SERVER_UTIL_BEGIN__

// TODO: docs for 'join_address'
extern std::string join_address(const std::string& address, uint16_t port);

// TODO: docs for 'is_ipv6'
inline bool is_ipv6(const std::string& addr)
{
	return ::inet_pton(AF_INET6, addr.c_str(), nullptr) > 0;
}

// TODO: docs for 'create_socket'
extern std::unique_ptr<abc::ISocket> create_server_socket(const std::string& address, uint16_t port);

// TODO: docs for 'create_socket'
extern std::unique_ptr<abc::ISocket> create_server_socket(
	const std::string& address, uint16_t port, xw::abc::ILogger* logger
);

// TODO: docs for 'create_socket'
extern std::unique_ptr<abc::ISocket> create_server_socket(
	const std::string& address, uint16_t port, size_t retries_count, xw::abc::ILogger* logger
);

// TODO: docs for 'close_socket'
extern void close_socket(abc::ISocket* socket, xw::abc::ILogger* logger);

// Wrapper for standard gethostname(char *__name, size_t __len) function.
extern std::string get_host_name();

// Wrapper for standard gethostbyaddr(const void *__addr, __socklen_t __len, int __type) function.
extern hostent* get_host_by_address(const std::string& address);

// Get fully qualified domain name from name.
// An empty argument is interpreted as meaning the local host.
extern std::string get_fully_qualified_domain_name(const std::string& name="");

// TESTME: socket_is_valid
// TODO: docs for 'socket_is_valid'
inline bool socket_is_valid(Socket socket)
{
	return socket >= 0;
}

__SERVER_UTIL_END__
