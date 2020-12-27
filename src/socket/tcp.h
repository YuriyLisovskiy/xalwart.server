/**
 * socket/tcp.h
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <functional>
#include <thread>

// Module definitions.
#include "../_def_.h"

// Server libraries.
#include "./base.h"


__SERVER_BEGIN__

class TCPSocket : public BaseSocket
{
public:
    // Event Listeners:
    std::function<void(std::string)> onMessageReceived;
    std::function<void(const char*, int)> onRawMessageReceived;
    std::function<void()> onSocketClosed;

    explicit TCPSocket(FDR_ON_ERROR, int socketId = -1, bool useIPv6 = false);

    int Send(const std::string& message);
    int Send(const char* bytes, size_t len);

    void Connect(const char* host, uint16_t port, const std::function<void()>& onConnected = [](){}, FDR_ON_ERROR);

    void Listen();

    void setAddressStruct(sockaddr_in addr);
    void setAddress6Struct(sockaddr_in6 addr);

private:
    static void Receive(TCPSocket* socket);
};

__SERVER_END__
