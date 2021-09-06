/**
 * socket/io.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./io.h"

// C++ libraries.
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

// Server libraries.
#include "../exceptions.h"


__SERVER_BEGIN__

SocketIO::SocketIO(Socket file_descriptor, timeval timeout, std::unique_ptr<abc::ISelector> selector) :
	_file_descriptor(file_descriptor),
	_timeout(timeout),
	_selector(std::move(selector)),
	_buffer_size(-1)
{
	this->_buffer[0] = '\0';
	this->_selector->register_read_event();
}

SocketIO& SocketIO::operator= (SocketIO&& other) noexcept
{
	this->_file_descriptor = other._file_descriptor;
	this->_timeout = other._timeout;
	this->_selector = std::move(other._selector);
	if (!this->buffer_is_empty())
	{
		strcpy(this->_buffer, other._buffer);
	}

	this->_buffer_size = other._buffer_size;
	return *this;
}

ssize_t SocketIO::read_line(std::string& line)
{
	ssize_t total_bytes_read_count = 0;
	line.clear();
	bool new_line_reached = false;
	while (!new_line_reached && this->read_bytes(MAX_BUFFER_SIZE))
	{
		auto ptr = strstr(this->_buffer, "\r\n");
		if (!ptr)
		{
			total_bytes_read_count += this->append_from_buffer_to(line);
		}
		else
		{
			ssize_t count = ptr - this->_buffer + 2; // 2 is the size of "\r\n" string
			total_bytes_read_count += this->append_from_buffer_to(line, count);
			new_line_reached = true;
		}
	}

	return total_bytes_read_count;
}

ssize_t SocketIO::read(std::string& buffer, size_t max_count)
{
	buffer.clear();
	if (this->read_bytes(max_count))
	{
		return this->append_from_buffer_to(buffer, (ssize_t)max_count);
	}

	return 0;
}

ssize_t SocketIO::peek(std::string& buffer, ssize_t max_count)
{
	buffer.clear();
	if (this->read_bytes(max_count))
	{
		return this->append_from_buffer_to(buffer, (ssize_t)max_count, false);
	}

	return 0;
}

ssize_t SocketIO::write(const char* data, ssize_t count)
{
	ssize_t bytes_sent_count;
	bool try_again;
	do
	{
		try_again = false;
		bytes_sent_count = ::send(this->file_descriptor(), data, count, MSG_NOSIGNAL);
		if (bytes_sent_count < 0)
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
	return bytes_sent_count;
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

ssize_t SocketIO::append_from_buffer_to(std::string& buffer, ssize_t max_count, bool erase)
{
	auto count = (max_count >= 0 && max_count < this->_buffer_size) ? max_count : this->_buffer_size;
	buffer += std::string(this->_buffer, count);
	if (erase)
	{
		if (count < this->_buffer_size)
		{
			this->_buffer_size -= count;
			memmove(this->_buffer, &(this->_buffer[count]), this->_buffer_size);
			this->_buffer[this->_buffer_size] = '\0';
		}
		else
		{
			this->clear_buffer();
		}
	}

	return count;
}

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

		auto bytes_count = std::min(max_count, MAX_BUFFER_SIZE);
		auto len = ::read(this->file_descriptor(), this->_buffer, bytes_count);
		if (len > 0)
		{
			this->_buffer_size = len;
			this->_buffer[len] = '\0';
			return true;
		}
		else if (len == 0)
		{
			// EOF
			return false;
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
	return false;
}

__SERVER_END__
