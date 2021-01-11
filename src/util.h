/**
 * util.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <memory>
#include <string>

// Core libraries.
#include <xalwart.core/logger.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket/base.h"


__SERVER_UTIL_BEGIN__

extern std::string join_addr(const std::string& addr, uint16_t port);

extern bool is_ipv6(const std::string& addr);

extern std::shared_ptr<BaseSocket> create_socket(
	const std::string& address, uint16_t port, size_t retries_count, core::ILogger* logger
);

extern void close_socket(
	std::shared_ptr<BaseSocket>& socket, core::ILogger* logger
);

// Wrapper for standard gethostname(char *__name, size_t __len) function.
extern std::string get_host_name();

// Wrapper for standard gethostbyaddr(const void *__addr, __socklen_t __len, int __type) function.
extern struct hostent* get_host_by_addr(const std::string& address);

// Get fully qualified domain name from name.
// An empty argument is interpreted as meaning the local host.
extern std::string fqdn(const std::string& name="");

__SERVER_UTIL_END__
