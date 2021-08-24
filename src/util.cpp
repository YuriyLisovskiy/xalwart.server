/**
 * util.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./util.h"

// C++ libraries.
#include <chrono>

// Base libraries.
#include <xalwart.base/string_utils.h>

// Server libraries.
#include "./sockets/tcp.h"
#include "./sockets/tcp6.h"
#include "./sockets/unix.h"
#include "./exceptions.h"


__SERVER_UTIL_BEGIN__

std::string join_addr(const std::string& addr, uint16_t port)
{
	auto result = addr;
	if (port)
	{
		result += ":" + std::to_string(port);
	}

	return result;
}

std::shared_ptr<BaseSocket> create_socket(
	const std::string& address, uint16_t port, size_t retries_count, log::ILogger* logger
)
{
	std::function<std::shared_ptr<BaseSocket>()> make_socket;
	if (port)
	{
		if (is_ipv6(address))
		{
			make_socket = [address, port]() {
				return std::make_shared<TCP6Socket>(address.c_str(), port);
			};
		}
		else
		{
			make_socket = [address, port]() {
				return std::make_shared<TCPSocket>(address.c_str(), port);
			};
		}

	}
	else
	{
		make_socket = [address]() {
			return std::make_shared<UnixSocket>(address.c_str());
		};
	}

	std::shared_ptr<BaseSocket> socket;
	for (size_t i = 0; i < retries_count; i++)
	{
		try
		{
			socket = make_socket();
			break;
		}
		catch (const SocketError& exc)
		{
			if (!logger)
			{
				throw NullPointerException("'logger' is nullptr", _ERROR_DETAILS_);
			}

			switch (exc.error_code())
			{
				case EADDRINUSE:
					logger->error("Connection in use: " + join_addr(address, port));
					break;
				case EADDRNOTAVAIL:
					logger->error("Invalid address: " + join_addr(address, port));
					break;
			}

			if (i < retries_count)
			{
				logger->debug("connection to " + join_addr(address, port) + " failed: " + exc.what());
				logger->debug("Retrying in 1 second.");
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		}
	}

	return socket;
}

void close_socket(BaseSocket* socket, log::ILogger* logger)
{
	if (!socket)
	{
		throw NullPointerException("'socket' is nullptr", _ERROR_DETAILS_);
	}

	try
	{
		socket->close();
	}
	catch (const SocketError& exc)
	{
		logger->error("Error while closing socket: " + std::string(exc.what()));
	}
}

std::string get_host_name()
{
	char host_buffer[256];
	if (gethostname(host_buffer, sizeof(host_buffer)))
	{
		auto err_code = errno;
		throw SocketError(err_code, "'gethostname' call failed: " + std::to_string(err_code), _ERROR_DETAILS_);
	}

	return host_buffer;
}

struct hostent* get_host_by_addr(const std::string& address)
{
	struct in_addr ip{};
	hostent* hp;
	if (!inet_aton(address.c_str(), &ip))
	{
		throw SocketError(1, "can't parse IP address " + address, _ERROR_DETAILS_);
	}

	if ((hp = gethostbyaddr((const char *) &ip, sizeof(ip), INADDR_ANY)) == nullptr)
	{
		throw SocketError(1, "no name associated with " + address, _ERROR_DETAILS_);
	}

	return hp;
}

std::string fqdn(const std::string& name)
{
	auto nm = str::trim(name);
	if (nm.empty() || nm == "0.0.0.0")
	{
		nm = get_host_name();
	}

	try
	{
		auto ht = get_host_by_addr(nm);
		if (ht->h_name && str::contains(ht->h_name, '.'))
		{
			return nm;
		}

		bool is_broke = false;
		for (int i = 0; ht->h_aliases[i]; i++)
		{
			if (ht->h_aliases[i] && str::contains(ht->h_aliases[i], '.'))
			{
				is_broke = true;
				break;
			}
		}

		if (!is_broke && ht->h_name)
		{
			nm = ht->h_name;
		}
	}
	catch (const SocketError&)
	{
	}

	return nm;
}

__SERVER_UTIL_END__
