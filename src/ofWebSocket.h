#pragma once

#include "ofMain.h"
#include <atomic>
#include <functional>
#include <thread>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

class ofWebSocket {
public:
	ofWebSocket();
	~ofWebSocket();

	void connect(const std::string & uri);
	void send(const std::string & message);
	void close();

	std::function<void(const std::string &)> onMessage;

	struct ParsedData {
		std::string id;
		int param;
		int total_parameters;
	};

	ParsedData parsedData;
	std::atomic<bool> isConnected;

private:
	using client = websocketpp::client<websocketpp::config::asio_client>;
	using message_ptr = websocketpp::config::asio_client::message_type::ptr;

	client wsClient;
	websocketpp::connection_hdl connection;
	std::thread clientThread;

	void run();
	void onMessageInternal(websocketpp::connection_hdl hdl, message_ptr msg);
	void onOpen(websocketpp::connection_hdl hdl);
	void onFail(websocketpp::connection_hdl hdl);
	void onClose(websocketpp::connection_hdl hdl);

	void parsePayload(const std::string & payload);
};
