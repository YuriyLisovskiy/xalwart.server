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

inline const size_t MAX_BUFFER_SIZE = 65537; // in bytes

// TODO: docs for 'SocketIO'
class SocketIO final : public io::IReader, public io::IWriter
{
private:
	int _fd;
	timeval _timeout;
	std::unique_ptr<abc::ISelector> _selector;
	char _buffer[MAX_BUFFER_SIZE]{};
	ssize_t _buffer_size;
	bool _has_bytes_to_read;

protected:
	[[nodiscard]]
	inline int file_descriptor() const
	{
		return this->_fd;
	}

	inline void append_buffer_to(std::string& destination, ssize_t max_count=-1) const
	{
		auto count = (max_count >= 0 && max_count < this->_buffer_size) ? max_count : this->_buffer_size;
		destination += std::string(this->_buffer, count);
	}

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

public:
	explicit SocketIO(int fd, timeval timeout, std::unique_ptr<abc::ISelector> selector);

	SocketIO& operator= (SocketIO&& other) noexcept;

	bool read_line(std::string& line) override;

	bool read(std::string& content, size_t max_count) override;

	bool write(const char* data, ssize_t count) override;

	bool close_reader() override;

	bool close_writer() override;

	[[nodiscard]]
	int shutdown(int how) const;
};

__SERVER_END__
