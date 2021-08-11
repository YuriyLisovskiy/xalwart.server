/**
 * socket/io.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * SocketIO implements logic for socket fd.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../selectors.h"


#define MAX_BUFF_SIZE (size_t)65537 // bytes

__SERVER_BEGIN__

class SocketIO final
{
public:
	enum class State
	{
		Done,
		TimedOut,
		ConnectionBroken,
		Failed
//		Eof
	};

	explicit SocketIO(
		int fd, timeval timeout, std::shared_ptr<ISelector> selector
	);

	SocketIO& operator= (SocketIO&& other) noexcept;

	State read_line(std::string& line, size_t max_n=MAX_BUFF_SIZE);
	State read_bytes(std::string& content, size_t n);
	State write(const char* data, size_t n) const;

	[[nodiscard]]
	inline int shutdown(int how) const
	{
		return ::shutdown(this->_fd, how);
	}

private:
	State _recv(size_t n);

private:
	int _fd;
	timeval _timeout;
	char _buffer[MAX_BUFF_SIZE]{};
	ssize_t _buffer_size;
	std::shared_ptr<ISelector> _selector;
};

inline std::ostream& operator<< (std::ostream& os, SocketIO::State state)
{
	switch (state)
	{
		case SocketIO::State::Done:
			os << "SocketIO::State::Done";
			break;
		case SocketIO::State::TimedOut:
			os << "SocketIO::State::TimedOut";
			break;
		case SocketIO::State::ConnectionBroken:
			os << "SocketIO::State::ConnectionBroken";
			break;
		case SocketIO::State::Failed:
			os << "SocketIO::State::Failed";
			break;
//		case SocketIO::State::Eof:
//			os << "SocketIO::State::Eof";
//			break;
		default:
			os << "Unknown";
			break;
	}

	return os;
}

__SERVER_END__
