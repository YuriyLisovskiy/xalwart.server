/**
 * tcp_server.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include "./server.h"

// C++ libraries.
#include <iostream>


__SERVER_BEGIN__

HTTPServer::HTTPServer(
	const Context& ctx, HandlerFunc handler
) : BaseSocket(ctx.on_error, TCP, -1), _ctx(ctx), _handler(std::move(handler))
{
	int opt = 1;
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(int));
	this->_ctx.normalize();
	this->_threadPool = std::make_shared<core::internal::ThreadPool>(this->_ctx.threads_count);
	if (!this->_handler)
	{
		this->_handler = [](const int, internal::request_parser*, core::Error*)
		{
			std::cerr << "request handler is not specified";
		};
	}
}

bool HTTPServer::bind(uint16_t port, bool useIPv6)
{
	return useIPv6 ? this->_bind6(port) : this->_bind(port);
}

bool HTTPServer::bind(const char* host, uint16_t port, bool useIPv6)
{
	return useIPv6 ? this->_bind6(host, port) : this->_bind(host, port);
}

bool HTTPServer::listen(const std::string& startupMessage)
{
	if (::listen(this->sock, SOMAXCONN) < 0)
	{
		this->_ctx.on_error(errno, "server can't listen the socket");
		return false;
	}

	if (!startupMessage.empty())
	{
		std::cout << startupMessage;
		std::cout.flush();
	}

	return _accept(this);
//	std::thread t(_accept, this);
//	t.detach();
//	return true;
}

void HTTPServer::close()
{
	this->_threadPool->wait();
	::shutdown(this->sock, SHUT_RDWR);
	BaseSocket::close();
}

core::Error HTTPServer::send(int sock, const char* data)
{
	if (::send(sock, data, std::strlen(data), 0) < 0)
	{
		return core::Error(
			core::HttpError, "failed to send bytes to socket connection", _ERROR_DETAILS_
		);
	}

	return core::Error::none();
}

core::Error HTTPServer::write(int sock, const char* data, size_t bytes_to_write)
{
	if (::write(sock, data, bytes_to_write) < 0)
	{
		return core::Error(
			core::HttpError, "failed to send bytes to socket connection", _ERROR_DETAILS_
		);
	}

	return core::Error::none();
}

bool HTTPServer::_bind(uint16_t port)
{
	return this->_bind("0.0.0.0", port);
}

bool HTTPServer::_bind(const char* address, uint16_t port)
{
	this->use_ipv6 = false;
	if (inet_pton(AF_INET, address, &this->addr.sin_addr) <= 0)
	{
		this->_ctx.on_error(errno, "invalid address, address type is not supported");
		return false;
	}

	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);

	if (::bind(this->sock, (const sockaddr *)&this->addr, sizeof(this->addr)) < 0)
	{
		this->_ctx.on_error(errno, "cannot bind the socket");
		return false;
	}

	return true;
}

bool HTTPServer::_bind6(uint16_t port)
{
	this->use_ipv6 = true;
	return this->_bind6("::1", port);
}

bool HTTPServer::_bind6(const char* address, uint16_t port)
{
	this->use_ipv6 = true;
	if (inet_pton(AF_INET6, address, &this->addr6.sin6_addr) <= 0)
	{
		this->_ctx.on_error(errno, "invalid address, address type not supported");
		return false;
	}

	this->addr6.sin6_family = AF_INET6;
	this->addr6.sin6_port = htons(port);

	if (::bind(this->sock, (const sockaddr *)&this->addr6, sizeof(this->addr6)) < 0)
	{
		this->_ctx.on_error(errno, "cannot bind the socket");
		return false;
	}

	return true;
}

bool HTTPServer::_accept(HTTPServer* s)
{
	if (s->use_ipv6)
	{
		sockaddr_in6 newSocketInfo{};
		socklen_t newSocketInfoLength = sizeof(newSocketInfo);

		int newSock;
		while (!s->is_closed)
		{
			while ((newSock = ::accept(s->sock, (sockaddr *)&newSocketInfo, &newSocketInfoLength)) < 0)
			{
				if (errno == EBADF || errno == EINVAL)
				{
					return false;
				}

				s->_ctx.on_error(errno, "error while accepting a new connection");
				return false;
			}

			if (!s->is_closed && newSock >= 0)
			{
				s->_handleConnection(newSock);
			}
		}
	}
	else
	{
		sockaddr_in newSocketInfo{};
		socklen_t newSocketInfoLength = sizeof(newSocketInfo);

		int newSock;
		while (!s->is_closed)
		{
			while ((newSock = ::accept(s->sock, nullptr, nullptr)) < 0)
			{
				if (errno == EBADF || errno == EINVAL)
				{
					return false;
				}

				s->_ctx.on_error(errno, "error while accepting a new connection");
				return false;
			}

			if (!s->is_closed && newSock >= 0)
			{
				s->_handleConnection(newSock);
			}
		}
	}

	return true;
}

void HTTPServer::_handleConnection(const int& sock)
{
	this->_threadPool->push([this, sock](){
		internal::request_parser rp;
		xw::string body_beginning;
		auto result = _read_headers(sock, body_beginning);
		if (result.err)
		{
			this->_handler(sock, &rp, &result.err);
		}
		else
		{
			rp.parse_headers(result.value);
			if (rp.headers.find("Content-Length") != rp.headers.end())
			{
				size_t body_length = std::strtol(rp.headers["Content-Length"].c_str(), nullptr, 10);
				xw::string body;
				if (body_length == body_beginning.size())
				{
					body = body_beginning;
				}
				else
				{
					result = _read_body(sock, body_beginning, body_length);
					if (result.err)
					{
						this->_handler(sock, &rp, &result.err);
//						return result.forward<std::shared_ptr<http::HttpRequest>>();
					}
					else
					{
						body = result.value;
					}
				}

				rp.parse_body(body, this->_ctx.media_root);
			}

			this->_handler(sock, &rp, nullptr);
		}
	});
}

int HTTPServer::_error()
{
	switch (errno)
	{
		case EBADF:
		case EFAULT:
		case EINVAL:
		case ENXIO:
			// Fatal error.
			return errno - 100;
		case EIO:
		case ENOBUFS:
		case ENOMEM:
			// Resource acquisition failure or device error.
			return errno - 101;
		case EINTR:
			// TODO: Check for user interrupt flags.
		case ETIMEDOUT:
		case EAGAIN:
			// Temporary error.
			return read_result_enum::rr_continue;
		case ECONNRESET:
		case ENOTCONN:
			// Connection broken.
			// Return the data we have available and exit
			// as if the connection was closed correctly.
			return read_result_enum::rr_break;
		default:
			return errno;
	}
}

core::Result<xw::string> HTTPServer::_read_headers(
	const int& sock, xw::string& body_beginning
)
{
	unsigned long size = 0;
	xw::string data;
	size_t headers_delimiter_pos = std::string::npos;
	std::string delimiter = "\r\n\r\n";

	char buffer[MAX_BUFF_SIZE];
	long long message_len;
	do
	{
		message_len = recv(sock, buffer, MAX_BUFF_SIZE, 0);
		buffer[message_len] = '\0';
		if (message_len > 0)
		{
			data.append(buffer);
			size += message_len;

			// Maybe it is better to check each header value's size.
			if (size > MAX_HEADERS_SIZE)
			{
				return core::raise<core::EntityTooLargeError, xw::string>(
					"Request data is too big", _ERROR_DETAILS_
				);
			}
		}
		else if (message_len == -1)
		{
			auto status = _error();
			if (status == read_result_enum::rr_continue)
			{
				continue;
			}
			else if (status == read_result_enum::rr_break)
			{
				break;
			}
			else
			{
				return core::raise<core::HttpError, xw::string>(
					"request finished with error code " + std::to_string(status), _ERROR_DETAILS_
				);
			}
		}

		headers_delimiter_pos = data.find(delimiter);
	}
	while (headers_delimiter_pos == std::string::npos);

	if (headers_delimiter_pos == std::string::npos)
	{
		return core::raise<core::HttpError, xw::string>(
			"invalid http request has been received", _ERROR_DETAILS_
		);
	}

	headers_delimiter_pos += delimiter.size();
	body_beginning = data.substr(headers_delimiter_pos);
	return core::Result(data.substr(0, headers_delimiter_pos));
}

core::Result<xw::string> HTTPServer::_read_body(
	const int& sock, const xw::string& body_beginning, size_t body_length
)
{
	xw::string data;
	if (body_length <= 0)
	{
		return core::Result(data);
	}

	size_t size = body_beginning.size();
	if (size == body_length)
	{
		return core::Result(body_beginning);
	}

	long long message_len;
	const size_t buff_size = MAX_BUFF_SIZE < body_length ? MAX_BUFF_SIZE : body_length;
	char buffer[MAX_BUFF_SIZE];
	while (size < body_length)
	{
		message_len = recv(sock, buffer, buff_size, 0);
		buffer[message_len] = '\0';
		if (message_len > 0)
		{
			data.append(buffer);
			size += message_len;

			// Maybe it is better to check each header value's size.
			if (size > MAX_HEADERS_SIZE)
			{
				return core::raise<core::EntityTooLargeError, xw::string>(
					"Request data is too big", _ERROR_DETAILS_
				);
			}
		}
		else if (message_len == -1)
		{
			auto status = _error();
			if (status == read_result_enum::rr_continue)
			{
				continue;
			}
			else if (status == read_result_enum::rr_break)
			{
				break;
			}
			else
			{
				return core::raise<core::HttpError, xw::string>(
					"request finished with error code " + std::to_string(status), _ERROR_DETAILS_
				);
			}
		}
	}

	data = body_beginning + data;
	if (data.size() != body_length)
	{
		return core::raise<core::HttpError, xw::string>(
			"actual body size is not equal to header's value", _ERROR_DETAILS_
		);
	}

	return core::Result(data);
}

__SERVER_END__
