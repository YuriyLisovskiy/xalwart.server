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

// Base libraries.
#include <xalwart.base/net/_def_.h>

// Server libraries.
#include "../exceptions.h"


__SERVER_BEGIN__

SocketIO::SocketIO(Socket file_descriptor, timeval timeout, std::unique_ptr<ISelector> selector) :
	_file_descriptor(file_descriptor),
	_timeout(timeout),
	_selector(std::move(selector)),
	_limit(-1)
{
	this->_selector->register_read_event();
}

SocketIO& SocketIO::operator= (SocketIO&& other) noexcept
{
	this->_file_descriptor = other._file_descriptor;
	this->_timeout = other._timeout;
	this->_selector = std::move(other._selector);
	if (!this->buffer_is_empty())
	{
		this->_buffer = other._buffer;
	}

	this->_limit = other._limit;
	return *this;
}

ssize_t SocketIO::read_line(std::string& line)
{
	ssize_t total_bytes_read_count = 0;
	line.clear();
	bool new_line_reached = false;
	while (!new_line_reached)
	{
		bool is_read = true;
		if (this->read_allowed())
		{
			is_read = this->read_bytes(net::DEFAULT_BUFFER_SIZE);
		}

		if (!is_read)
		{
			break;
		}

		auto pos = this->_buffer.find("\r\n");
		if (pos == std::string::npos)
		{
			total_bytes_read_count += this->append_from_buffer_to(line, -1);
		}
		else
		{
			ssize_t count = (ssize_t)pos + 2;
			total_bytes_read_count += this->append_from_buffer_to(line, count);
			new_line_reached = true;
		}
	}

	return total_bytes_read_count;
}

ssize_t SocketIO::read(std::string& buffer, size_t max_count)
{
	buffer.clear();
	bool is_read = true;
	if (this->read_allowed())
	{
		is_read = this->read_bytes(max_count);
	}

	if (is_read)
	{
		return this->append_from_buffer_to(buffer, (ssize_t)max_count);
	}

	return 0;
}

ssize_t SocketIO::peek(std::string& buffer, size_t max_count)
{
	buffer.clear();
	bool is_read = true;
	if (max_count - this->buffered() > 0)
	{
		is_read = this->read_bytes(max_count);
	}

	if (is_read)
	{
		return this->append_from_buffer_to(buffer, max_count, false);
	}

	return 0;
}

ssize_t SocketIO::write(const char* data, size_t count)
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

ssize_t SocketIO::append_from_buffer_to(std::string& buffer, size_t max_count, bool erase)
{
	auto count = (max_count < this->buffered()) ? max_count : this->buffered();
	buffer += this->_buffer.substr(0, count);
	if (erase)
	{
		if (count < this->buffered())
		{
			this->_buffer = this->_buffer.substr(count);
		}
		else
		{
			this->clear_buffer();
		}
	}

	return (ssize_t)count;
}

bool SocketIO::read_bytes(size_t max_count)
{
	bool try_again;
	do
	{
		auto bytes_count = max_count;
		if (this->has_limit())
		{
			bytes_count = std::min((size_t)this->limit(), bytes_count);
		}

		bytes_count = std::min(net::DEFAULT_BUFFER_SIZE, bytes_count);
		if (bytes_count == 0)
		{
			throw EoF("end of socket stream", _ERROR_DETAILS_);
		}

		try_again = false;
		if (!this->_selector->select(this->_timeout.tv_sec, this->_timeout.tv_usec))
		{
			throw SocketError(ETIMEDOUT, "Connection timed out", _ERROR_DETAILS_);
		}

		char buf[net::DEFAULT_BUFFER_SIZE];
		auto len = ::read(this->file_descriptor(), buf, bytes_count);
		if (len > 0)
		{
			if (this->has_limit())
			{
				this->_limit -= len;
			}

			this->_buffer += std::string(buf, len);
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
