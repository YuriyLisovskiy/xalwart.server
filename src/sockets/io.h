/**
 * socket/io.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * `SocketIO` implements logic for socket file descriptor.
 */

#pragma once

// C++ libraries.
#include <string>

// Base libraries.
#include <xalwart.base/io.h>
#include <xalwart.base/net/request_context.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../selectors.h"


__SERVER_BEGIN__

inline const size_t MAX_BUFFER_SIZE = 65537; // in bytes

// TODO: docs for 'SocketIO'
class SocketIO final : public io::IReader
{
private:
	int _fd;
	timeval _timeout;
	char _buffer[MAX_BUFFER_SIZE]{};
	ssize_t _buffer_size;
	std::unique_ptr<ISelector> _selector;

public:
	inline explicit SocketIO(int fd, timeval timeout, std::unique_ptr<ISelector> selector) :
		_fd(fd), _timeout(timeout), _selector(std::move(selector)), _buffer_size(-1)
	{
		this->_buffer[0] = '\0';
		this->_selector->register_(fd, EVENT_READ);
	}

	SocketIO& operator= (SocketIO&& other) noexcept;

	int read_line(std::string& line, size_t max_n) override;

	int read(std::string& content, size_t n) override;

	net::SocketReaderState write(const char* data, size_t n) const;

	[[nodiscard]]
	inline int shutdown(int how) const
	{
		return ::shutdown(this->_fd, how);
	}

private:
	net::SocketReaderState _recv(size_t n);
};

// TODO: docs for 'operator<<'
inline std::ostream& operator<< (std::ostream& os, net::SocketReaderState state)
{
	switch (state)
	{
		case net::SocketReaderState::Done:
			os << "xw::net::SocketReaderState::Done";
			break;
		case net::SocketReaderState::TimedOut:
			os << "xw::net::SocketReaderState::TimedOut";
			break;
		case net::SocketReaderState::ConnectionBroken:
			os << "xw::net::SocketReaderState::ConnectionBroken";
			break;
		case net::SocketReaderState::Failed:
			os << "xw::net::SocketReaderState::Failed";
			break;
		case net::SocketReaderState::Eof:
			os << "xw::net::SocketReaderState::Eof";
			break;
		default:
			os << "Unknown";
			break;
	}

	return os;
}

__SERVER_END__
