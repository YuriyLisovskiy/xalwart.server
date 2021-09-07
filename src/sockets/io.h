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

// TODO: docs for 'SocketIO'
class SocketIO final : public io::ILimitedBufferedStream
{
public:
	explicit SocketIO(int fd, timeval timeout, std::unique_ptr<abc::ISelector> selector);

	SocketIO& operator= (SocketIO&& other) noexcept;

	ssize_t read_line(std::string& line) override;

	ssize_t read(std::string& buffer, size_t max_count) override;

	ssize_t peek(std::string& buffer, size_t max_count) override;

	ssize_t write(const char* data, size_t count) override;

	[[nodiscard]]
	inline ssize_t buffered() const override
	{
		return (ssize_t)this->_buffer.size();
	}

	bool close_reader() override;

	bool close_writer() override;

	inline void set_limit(ssize_t limit) override
	{
		this->_limit = limit - (ssize_t)this->_buffer.size();
	}

	[[nodiscard]]
	inline ssize_t limit() const override
	{
		return this->_limit;
	}

	[[nodiscard]]
	int shutdown(int how) const;

protected:
	[[nodiscard]]
	inline int file_descriptor() const
	{
		return this->_file_descriptor;
	}

	ssize_t append_from_buffer_to(std::string& buffer, size_t max_count, bool erase=true);

	bool read_bytes(size_t max_count);

	inline void clear_buffer()
	{
		this->_buffer.clear();
	}

	[[nodiscard]]
	inline bool buffer_is_empty() const
	{
		return this->_buffer.empty();
	}

	[[nodiscard]]
	inline bool read_allowed() const
	{
		return this->buffer_is_empty();
	}

	[[nodiscard]]
	inline bool has_limit() const
	{
		return this->limit() >= 0;
	}

private:
	Socket _file_descriptor;
	timeval _timeout;
	std::unique_ptr<abc::ISelector> _selector;
	std::string _buffer;
	ssize_t _limit;
};

__SERVER_END__
