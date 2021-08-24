/**
 * selectors.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./selectors.h"

// C++ libraries.
#include <cerrno>
#include <cstring>


__SERVER_BEGIN__

void Selector::register_(uint file_descriptor, int events_)
{
	this->fd = (int)file_descriptor;
	this->events = events_;
	if (this->events & EVENT_READ)
	{
		FD_ZERO(&this->readers);
		FD_SET(this->fd, &this->readers);
	}

	if (this->events & EVENT_WRITE)
	{
		FD_ZERO(&this->writers);
		FD_SET(this->fd, &this->writers);
	}
}

bool Selector::select(uint timeout_sec, uint timeout_usec)
{
	auto fd_writers = this->writers;
	auto fd_readers = this->readers;
	struct timeval t_val{};
	t_val.tv_sec = timeout_sec;
	t_val.tv_usec = timeout_usec;
	int ret = ::select(this->fd + 1, &fd_readers, &fd_writers, nullptr, &t_val);
	if (ret == -1)
	{
		if (!this->logger)
		{
			throw NullPointerException("xw::server::SelectSelector: logger is nullptr", _ERROR_DETAILS_);
		}

		this->logger->error("'select' call failed: " + std::string(strerror(errno)), _ERROR_DETAILS_);
	}
	else if (ret == 0)
	{
		// timeout, just skip
	}
	else
	{
		return true;
	}

	return false;
}

__SERVER_END__
