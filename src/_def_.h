/**
 * _def_.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: main module's definitions.
 */

#pragma once

#include <xalwart.core/_def_.h>


// xw::server
#define __SERVER_BEGIN__ __MAIN_NAMESPACE_BEGIN__ namespace server {
#define __SERVER_END__ } __MAIN_NAMESPACE_END__

// xw::server::internal
#define __SERVER_INTERNAL_BEGIN__ __SERVER_BEGIN__ namespace internal {
#define __SERVER_INTERNAL_END__ } __SERVER_END__


__SERVER_BEGIN__

typedef std::function<void(int, const std::string&)> OnErrorFunc;

__SERVER_END__
