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

ParseHeadersStatus parse_headers(std::map<std::string, std::string>& result, server::SocketIO* r_file)
{
	while (true)
	{
		std::string line;
		auto r_status = r_file->read_line(line, MAX_LINE_LENGTH + 1);
		if (r_status != net::SocketReaderState::Done)
		{
			switch (r_status)
			{
				case net::SocketReaderState::TimedOut:
					return ParseHeadersStatus::TimedOut;
				case net::SocketReaderState::ConnectionBroken:
					return ParseHeadersStatus::ConnectionBroken;
				case net::SocketReaderState::Failed:
					return ParseHeadersStatus::Failed;
				case net::SocketReaderState::Eof:
				default:
					break;
			}
		}

		if (line.size() > MAX_LINE_LENGTH)
		{
			return ParseHeadersStatus::LineTooLong;
		}

		line = str::rtrim(line, "\r\n");
		auto pair = str::split(encoding::encode_iso_8859_1(line, encoding::Mode::Strict), ':', 1);
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
		result.insert(std::make_pair(pair[0], pair[1]));
	}

	return ParseHeadersStatus::Done;
}

__SERVER_PARSER_END__
