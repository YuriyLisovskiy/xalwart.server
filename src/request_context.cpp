/**
 * request_context.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./request_context.h"


__SERVER_BEGIN__

bool RequestContext::proto_v_eq(short major, short minor) const
{
	return this->major_v == major && this->minor_v == minor;
}

bool RequestContext::proto_v_gte(short major, short minor) const
{
	return this->major_v >= major && this->minor_v >= minor;
}

bool RequestContext::proto_v_lte(short major, short minor) const
{
	return this->major_v <= major && this->minor_v <= minor;
}

bool RequestContext::proto_v_gt(short major, short minor) const
{
	return this->major_v > major && this->minor_v > minor;
}

bool RequestContext::proto_v_lt(short major, short minor) const
{
	return this->major_v < major && this->minor_v < minor;
}

__SERVER_END__
