/**
 * selectors.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Request selectors implementations.
 */

#pragma once

// C++ libraries.
#if defined(__linux__) || defined(__APPLE__)
#include <sys/socket.h>
#include <sys/select.h>
#elif _WIN32
#include <winsock32.h>
#endif

// Base libraries.
#include <xalwart.base/logger.h>

// Module definitions.
#include "./_def_.h"


__SERVER_BEGIN__

const int EVENT_READ = (1 << 0);
const int EVENT_WRITE = (1 << 1);

class ISelector
{
public:
	virtual void register_(uint file_descriptor, int events) = 0;
	virtual bool select(uint timeout_sec, uint timeout_usec) = 0;
};

class SelectSelector : public ISelector
{
protected:
	log::ILogger* logger;
	fd_set readers{};
	fd_set writers{};
	int fd;
	int events;

public:
	explicit SelectSelector(log::ILogger* logger);
	void register_(uint fd, int events) override;
	bool select(uint timeout_sec, uint timeout_usec) override;
};

__SERVER_END__
