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
	enum state
	{
		s_done,
		s_timed_out,
		s_conn_broken,
		s_failed
//		s_eof
	};

	explicit SocketIO(
		int fd, timeval timeout, std::shared_ptr<ISelector> selector
	);

	SocketIO& operator= (SocketIO&& other) noexcept;

	state read_line(std::string& line, size_t max_n=MAX_BUFF_SIZE);
	state read_bytes(std::string& content, size_t n);
	state write(const char* data, size_t n) const;

	[[nodiscard]]
	inline int shutdown(int how) const
	{
		return ::shutdown(this->_fd, how);
	}

private:
	state _recv(size_t n);

private:
	int _fd;
	timeval _timeout;
	char _buffer[MAX_BUFF_SIZE]{};
	ssize_t _buffer_size;
	std::shared_ptr<ISelector> _selector;
};

inline std::ostream& operator<< (std::ostream& os, SocketIO::state st)
{
	switch (st)
	{
		case SocketIO::s_done:
			os << "s_done";
			break;
		case SocketIO::s_timed_out:
			os << "s_timed_out";
			break;
		case SocketIO::s_conn_broken:
			os << "s_conn_broken";
			break;
		case SocketIO::s_failed:
			os << "s_failed";
			break;
//		case SocketIO::s_eof:
//			os << "s_eof";
//			break;
		default:
			os << "s_unknown";
			break;
	}

	return os;
}

__SERVER_END__
