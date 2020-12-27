/**
 * tests_server.cpp
 *
 * Copyright (c) 2020 Yuriy Lisovskiy
 */

#include <iostream>

#include <gtest/gtest.h>

#include "../src/tcp_server.h"

using namespace std;

void onError(int errorCode, const string& errorMessage)
{
	cout << errorCode << " : " << errorMessage << endl;
}

class TestServer
{
public:
	int sizeReceived = 0;

	TestServer()
	{
		// Initialize server socket..
		xw::server::TCPServer tcpServer(onError);

		// When a new client connected:
		tcpServer.onNewConnection = [&](const std::shared_ptr<xw::server::TCPSocket>& newClient) {
			cout << "New client: [";
			cout << newClient->RemoteAddress() << ":" << newClient->RemotePort() << "]" << endl;

			newClient->onRawMessageReceived = [this, newClient](const char* message, int len) mutable {

				this->sizeReceived += len;
				cout << this->sizeReceived << endl;

//				cout << newClient->RemoteAddress() << ":" << newClient->RemotePort() << " => " << message << endl;
				newClient->Send("OK!");
			};

			// If you want to use raw bytes
			/*
			newClient->onRawMessageReceived = [newClient](const char* message, int length) {
				cout << newClient->remoteAddress() << ":" << newClient->remotePort() << " => " << message << "(" << length << ")" << endl;
				newClient->Send("OK!");
			};
			*/

			newClient->onSocketClosed = [newClient]() {
				cout << "Socket closed:" << newClient->RemoteAddress() << ":" << newClient->RemotePort() << endl;
			};

			newClient->Listen();
		};

		// Bind the server to a port.
		if (tcpServer.Bind(3000, onError))
		{
			// Start Listening the server.
			tcpServer.Listen(onError);

			// You should do an input loop so the program will not terminated immediately:
//		string input;
//		getline(cin, input);
//		while (input != "q")
//		{
//			getline(cin, input);
//		}
		}

		// Close the server before exiting the program.
		tcpServer.Close();
	}
};

TEST(ServerTestCase, server)
{
//	TestServer();
}
