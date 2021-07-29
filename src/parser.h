/**
 * parser.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * HTTP headers parser.
 */

#pragma once

// Base libraries.
#include <xalwart.base/collections/dictionary.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./socket/io.h"


__SERVER_PARSER_BEGIN__

enum parse_headers_status
{
	ph_done,
	ph_line_too_long,
	ph_max_headers_reached,
	ph_timed_out,
	ph_conn_broken,
	ph_failed
};

// Parses only RFC2822 headers from a file pointer.
extern parse_headers_status parse_headers(
	collections::Dictionary<std::string, std::string>& result, server::SocketIO* r_file
);

__SERVER_PARSER_END__
