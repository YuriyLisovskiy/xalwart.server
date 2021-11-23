/**
 * socket/unix.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./unix.h"

#if defined(__linux__) || defined(__mac__)

// C++ libraries.
#include <sys/un.h>
#include <cstring>

// Server libraries.
#include "../exceptions.h"


__SERVER_BEGIN__

void UnixSocket::bind()
{
	sockaddr_un internet_socket_address{};
	internet_socket_address.sun_family = this->family;
	::strcpy(internet_socket_address.sun_path, this->address.c_str());
	if (::bind(this->socket, (const sockaddr*)&internet_socket_address, sizeof(internet_socket_address)))
	{
		auto error_code = errno;
		throw SocketError(error_code, "'bind' call failed: " + std::to_string(error_code), _ERROR_DETAILS_);
	}
}

void UnixSocket::close()
{
	BaseSocket::close();
	if (std::remove(this->address.c_str()) != 0)
	{
		throw FileError("unable to remove unix socket file: '" + this->address + "'", _ERROR_DETAILS_);
	}
}

__SERVER_END__

#endif // __linux__ || __mac__
