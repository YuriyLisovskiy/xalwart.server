/**
 * context.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./context.h"

// Base libraries.
#include <xalwart.base/exceptions.h>

// Server libraries.
#include "./selectors.h"
#include "./sockets/io.h"
#include "./handlers/http_handler.h"


__SERVER_BEGIN__

void Context::set_defaults()
{
	if (!this->create_selector)
	{
		this->create_selector = [](const Context& context, Socket socket) -> std::unique_ptr<abc::ISelector> {
			return std::make_unique<Selector>(socket, context.logger);
		};
	}

	if (!this->create_request_handler)
	{
		this->create_request_handler = [](
			const Context& context,
			std::unique_ptr<io::ILimitedBufferedStream> stream,
			const std::map<std::string, std::string>& environment
		) -> std::unique_ptr<abc::IRequestHandler> {
			require_non_null(stream.get(), "'stream' is nullptr", _ERROR_DETAILS_);
			return std::make_unique<HTTPRequestHandler>(
				std::move(stream), v::version.to_string(),
				context.max_header_length, context.max_headers_count,
				context.logger, environment, context.handler
			);
		};
	}

	if (!this->create_stream)
	{
		this->create_stream = [](const Context& context, Socket socket) -> std::unique_ptr<io::ILimitedBufferedStream> {
			struct timeval timeout{
				.tv_sec = context.timeout_seconds,
				.tv_usec = context.timeout_microseconds
			};
			return std::make_unique<SocketIO>(socket, timeout, context.create_selector(context, socket));
		};
	}
}

void Context::validate() const
{
	require_non_null(this->logger, "'logger' is nullptr", _ERROR_DETAILS_);
	require_non_null(this->timezone.get(), "'timezone' is nullptr", _ERROR_DETAILS_);
	require_non_null(this->worker.get(), "'worker' is nullptr", _ERROR_DETAILS_);
	if (!this->handler)
	{
		throw NullPointerException("'handler' function is nullptr", _ERROR_DETAILS_);
	}

	if (!this->create_selector)
	{
		throw NullPointerException("'create_selector' function is nullptr", _ERROR_DETAILS_);
	}

	if (!this->create_request_handler)
	{
		throw NullPointerException("'create_request_handler' function is nullptr", _ERROR_DETAILS_);
	}

	if (!this->create_stream)
	{
		throw NullPointerException("'create_stream' function is nullptr", _ERROR_DETAILS_);
	}
}

__SERVER_END__
