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
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./sockets/base.h"


__SERVER_UTIL_BEGIN__

// TODO: docs for 'join_addr'
extern std::string join_addr(const std::string& addr, uint16_t port);

// TODO: docs for 'is_ipv6'
inline bool is_ipv6(const std::string& addr)
{
	return inet_pton(AF_INET6, addr.c_str(), nullptr) > 0;
}

// TODO: docs for 'create_socket'
extern std::shared_ptr<BaseSocket> create_socket(
	const std::string& address, uint16_t port, size_t retries_count, log::ILogger* logger
);

// TODO: docs for 'close_socket'
extern void close_socket(std::shared_ptr<BaseSocket>& socket, log::ILogger* logger);

// Wrapper for standard gethostname(char *__name, size_t __len) function.
extern std::string get_host_name();

// Wrapper for standard gethostbyaddr(const void *__addr, __socklen_t __len, int __type) function.
extern struct hostent* get_host_by_addr(const std::string& address);

// Get fully qualified domain name from name.
// An empty argument is interpreted as meaning the local host.
extern std::string fqdn(const std::string& name="");

__SERVER_UTIL_END__
