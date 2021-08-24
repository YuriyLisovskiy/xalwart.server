/**
 * exceptions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * The collection of server errors.
 */

#pragma once

// Base libraries.
#include <xalwart.base/exceptions.h>

// Module definitions.
#include "./_def_.h"


__SERVER_BEGIN__

// TESTME: SocketError
// TODO: docs for 'SocketError'
class SocketError : public IOError
{
private:
	int _error_code;

protected:
	inline SocketError(
		int error_code, const char* message, int line, const char* function, const char* file, const char* type
	) : IOError(message, line, function, file, type), _error_code(error_code)
	{
	}

public:
	inline explicit SocketError(
		int error_code, const std::string& message, int line=0, const char* function="", const char* file=""
	) : SocketError(error_code, message.c_str(), line, function, file, "xw::server::SocketError")
	{
	}

	[[nodiscard]]
	inline int error_code() const
	{
		return this->_error_code;
	}
};

class ParseError : public ServerError
{
protected:
	inline ParseError(
		const char* message, int line, const char* function, const char* file, const char* type
	) : ServerError(message, line, function, file, type)
	{
	}

public:
	inline explicit ParseError(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : ParseError(message.c_str(), line, function, file, "xw::server::ParseError")
	{
	}
};

// TESTME: LineTooLongError
// TODO: docs for 'LineTooLongError'
class LineTooLongError : public ParseError
{
protected:
	inline LineTooLongError(
		const char* message, int line, const char* function, const char* file, const char* type
	) : ParseError(message, line, function, file, type)
	{
	}

public:
	inline explicit LineTooLongError(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : LineTooLongError(message.c_str(), line, function, file, "xw::server::LineTooLongError")
	{
	}
};

// TESTME: TooMuchHeadersError
// TODO: docs for 'TooMuchHeadersError'
class TooMuchHeadersError : public ParseError
{
protected:
	inline TooMuchHeadersError(
		const char* message, int line, const char* function, const char* file, const char* type
	) : ParseError(message, line, function, file, type)
	{
	}

public:
	inline explicit TooMuchHeadersError(
		const std::string& message, int line=0, const char* function="", const char* file=""
	) : TooMuchHeadersError(message.c_str(), line, function, file, "xw::server::TooMuchHeadersError")
	{
	}
};

__SERVER_END__
