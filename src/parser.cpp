/**
 * core/parsers/request_parser.cpp
 *
 * Copyright (c) 2019-2020 Yuriy Lisovskiy
 */

#include "./parser.h"

// C++ libraries.
#include <cstring>

// Core libraries.
#include <xalwart.core/exceptions.h>

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif


__SERVER_INTERNAL_BEGIN__

// Parses http request headers from given stream.
void request_parser::parse_headers(const xw::string& data)
{
	std::string new_header_name;
	std::string new_header_value;
	std::map<std::string, std::string>::iterator connection_it;
	for (const auto& input : data)
	{
		switch (this->state)
		{
			case request_parser::state_enum::s_method_begin:
				if (!request_parser::is_char(input) || request_parser::is_control(input) || request_parser::is_special(input))
				{
					throw core::ParseError("unable to parse method type", _ERROR_DETAILS_);
				}
				else
				{
					this->state = request_parser::state_enum::s_method;
					this->method.push_back(input);
				}
				break;
			case request_parser::state_enum::s_method:
				if (input == ' ')
				{
					this->state = request_parser::state_enum::s_path_begin;
				}
				else if (!request_parser::is_char(input) || request_parser::is_control(input) || request_parser::is_special(input))
				{
					throw core::ParseError("unable to parse http method type", _ERROR_DETAILS_);
				}
				else
				{
					this->method.push_back(input);
				}
				break;
			case request_parser::state_enum::s_path_begin:
				if (request_parser::is_control(input))
				{
					throw core::ParseError("unable to parse http url path", _ERROR_DETAILS_);
				}
				else
				{
					this->state = request_parser::state_enum::s_path;
					this->path.push_back(input);
				}
				break;
			case request_parser::state_enum::s_path:
				if (input == ' ')
				{
					this->state = request_parser::state_enum::s_http_version_h;
				}
				else if (input == '?')
				{
					this->state = request_parser::state_enum::s_query;
				}
				else if (input == '\r')
				{
					this->major_v = 0;
					this->minor_v = 9;
					return;
				}
				else if (request_parser::is_control(input))
				{
					throw core::ParseError("unable to parse http url path", _ERROR_DETAILS_);
				}
				else
				{
					this->path.push_back(input);
				}
				break;
			case request_parser::state_enum::s_query:
				if (input == ' ')
				{
					this->state = request_parser::state_enum::s_http_version_h;
				}
				else if (input == '#')
				{
					this->state = request_parser::state_enum::s_fragment;
				}
				else if (input == '\r')
				{
					this->major_v = 0;
					this->minor_v = 9;

					return;
				}
				else if (request_parser::is_control(input))
				{
					throw core::ParseError("unable to parse http url query", _ERROR_DETAILS_);
				}
				else
				{
					this->query.push_back(input);
				}
				break;
			case request_parser::state_enum::s_fragment:
				if (input == ' ')
				{
					this->state = request_parser::state_enum::s_http_version_h;
				}
				// INFO: save fragment maybe.
				break;
			case request_parser::state_enum::s_http_version_h:
				this->parse_http_word(input, 'H', request_parser::state_enum::s_http_version_ht);
				break;
			case request_parser::state_enum::s_http_version_ht:
				this->parse_http_word(input, 'T', request_parser::state_enum::s_http_version_htt);
				break;
			case request_parser::state_enum::s_http_version_htt:
				this->parse_http_word(input, 'T', request_parser::state_enum::s_http_version_http);
				break;
			case request_parser::state_enum::s_http_version_http:
				this->parse_http_word(input, 'P', request_parser::state_enum::s_http_version_slash);
				break;
			case request_parser::state_enum::s_http_version_slash:
				if (input == '/')
				{
					this->major_v = 0;
					this->minor_v = 0;
					this->state = request_parser::state_enum::s_http_version_major_begin;
				}
				else
				{
					throw core::ParseError("unable to parse http protocol version", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_http_version_major_begin:
				if (request_parser::is_digit(input))
				{
					this->major_v = input - '0';
					this->state = request_parser::state_enum::s_http_version_major;
				}
				else
				{
					throw core::ParseError("unable to parse major part of http protocol version", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_http_version_major:
				if (input == '.')
				{
					this->state = request_parser::state_enum::s_http_version_minor_begin;
				}
				else if (request_parser::is_digit(input))
				{
					this->major_v = this->major_v * 10 + input - '0';
				}
				else
				{
					throw core::ParseError("unable to parse major part of http protocol version", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_http_version_minor_begin:
				if (request_parser::is_digit(input))
				{
					this->minor_v = input - '0';
					this->state = request_parser::state_enum::s_http_version_minor;
				}
				else
				{
					throw core::ParseError("unable to parse minor part of http protocol version", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_http_version_minor:
				if (input == '\r')
				{
					this->state = request_parser::state_enum::s_http_version_new_line;
				}
				else if(request_parser::is_digit(input))
				{
					this->minor_v = this->minor_v * 10 + input - '0';
				}
				else
				{
					throw core::ParseError("unable to parse minor part of http protocol version", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_http_version_new_line:
				if (input == '\n')
				{
					this->state = request_parser::state_enum::s_header_line_start;
				}
				else
				{
					throw core::ParseError("unable to parse http protocol main data", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_header_line_start:
				if (input == '\r')
				{
					this->state = request_parser::state_enum::s_expecting_new_line_3;
				}
				else if (!this->headers.empty() && (input == ' ' || input == '\t'))
				{
					this->state = request_parser::state_enum::s_header_lws;
				}
				else if(!request_parser::is_char(input) || request_parser::is_control(input) || request_parser::is_special(input))
				{
					throw core::ParseError("unable to parse http request header", _ERROR_DETAILS_);
				}
				else
				{
					new_header_name.reserve(16);
					new_header_value.reserve(16);
					new_header_name.push_back(input);
					this->state = request_parser::state_enum::s_header_name;
				}
				break;
			case request_parser::state_enum::s_header_lws:
				if(input == '\r')
				{
					this->state = request_parser::state_enum::s_expecting_new_line_2;
				}
				else if(input == ' ' || input == '\t')
				{
				}
				else if (request_parser::is_control(input))
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				else
				{
					this->state = request_parser::state_enum::s_header_value;
					new_header_value.push_back(input);
				}
				break;
			case request_parser::state_enum::s_header_name:
				if (input == ':')
				{
					this->state = request_parser::state_enum::s_header_space_before_value;
				}
				else if (!request_parser::is_char(input) || request_parser::is_control(input) || request_parser::is_special(input))
				{
					throw core::ParseError("unable to parse http request header name", _ERROR_DETAILS_);
				}
				else
				{
					new_header_name.push_back(input);
				}
				break;
			case request_parser::state_enum::s_header_space_before_value:
				if (input == ' ')
				{
					this->state = request_parser::state_enum::s_header_value;
				}
				else
				{
					throw core::ParseError("unable to parse http request header", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_header_value:
				if (input == '\r')
				{
					if (strcasecmp(new_header_name.c_str(), "Content-Length") == 0)
					{
						this->content_size = strtol(new_header_value.c_str(), nullptr, 0);
//						this->content.reserve(this->content_size);
					}
					else if (strcasecmp(new_header_name.c_str(), "Transfer-Encoding") == 0)
					{
						if (strcasecmp(new_header_value.c_str(), "chunked") == 0)
						{
							this->chunked = true;
						}
					}

					this->headers[new_header_name] = new_header_value;
					new_header_name.clear();
					new_header_value.clear();
					this->state = request_parser::state_enum::s_expecting_new_line_2;
				}
				else if (request_parser::is_control(input))
				{
					throw core::ParseError("unable to parse http request header value", _ERROR_DETAILS_);
				}
				else
				{
					new_header_value.push_back(input);
				}
				break;
			case request_parser::state_enum::s_expecting_new_line_2:
				if (input == '\n')
				{
					this->state = request_parser::state_enum::s_header_line_start;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_expecting_new_line_3:
				connection_it = std::find_if(
					this->headers.begin(),
					this->headers.end(),
					[](const std::pair<std::string, std::string>& item) -> bool
					{
						return strcasecmp(item.first.c_str(), "Connection") == 0;
					}
				);
				if (connection_it != this->headers.end())
				{
					this->keep_alive = strcasecmp((*connection_it).second.c_str(), "Keep-Alive") == 0;
				}
				else
				{
					if (this->major_v > 1 || (this->major_v == 1 && this->minor_v == 1))
					{
						this->keep_alive = true;
					}
				}

				if (this->chunked)
				{
					this->state = request_parser::state_enum::s_chunk_size;
				}
				else if (this->content_size == 0)
				{
					if (input != '\n')
					{
						throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
					}
				}
				else
				{
					auto contentType = this->headers["Content-Type"];
					if (contentType.find("multipart/form-data") != std::string::npos)
					{
						this->content_type = request_parser::content_type_enum::ct_multipart_form_data;
					}
					else if (contentType.find("application/json") != std::string::npos)
					{
						this->content_type = request_parser::content_type_enum::ct_application_json;
					}
					else if (contentType.find("application/x-www-form-urlencoded") != std::string::npos)
					{
						this->content_type = request_parser::content_type_enum::ct_application_x_www_form_url_encoded;
					}
					else
					{
						this->content_type = request_parser::content_type_enum::ct_other;
					}
					this->state = request_parser::state_enum::s_request_body;
				}
				return;
			default:
				throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
		}
	}

	throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
}

// Parses http request body from given stream.
void request_parser::parse_body(const xw::string& data, const xw::string& media_root)
{
	if (strlen(data.c_str()))
	{
		if (this->state == request_parser::state_enum::s_request_body)
		{
			this->content = data;
		}
		else
		{
			this->parse_chunks(data);
		}
	}
}

// Parses 'HTTP' word from http request's head line.
void request_parser::parse_http_word(char input, char expected, request_parser::state_enum new_state)
{
	if (input == expected)
	{
		this->state = new_state;
	}
	else
	{
		throw core::ParseError("unable to parse http protocol version", _ERROR_DETAILS_);
	}
}

// Parses chunks from http request body if request is chunked.
// TODO: check if parse_chunks parses chunked request properly.
void request_parser::parse_chunks(const xw::string& data)
{
	for (const auto& input : data)
	{
		switch (this->state)
		{
			case request_parser::state_enum::s_chunk_size:
				if (isalnum(input))
				{
					this->chunk_size_str.push_back(input);
				}
				else if (input == ';')
				{
					this->state = request_parser::state_enum::s_chunk_extension_name;
				}
				else if (input == '\r')
				{
					this->state = request_parser::state_enum::s_chunk_size_new_line;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_extension_name:
				if (isalnum(input) || input == ' ')
				{
					// skip
				}
				else if (input == '=')
				{
					this->state = request_parser::state_enum::s_chunk_extension_value;
				}
				else if (input == '\r')
				{
					this->state = request_parser::state_enum::s_chunk_size_new_line;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_extension_value:
				if (std::isalnum(input) || input == ' ')
				{
					// skip
				}
				else if (input == '\r')
				{
					this->state = request_parser::state_enum::s_chunk_size_new_line;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_size_new_line:
				if (input == '\n')
				{
					this->chunk_size = strtol(this->chunk_size_str.c_str(), nullptr, 16);
					this->chunk_size_str.clear();
//					this->content.reserve(this->content.size() + this->chunk_size);

					if (this->chunk_size == 0)
					{
						this->state = request_parser::state_enum::s_chunk_size_new_line_2;
					}
					else
					{
						this->state = request_parser::state_enum::s_chunk_data;
					}
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_size_new_line_2:
				if (input == '\r')
				{
					this->state = request_parser::state_enum::s_chunk_size_new_line_3;
				}
				else if( isalpha(input) )
				{
					this->state = request_parser::state_enum::s_chunk_trailer_name;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_size_new_line_3:
				if (input == '\n')
				{
					return;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
			case request_parser::state_enum::s_chunk_trailer_name:
				if (std::isalnum(input))
				{
					// skip
				}
				else if (input == ':')
				{
					this->state = request_parser::state_enum::s_chunk_trailer_value;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_trailer_value:
				if (std::isalnum(input) || input == ' ')
				{
					// skip
				}
				else if( input == '\r' )
				{
					this->state = request_parser::state_enum::s_chunk_size_new_line;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_data:
				this->content.push_back(input);
				if (--this->chunk_size == 0)
				{
					this->state = request_parser::state_enum::s_chunk_data_new_line_1;
				}
				break;
			case request_parser::state_enum::s_chunk_data_new_line_1:
				if (input == '\r')
				{
					this->state = request_parser::state_enum::s_chunk_data_new_line_2;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			case request_parser::state_enum::s_chunk_data_new_line_2:
				if (input == '\n')
				{
					this->state = request_parser::state_enum::s_chunk_size;
				}
				else
				{
					throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
				}
				break;
			default:
				throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
		}
	}

	throw core::ParseError("unable to parse http request", _ERROR_DETAILS_);
}

// Checks if a byte is an HTTP character.
bool request_parser::is_char(uint c)
{
	return c >= 0 && c <= 127;
}

// Checks if a byte is an HTTP control character.
bool request_parser::is_control(uint c)
{
	return (c >= 0 && c <= 31) || (c == 127);
}

// Checks if a byte is defined as an HTTP special character.
bool request_parser::is_special(uint c)
{
	switch (c)
	{
		case '(':
		case ')':
		case '<':
		case '>':
		case '@':
		case ',':
		case ';':
		case ':':
		case '\\':
		case '"':
		case '/':
		case '[':
		case ']':
		case '?':
		case '=':
		case '{':
		case '}':
		case ' ':
		case '\t':
			return true;
		default:
			return false;
	}
}

// Checks if a byte is a digit.
bool request_parser::is_digit(uint c)
{
	return c >= '0' && c <= '9';
}

__SERVER_INTERNAL_END__
