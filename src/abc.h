/**
 * abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for server library.
 */

#pragma once

// Module definitions.
#include "./_def_.h"


__SERVER_ABC_BEGIN__

// TODO: docs for 'ISelector'
class ISelector
{
public:
	virtual ~ISelector() = default;

	virtual void register_(uint file_descriptor, int events) = 0;

	virtual bool select(uint timeout_sec, uint timeout_usec) = 0;
};

__SERVER_ABC_END__
