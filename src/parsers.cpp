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
	size_t total_bytes_read = 0;
	std::string line;
	while (reader->read_line(line))
	{
		total_bytes_read += line.size();
		if (total_bytes_read > max_request_size)
		{
			throw EntityTooLargeError(
				"Request entity exceeds " + std::to_string(max_request_size) + " bytes", _ERROR_DETAILS_
			);
		}

		if (line.size() > MAX_LINE_LENGTH)
		{
			throw LineTooLongError(
				"The header contains a value that exceeds " + std::to_string(MAX_LINE_LENGTH) + " bytes",
				_ERROR_DETAILS_
			);
		}

		line = str::rtrim(line, "\r\n");
		auto pair = str::split(encoding::encode_iso_8859_1(line, encoding::Mode::Strict), ':', 1);
		if (result.size() > MAX_HEADERS_NUMBER)
		{
			throw TooMuchHeadersError(
				"Headers count exceeds " + std::to_string(MAX_HEADERS_NUMBER), _ERROR_DETAILS_
			);
		}

		if (line.empty() || line == "\r\n" || line == "\n")
		{
			break;
		}

		if (pair.size() == 1)
		{
			pair.emplace_back("");
		}

		pair[1] = str::ltrim(pair[1]);
		result.insert(std::make_pair(pair[0], pair[1]));
	}

	return total_bytes_read;
}

__SERVER_PARSER_END__
