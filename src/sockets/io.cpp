/**
 * socket/io.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./io.h"

// C++ libraries.
#include <cstring>


__SERVER_BEGIN__

SocketIO& SocketIO::operator= (SocketIO&& other) noexcept
{
	this->_fd = other._fd;
	this->_timeout = other._timeout;
	strcpy(this->_buffer, other._buffer);
	this->_buffer_size = other._buffer_size;
	this->_selector = std::move(other._selector);
	return *this;
}

SocketIO::State SocketIO::read_line(std::string& line, size_t max_n)
{
	auto ret = this->_recv(std::min(max_n, MAX_BUFFER_SIZE));
	if (ret != State::Done)
	{
		return ret;
	}

	if (this->_buffer[0] == '\0')
	{
		line = "";
	}
	else
	{
		auto ptr = strstr(this->_buffer, "\r\n");
		if (!ptr)
		{
			line = this->_buffer;
			this->_buffer[0] = '\0';
		}
		else
		{
			size_t pos = ptr - this->_buffer + 2;       // 2 is the size of "\r\n" string
			line = std::string(this->_buffer, pos);
			this->_buffer_size -= pos;
			memmove(this->_buffer, &(this->_buffer[pos]), this->_buffer_size);
			this->_buffer[this->_buffer_size] = '\0';
		}
	}

	return State::Done;
}

SocketIO::State SocketIO::read_bytes(std::string& content, size_t n_bytes)
{
	if (this->_buffer[0] != '\0')
	{
		content += std::string(this->_buffer, this->_buffer_size);
		this->_buffer[0] = '\0';
	}

	SocketIO::State ret;
	while ((ret = this->_recv(std::min(n_bytes, MAX_BUFFER_SIZE))) == State::Done)
	{
		if (this->_buffer[0] != '\0')
		{
			content += std::string(this->_buffer, this->_buffer_size);
			this->_buffer[0] = '\0';
		}

		if (content.size() >= n_bytes)
		{
			break;
		}
	}

	return ret;
}

SocketIO::State SocketIO::write(const char* data, size_t n) const
{
	bool try_again;
	do
	{
		try_again = false;
		if (::send(this->_fd, data, n, MSG_NOSIGNAL) < 0)
		{
			switch (errno)
			{
				case ETIMEDOUT:
				case EAGAIN:
					try_again = true;
					break;
				case ECONNRESET:
				case ENOTCONN:
					return State::ConnectionBroken;
				default:
					return State::Failed;
			}
		}
	}
	while (try_again);
	return State::Done;
}

SocketIO::State SocketIO::_recv(size_t n)
{
	if (this->_buffer[0] != '\0')
	{
		return State::Done;
	}

	bool try_again;
	do
	{
		try_again = false;
		if (!this->_selector->select(this->_timeout.tv_sec, this->_timeout.tv_usec))
		{
			return State::TimedOut;
		}

		ssize_t len = recv(this->_fd, this->_buffer, n, 0);
		if (len > 0)
		{
			this->_buffer_size = len;
			this->_buffer[len] = '\0';
			return State::Done;
		}
		else if (len < 0)
		{
			switch (errno)
			{
				case ETIMEDOUT:
				case EAGAIN:
					try_again = true;
					break;
				case ECONNRESET:
				case ENOTCONN:
					return State::ConnectionBroken;
				default:
					return State::Failed;
			}
		}
	}
	while (try_again);
	return State::Done;
}

__SERVER_END__
