/**
 * context.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Context for HTTP server.
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <memory>
#include <functional>

// Base libraries.
#include <xalwart.base/exceptions.h>
#include <xalwart.base/interfaces/base.h>
#include <xalwart.base/datetime.h>
#include <xalwart.base/utility.h>
#include <xalwart.base/net/request_context.h>
#include <xalwart.base/workers/abstract_worker.h>

// Module definitions.
#include "./_def_.h"

// Server libraries.
#include "./interfaces.h"


__SERVER_BEGIN__

// TESTME: Context
// TODO: docs for 'Context'
struct Context final
{
	xw::ILogger* logger = nullptr;
	std::shared_ptr<dt::Timezone> timezone = std::make_shared<dt::Timezone>(dt::Timezone::UTC);
	size_t max_headers_count = 100;
	size_t max_header_length = 65535;
	time_t timeout_seconds = 5;
	time_t timeout_microseconds = 0;
	size_t socket_creation_retries_count = 5;
	std::unique_ptr<AbstractWorker> worker = nullptr;

	std::function<net::StatusCode(
		net::RequestContext* /* context */, const std::map<std::string, std::string>& /* environment */
	)> handler = nullptr;

	std::function<std::unique_ptr<ISelector>(const Context& context, Socket)> create_selector = nullptr;

	std::function<std::unique_ptr<IRequestHandler>(
		const Context&,
		std::unique_ptr<io::ILimitedBufferedStream>,
		const std::map<std::string, std::string>& /* environment */
	)> create_request_handler;

	std::function<std::unique_ptr<io::ILimitedBufferedStream>(const Context& context, Socket)> create_stream = nullptr;

	void set_defaults();

	void validate() const;
};

__SERVER_END__
