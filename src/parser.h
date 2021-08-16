/**
 * parser.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * HTTP headers parser.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./sockets/io.h"


__SERVER_PARSER_BEGIN__

enum class ParseHeadersStatus
{
	Done,
	LineTooLong,
	MaxHeadersReached,
	TimedOut,
	ConnectionBroken,
	Failed
};

// Parses only RFC2822 headers from a file pointer.
extern ParseHeadersStatus parse_headers(
	std::map<std::string, std::string>& result, const std::unique_ptr<server::SocketIO>& r_file
);

__SERVER_PARSER_END__
