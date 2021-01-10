/**
 * server.cpp
 *
 * Copyright (c) 2020-2021 Yuriy Lisovskiy
 */

#include "./server.h"

// Core libraries.
#include <xalwart.core/datetime.h>
#include <xalwart.core/string_utils.h>

// Framework libraries.
#include "../../http/meta.h"
#include "../parsers/query_parser.h"
#include "../parsers/multipart_parser.h"


__SERVER_BEGIN__

DefaultServer::DefaultServer(
	const Context& ctx, HttpHandlerFunc handler, const conf::Settings* settings
) : HTTPServer(ctx, this->_make_handler()), _http_handler(std::move(handler)), settings(settings)
{
}

void DefaultServer::init_environ()
{
	HTTPServer::init_environ();
	this->base_environ[http::meta::SERVER_NAME] = this->server_name;
	this->base_environ[http::meta::SERVER_PORT] = std::to_string(this->server_port);
}

HandlerFunc DefaultServer::_make_handler()
{
	return [&](int sock, parsers::request_parser* parser, core::Error* err)
	{
		if (err)
		{
			xw::core::Error fail;
			auto err_resp = _from_error(err);

			this->ctx.logger->trace(err->msg, _ERROR_DETAILS_);

			if ((fail = this->send(sock, err_resp->serialize().c_str())))
			{
				this->ctx.logger->error(fail.msg);
			}
		}
		else
		{
			auto request = this->_request(parser);
			this->_http_handler(
				request.get(),
				[this, sock](const http::HttpRequest* req, const core::Result<std::shared_ptr<http::IHttpResponse>>& res) {
					this->_start_response(sock, req, res);
				}
			);
		}
	};
}

std::shared_ptr<http::IHttpResponse> DefaultServer::_from_error(const core::Error* err)
{
	unsigned short code;
	switch (err->type)
	{
		case core::EntityTooLargeError:
			code = 413;
			break;
		case core::PermissionDenied:
			code = 403;
			break;
		case core::NotFound:
		case core::FileDoesNotExistError:
			code = 404;
			break;
		case core::RequestTimeout:
			code = 408;
			break;
		case core::InternalServerError:
			code = 500;
			break;
		case core::SuspiciousOperation:
		case core::DisallowedHost:
		case core::DisallowedRedirect:
			code = 400;
			break;
		case core::HttpError:
		default:
			code = 500;
			break;
	}

	return std::make_shared<http::HttpResponse>(code, err->msg);
}

std::shared_ptr<http::HttpRequest> DefaultServer::_request(parsers::request_parser* parser)
{
	parsers::query_parser qp;
	http::HttpRequest::Parameters<std::string, xw::string> get_params, post_params;
	http::HttpRequest::Parameters<std::string, files::UploadedFile> files_params;
	if (parser->content.empty())
	{
		qp.parse(parser->query);
		if (parser->method == "GET")
		{
			get_params = http::HttpRequest::Parameters(qp.dict, qp.multi_dict);
		}
		else if (parser->method == "POST")
		{
			post_params = http::HttpRequest::Parameters(qp.dict, qp.multi_dict);
		}
	}
	else
	{
		parsers::multipart_parser mp(this->ctx.media_root);
		switch (parser->content_type)
		{
			case parsers::request_parser::content_type_enum::ct_application_x_www_form_url_encoded:
				qp.parse(parser->content);
				if (parser->method == "GET")
				{
					get_params = http::HttpRequest::Parameters(qp.dict, qp.multi_dict);
				}
				else if (parser->method == "POST")
				{
					post_params = http::HttpRequest::Parameters(qp.dict, qp.multi_dict);
				}
				break;
			case parsers::request_parser::content_type_enum::ct_application_json:
				break;
			case parsers::request_parser::content_type_enum::ct_multipart_form_data:
				mp.parse(parser->headers["Content-Type"], parser->content);
				post_params = http::HttpRequest::Parameters(
					mp.post_values, mp.multi_post_value
				);
				files_params = http::HttpRequest::Parameters(
					mp.file_values, mp.multi_file_value
				);
				break;
			case parsers::request_parser::content_type_enum::ct_other:
				break;
			default:
				throw core::ParseError("Unknown content type", _ERROR_DETAILS_);
		}
	}

	auto env_copy = this->base_environ;
	for (const auto& header : parser->headers)
	{
		auto key = str::replace(header.first, "-", "_");
		env_copy["HTTP_" + str::upper(key)] = header.second;
	}

	return std::make_shared<http::HttpRequest>(
		this->settings,
		parser->method,
		parser->path,
		parser->major_v,
		parser->minor_v,
		parser->query,
		parser->keep_alive,
		parser->content,
		parser->headers,
		get_params,
		post_params,
		files_params,
		env_copy
	);
}

core::Error DefaultServer::_send(http::IHttpResponse* response, const int& client)
{
	return this->send(client, response->serialize().c_str());
}

core::Error DefaultServer::_send(http::StreamingHttpResponse* response, const int& client)
{
	std::string chunk;
	while (!(chunk = response->get_chunk()).empty())
	{
		auto err = this->write(client, chunk.c_str(), chunk.size());
		if (err)
		{
			this->ctx.logger->trace("Method 'write' returned an error", _ERROR_DETAILS_);
			return err;
		}
	}

	response->close();
	return core::Error::none();
}

void DefaultServer::_start_response(
	const int& client,
	const http::HttpRequest* request,
	const core::Result<std::shared_ptr<http::IHttpResponse>>& result
)
{
	std::shared_ptr<http::IHttpResponse> response;
	if (result.catch_(core::HttpError))
	{
		this->ctx.logger->trace(result.err.msg, _ERROR_DETAILS_);
		response = _from_error(&result.err);
	}
	else if (!result.value)
	{
		// Response was not instantiated, so return 204 - No Content.
		response = std::make_shared<http::HttpResponse>(204);
		this->ctx.logger->warning(
			"Response was not instantiated, returned 204",
			_ERROR_DETAILS_
		);
	}
	else
	{
		auto error = result.value->err();
		if (error)
		{
			this->ctx.logger->trace(error.msg, _ERROR_DETAILS_);
			response = _from_error(&error);
		}
		else
		{
			response = result.value;
		}
	}

	this->_send_response(request, response.get(), client, this->ctx.logger.get());
}

core::Error DefaultServer::_send_response(
	const http::HttpRequest* request, http::IHttpResponse* response, const int& client, core::ILogger* logger
)
{
	if (response->is_streaming())
	{
		auto* streaming_response = dynamic_cast<http::StreamingHttpResponse*>(response);
		auto err = this->_send(streaming_response, client);
		if (err)
		{
			this->ctx.logger->trace("Method '_send' returned an error", _ERROR_DETAILS_);
			return err;
		}
	}
	else
	{
		auto err = this->_send(response, client);
		if (err)
		{
			this->ctx.logger->trace("Method '_send' returned an error", _ERROR_DETAILS_);
			return err;
		}
	}

	_log_request(
		request->method() + " " +
		request->path() + " HTTP/" +
		request->version(),
		response->status(),
		logger
	);
	return core::Error::none();
}

void DefaultServer::_log_request(
	const std::string& info, unsigned short status_code, core::ILogger* logger
)
{
	if (logger)
	{
		core::Logger::Color color = core::Logger::Color::GREEN;
		if (status_code >= 400)
		{
			color = core::Logger::Color::YELLOW;
		}
		else if (status_code >= 500)
		{
			color = core::Logger::Color::RED;
		}

		logger->print(
			"[" + dt::Datetime::now().strftime("%d/%b/%Y %T") + "] \"" +
			info + "\" " + std::to_string(status_code),
			color
		);
	}
}

__SERVER_END__
