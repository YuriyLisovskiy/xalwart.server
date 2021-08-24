/**
 * socket/io.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./io.h"

// C++ libraries.
#include <cstring>

// Server libraries.
#include "../exceptions.h"
#include "../selectors.h"


__SERVER_BEGIN__

bool SocketIO::read_bytes(size_t max_count)
{
	if (!this->buffer_is_empty())
	{
		return true;
	}

	bool try_again;
	do
	{
		try_again = false;
		if (!this->_selector->select(this->_timeout.tv_sec, this->_timeout.tv_usec))
		{
			throw SocketError(ETIMEDOUT, "Connection timed out", _ERROR_DETAILS_);
		}

		auto count = std::min(max_count, MAX_BUFFER_SIZE);
		ssize_t len = recv(this->file_descriptor(), this->_buffer, count, 0);
		if (len > 0)
		{
			this->_buffer_size = len;
			this->_buffer[len] = '\0';
			this->_has_bytes_to_read = len >= count;
			return true;
		}
		else if (len < 0)
		{
			auto error_code = errno;
			switch (error_code)
			{
				case ETIMEDOUT:
				case EAGAIN:
					try_again = true;
					break;
				case ECONNRESET:
					throw SocketError(error_code, "Connection reset by peer", _ERROR_DETAILS_);
				case ENOTCONN:
					throw SocketError(error_code, "Transport endpoint is not connected", _ERROR_DETAILS_);
				default:
					throw SocketError(error_code, "Connection filed", _ERROR_DETAILS_);
			}
		}
	}
	while (try_again);
	this->_has_bytes_to_read = false;
	return this->_has_bytes_to_read;
}

SocketIO::SocketIO(int fd, timeval timeout, std::unique_ptr<abc::ISelector> selector) :
	_fd(fd), _timeout(timeout), _selector(std::move(selector)), _buffer_size(-1), _has_bytes_to_read(true)
{
	this->_buffer[0] = '\0';
	this->_selector->register_(fd, EVENT_READ);
}

SocketIO& SocketIO::operator= (SocketIO&& other) noexcept
{
	this->_fd = other._fd;
	this->_timeout = other._timeout;
	this->_selector = std::move(other._selector);
	if (!this->buffer_is_empty())
	{
		strcpy(this->_buffer, other._buffer);
	}

	this->_buffer_size = other._buffer_size;
	this->_has_bytes_to_read = other._has_bytes_to_read;
	return *this;
}

bool SocketIO::read_line(std::string& line)
{
	line.clear();
	bool new_line_reached = false;
	while (!new_line_reached && this->read_bytes(MAX_BUFFER_SIZE))
	{
		auto ptr = strstr(this->_buffer, "\r\n");
		if (!ptr)
		{
			this->append_buffer_to(line);
			this->clear_buffer();
		}
		else
		{
			ssize_t count = ptr - this->_buffer + 2; // 2 is the size of "\r\n" string
			this->append_buffer_to(line, count);
			this->_buffer_size -= count;
			memmove(this->_buffer, &(this->_buffer[count]), this->_buffer_size);
			this->_buffer[this->_buffer_size] = '\0';
			new_line_reached = true;
		}
	}

	return !this->buffer_is_empty() || this->_has_bytes_to_read;
}

bool SocketIO::read(std::string& content, size_t max_count)
{
	content.clear();
	while (this->read_bytes(max_count))
	{
		if (!this->buffer_is_empty())
		{
			this->append_buffer_to(content, this->_buffer_size);
			this->clear_buffer();
		}

		if (content.size() >= max_count)
		{
			break;
		}
	}

	return false;
}

bool SocketIO::write(const char* data, ssize_t count)
{
	bool try_again;
	do
	{
		try_again = false;
		if (::send(this->file_descriptor(), data, count, MSG_NOSIGNAL) < 0)
		{
			auto error_code = errno;
			switch (error_code)
			{
				case ETIMEDOUT:
				case EAGAIN:
					try_again = true;
					break;
				case ECONNRESET:
					throw SocketError(error_code, "Connection reset by peer", _ERROR_DETAILS_);
				case ENOTCONN:
					throw SocketError(error_code, "Transport endpoint is not connected", _ERROR_DETAILS_);
				default:
					throw SocketError(error_code, "Connection filed", _ERROR_DETAILS_);
			}
		}
	}
	while (try_again);
	return true;
}

bool SocketIO::close_reader()
{
	return this->shutdown(SHUT_RD) == 0;
}

bool SocketIO::close_writer()
{
	return this->shutdown(SHUT_WR) == 0;
}

int SocketIO::shutdown(int how) const
{
	return ::shutdown(this->file_descriptor(), how);
}

__SERVER_END__
