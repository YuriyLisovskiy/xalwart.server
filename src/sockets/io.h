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
#include <memory>

// Base libraries.
#include <xalwart.base/io.h>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "../abc.h"


__SERVER_BEGIN__

inline const size_t MAX_BUFFER_SIZE = 65535; // in bytes

// TODO: docs for 'SocketIO'
class SocketIO final : public io::IStream
{
public:
	explicit SocketIO(int fd, timeval timeout, std::unique_ptr<abc::ISelector> selector);

	SocketIO& operator= (SocketIO&& other) noexcept;

	ssize_t read_line(std::string& line) override;

	ssize_t read(std::string& buffer, size_t max_count) override;

	ssize_t write(const char* data, ssize_t count) override;

	bool close_reader() override;

	bool close_writer() override;

	[[nodiscard]]
	int shutdown(int how) const;

protected:
	[[nodiscard]]
	inline int file_descriptor() const
	{
		return this->_file_descriptor;
	}

	ssize_t append_from_buffer_to(std::string& buffer, ssize_t max_count=-1);

	bool read_bytes(size_t max_count);

	inline void clear_buffer()
	{
		this->_buffer[0] = '\0';
		this->_buffer_size = 0;
	}

	[[nodiscard]]
	inline bool buffer_is_empty() const
	{
		return this->_buffer[0] == '\0';
	}

private:
	Socket _file_descriptor;
	timeval _timeout;
	std::unique_ptr<abc::ISelector> _selector;
	char _buffer[MAX_BUFFER_SIZE]{};
	ssize_t _buffer_size;
};

__SERVER_END__
