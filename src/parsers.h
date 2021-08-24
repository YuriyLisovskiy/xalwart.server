/**
 * parser.h
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 *
 * HTTP headers parser.
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>

// Base libraries.
#include <xalwart.base/io.h>

// Module definitions.
#include "./_def_.h"


__SERVER_PARSER_BEGIN__

/** Parses only RFC2822 headers from a stream.
 *
 * \param result contains collected header values
 * \param reader stream to read from
 * \param max_request_size maximum bytes count before throwing `EntityTooLargeError`
 *
 * \throw `LineTooLongError` when header value contains more than 65536 bytes,
 * `TooMuchHeadersError` when headers count is greater than 100,
 * `EntityTooLargeError` when read bytes exceeds `max_request_size`.
 *
 * \return total bytes read count.
 */
extern size_t parse_headers(std::map<std::string, std::string>& result, io::IReader* reader, size_t max_request_size);

__SERVER_PARSER_END__
