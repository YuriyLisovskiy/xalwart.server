/**
 * parser.cpp
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 */

#include "./parsers.h"

// Base libraries.
#include <xalwart.base/string_utils.h>
#include <xalwart.base/encoding.h>

// Server libraries.
#include "./exceptions.h"


__SERVER_PARSER_BEGIN__

size_t parse_headers(std::map<std::string, std::string>& result, io::IReader* reader, size_t max_request_size)
{
	const short HEADER_KEY = 0, HEADER_VALUE = 1;
	size_t total_bytes_read = 0;
	std::string header_line;
	while (reader->read_line(header_line))
	{
		total_bytes_read += header_line.size();
		if (total_bytes_read > max_request_size)
		{
			throw EntityTooLargeError(
				"Request entity exceeds " + std::to_string(max_request_size) + " bytes", _ERROR_DETAILS_
			);
		}

		if (header_line.size() > MAX_LINE_LENGTH)
		{
			throw LineTooLongError(
				"The header contains a value that exceeds " + std::to_string(MAX_LINE_LENGTH) + " bytes",
				_ERROR_DETAILS_
			);
		}

		header_line = str::rtrim(header_line, "\r\n");
		auto full_header = str::split(
			encoding::encode_iso_8859_1(header_line, encoding::Mode::Strict), ':', 1
		);
		if (result.size() > MAX_HEADERS_NUMBER)
		{
			throw TooMuchHeadersError(
				"Headers count exceeds " + std::to_string(MAX_HEADERS_NUMBER), _ERROR_DETAILS_
			);
		}

		if (header_line.empty() || header_line == "\r\n" || header_line == "\n")
		{
			break;
		}

		if (full_header.size() == 1)
		{
			full_header.emplace_back("");
		}

		full_header[HEADER_VALUE] = str::ltrim(full_header[HEADER_VALUE]);
		result.insert(std::make_pair(full_header[HEADER_KEY], full_header[HEADER_VALUE]));
	}

	return total_bytes_read;
}

__SERVER_PARSER_END__
