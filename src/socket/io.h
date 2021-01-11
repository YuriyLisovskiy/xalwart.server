/**
 * socket/io.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>

// Core libraries.
#include <xalwart.core/str.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../selectors.h"


#define MAX_BUFF_SIZE 65537 // bytes

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
	};

	explicit SocketIO(
		int fd, timeval timeout, std::shared_ptr<ISelector> selector
	);

	SocketIO& operator= (SocketIO&& other) noexcept;

	state read_line(xw::string& line, int max_n=MAX_BUFF_SIZE);
	state write(const char* data, size_t n) const;

	[[nodiscard]]
	int fd() const;

private:
	state _recv(int n);

private:
	int _fd;
	timeval _timeout;
	char _buffer[MAX_BUFF_SIZE]{};
	size_t _buffer_size;
	std::shared_ptr<ISelector> _selector;
};

__SERVER_END__
