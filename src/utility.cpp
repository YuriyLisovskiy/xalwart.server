/**
 * util.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./utility.h"

// C++ libraries.
#include <chrono>
#include <thread>
#include <unistd.h>

// Base libraries.
#include <xalwart.base/string_utils.h>

// Server libraries.
#include "./sockets/tcp.h"
#include "./sockets/tcp6.h"
#include "./sockets/unix.h"
#include "./exceptions.h"


__SERVER_UTIL_BEGIN__

std::string join_address(const std::string& address, uint16_t port)
{
	if (port != 0)
	{
		return address + ":" + std::to_string(port);
	}

	return address;
}

std::unique_ptr<abc::ISocket> create_server_socket(const std::string& address, uint16_t port)
{
	if (port != 0)
	{
		if (is_ipv6(address))
		{
			return std::make_unique<TCP6Socket>(address.c_str(), port);
		}

		return std::make_unique<TCPSocket>(address.c_str(), port);
	}

	return std::make_unique<UnixSocket>(address.c_str());
}

std::unique_ptr<abc::ISocket> create_server_socket(const std::string& address, uint16_t port, xw::abc::Logger* logger)
{
	require_non_null(logger, "'logger' is nullptr", _ERROR_DETAILS_);
	try
	{
		return create_server_socket(address, port);
	}
	catch (const SocketError& exc)
	{
		switch (exc.error_code())
		{
			case EADDRINUSE:
				logger->error("Connection in use: " + join_address(address, port));
				break;
			case EADDRNOTAVAIL:
				logger->error("Invalid address: " + join_address(address, port));
				break;
		}
	}

	return nullptr;
}

std::unique_ptr<abc::ISocket> create_server_socket(
	const std::string& address, uint16_t port, size_t retries_count, xw::abc::Logger* logger
)
{
	require_non_null(logger, "'logger' is nullptr", _ERROR_DETAILS_);
	std::unique_ptr<abc::ISocket> server_socket = nullptr;
	for (size_t i = 0; i < retries_count; i++)
	{
		server_socket = create_server_socket(address, port, logger);
		if (server_socket)
		{
			break;
		}

		if (i < retries_count)
		{
			logger->debug("connection to " + join_address(address, port) + " failed");
			logger->debug("Retrying in 1 second.");
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	return server_socket;
}

void close_socket(abc::ISocket* socket, xw::abc::Logger* logger)
{
	try
	{
		require_non_null(socket, "'socket' is nullptr", _ERROR_DETAILS_)->close();
	}
	catch (const SocketError& exc)
	{
		logger->error("Error while closing socket: " + std::string(exc.what()));
	}
}

std::string get_host_name()
{
	const int HOSTNAME_BUFFER_SIZE = 256;
	char hostname_buffer[HOSTNAME_BUFFER_SIZE];
	if (::gethostname(hostname_buffer, sizeof(hostname_buffer)))
	{
		auto err_code = errno;
		throw SocketError(err_code, "'gethostname' call failed: " + std::to_string(err_code), _ERROR_DETAILS_);
	}

	return hostname_buffer;
}

hostent* get_host_by_address(const std::string& address)
{
	in_addr internet_address{};
	hostent* host_entry;
	if (!::inet_aton(address.c_str(), &internet_address))
	{
		throw SocketError(1, "can't parse IP address " + address, _ERROR_DETAILS_);
	}

	host_entry = ::gethostbyaddr((const char *) &internet_address, sizeof(internet_address), INADDR_ANY);
	if (!host_entry)
	{
		throw SocketError(1, "no name associated with " + address, _ERROR_DETAILS_);
	}

	return host_entry;
}

std::string get_fully_qualified_domain_name(const std::string& name)
{
	auto result_name = str::trim(name);
	if (result_name.empty() || result_name == "0.0.0.0")
	{
		result_name = get_host_name();
	}

	try
	{
		auto* host_entry = get_host_by_address(result_name);
		if (host_entry->h_name && str::contains(host_entry->h_name, '.'))
		{
			return result_name;
		}

		bool is_broke = false;
		for (int i = 0; host_entry->h_aliases[i]; i++)
		{
			if (host_entry->h_aliases[i] && str::contains(host_entry->h_aliases[i], '.'))
			{
				is_broke = true;
				break;
			}
		}

		if (!is_broke && host_entry->h_name)
		{
			result_name = host_entry->h_name;
		}
	}
	catch (const SocketError&)
	{
	}

	return result_name;
}

__SERVER_UTIL_END__
