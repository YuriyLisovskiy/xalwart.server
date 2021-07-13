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

const int _MAX_LINE = 65536;
const int _MAX_HEADERS = 100;

parse_headers_status parse_headers(
	collections::Dict<std::string, std::string>& result, server::SocketIO* r_file
)
{
	while (true)
	{
		std::string line;
		auto r_status = r_file->read_line(line, _MAX_LINE + 1);
		if (r_status != SocketIO::s_done)
		{
			switch (r_status)
			{
				case SocketIO::s_timed_out:
					return ph_timed_out;
				case SocketIO::s_conn_broken:
					return ph_conn_broken;
				case SocketIO::s_failed:
					return ph_failed;
				default:
					break;
			}
		}

		if (line.size() > _MAX_LINE)
		{
			return ph_line_too_long;
		}

		line = str::rtrim(line, "\r\n");
		auto pair = str::split(
			encoding::encode_iso_8859_1(line, encoding::STRICT), ':', 1
		);
		if (result.size() > _MAX_HEADERS)
		{
			return ph_max_headers_reached;
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
		result[pair[0]] = pair[1];
	}

	return ph_done;
}

__SERVER_PARSER_END__
