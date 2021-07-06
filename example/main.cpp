/**
 * main.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./server.h"

int main()
{
	xw::InterruptException::initialize();
	HelloWorldServer("/tmp/xw.sock");
	return 0;
}
