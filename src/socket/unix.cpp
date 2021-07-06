/**
 * socket/unix.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./unix.h"

#ifdef __unix__

// C++ libraries.
#include <sys/un.h>

// Core libraries.
#include <xalwart.core/exceptions.h>


__SERVER_BEGIN__

void UnixSocket::bind()
{
	sockaddr_un addr{};
	addr.sun_family = this->family;
	strcpy(addr.sun_path, this->address.c_str());
	if (::bind(this->sock, (const sockaddr *)&addr, sizeof(addr)))
	{
		auto err = errno;
		throw SocketError(
			err, "'bind' call failed: " + std::to_string(err), _ERROR_DETAILS_
		);
	}
}

void UnixSocket::close()
{
	BaseSocket::close();
	if (std::remove(this->address.c_str()) != 0)
	{
		throw FileError(
			"unable to remove unix socket file: '" + this->address + "'",
			_ERROR_DETAILS_
		);
	}
}

__SERVER_END__

#endif // __unix__
