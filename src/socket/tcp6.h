/**
 * socket/tcp6.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

class TCP6Socket : public BaseSocket
{
protected:
	void bind() override;

public:
	explicit TCP6Socket(const char* address, uint16_t port);
	void set_options() override;
};

__SERVER_END__
