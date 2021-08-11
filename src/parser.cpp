/**
 * parser.cpp
 *
 * Copyright (c) 2019-2021 Yuriy Lisovskiy
 */

#include "./parser.h"

// Base libraries.
#include <xalwart.base/string_utils.h>
#include <xalwart.base/encoding.h>


__SERVER_PARSER_BEGIN__

inline const int MAX_LINE_LENGTH = 65536;
inline const int MAX_HEADERS_NUMBER = 100;

ParseHeadersStatus parse_headers(
	collections::Dictionary<std::string, std::string>& result, server::SocketIO* r_file
)
{
	while (true)
	{
		std::string line;
		auto r_status = r_file->read_line(line, MAX_LINE_LENGTH + 1);
		if (r_status != SocketIO::State::Done)
		{
			switch (r_status)
			{
				case SocketIO::State::TimedOut:
					return ParseHeadersStatus::TimedOut;
				case SocketIO::State::ConnectionBroken:
					return ParseHeadersStatus::ConnectionBroken;
				case SocketIO::State::Failed:
					return ParseHeadersStatus::Failed;
				default:
					break;
			}
		}

		if (line.size() > MAX_LINE_LENGTH)
		{
			return ParseHeadersStatus::LineTooLong;
		}

		line = str::rtrim(line, "\r\n");
		auto pair = str::split(encoding::encode_iso_8859_1(line, encoding::STRICT), ':', 1);
		if (result.size() > MAX_HEADERS_NUMBER)
		{
			return ParseHeadersStatus::MaxHeadersReached;
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
		result.set(pair[0], pair[1]);
	}

	return ParseHeadersStatus::Done;
}

__SERVER_PARSER_END__
