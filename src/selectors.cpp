/**
 * selectors.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./selectors.h"

// C++ libraries.
#include <cerrno>
#include <cstring>

// Server libraries.
#include "./utility.h"
#include "./exceptions.h"


__SERVER_BEGIN__

Selector::Selector(Socket socket, xw::abc::ILogger* logger) : logger(logger), socket(socket)
{
	if (!this->logger)
	{
		throw NullPointerException("'logger' is nullptr", _ERROR_DETAILS_);
	}

	if (!util::socket_is_valid(this->socket))
	{
		throw ArgumentError("socket is invalid", _ERROR_DETAILS_);
	}
}

void Selector::register_read_event()
{
	FD_ZERO(&this->readers);
	FD_SET(this->socket, &this->readers);
}

void Selector::register_write_event()
{
	FD_ZERO(&this->writers);
	FD_SET(this->socket, &this->writers);
}

bool Selector::select(uint timeout_seconds, uint timeout_microseconds)
{
	const int STATUS_TIMEOUT = 0, STATUS_INVALID = -1;
	auto fd_writers = this->writers;
	auto fd_readers = this->readers;
	timeval t_val{
		.tv_sec = timeout_seconds,
		.tv_usec = (int)timeout_microseconds
	};
	int select_status = ::select(this->socket + 1, &fd_readers, &fd_writers, nullptr, &t_val);
	if (select_status == STATUS_INVALID)
	{
		this->logger->error("'select' call failed: " + std::string(strerror(errno)), _ERROR_DETAILS_);
	}
	else if (select_status == STATUS_TIMEOUT)
	{
	}
	else
	{
		return true;
	}

	return false;
}

__SERVER_END__
