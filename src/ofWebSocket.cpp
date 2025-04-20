#include "ofWebSocket.h"

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

ofWebSocket::ofWebSocket()
	: isConnected(false) {
	wsClient.clear_access_channels(websocketpp::log::alevel::all);
	wsClient.init_asio();

	wsClient.set_message_handler(bind(&ofWebSocket::onMessageInternal, this, _1, _2));
	wsClient.set_open_handler(bind(&ofWebSocket::onOpen, this, _1));
	wsClient.set_fail_handler(bind(&ofWebSocket::onFail, this, _1));
	wsClient.set_close_handler(bind(&ofWebSocket::onClose, this, _1));
}

ofWebSocket::~ofWebSocket() {
	close();
}

void ofWebSocket::connect(const std::string & uri) {
	websocketpp::lib::error_code ec;
	auto con = wsClient.get_connection(uri, ec);
	if (ec) {
		ofLogError("ofWebSocket") << "Connection error: " << ec.message();
		return;
	}

	connection = con->get_handle();
	wsClient.connect(con);

	clientThread = std::thread(&ofWebSocket::run, this);
}

void ofWebSocket::run() {
	try {
		wsClient.run();
	} catch (const std::exception & e) {
		ofLogError("ofWebSocket") << "Run exception: " << e.what();
	}
}

void ofWebSocket::parsePayload(const std::string & payload) {

	if (payload.empty()) {
		ofLogError("ofWebSocket") << "Empty payload received.";
		return;
	}
	if (payload[0] != '{') {
		ofLogError("ofWebSocket") << "Invalid payload format: " << payload;
		return;
	}

	ofJson json;

	try {
		json = ofJson::parse(payload);
	} catch (std::exception & e) {
		ofLogError("ofWebSocket") << "JSON parse error: " << e.what();
		return;
	}

	parsedData.id = json["id"];
	parsedData.param = json["param"];
	parsedData.total_parameters = json["total_parameters"];
}

void ofWebSocket::onMessageInternal(websocketpp::connection_hdl hdl, message_ptr msg) {
	std::string payload = msg->get_payload();
	ofLogNotice("ofWebSocket") << "Message received: " << payload;

	if (onMessage) {

		onMessage(payload);
		parsePayload(payload);
	}
}

void ofWebSocket::onOpen(websocketpp::connection_hdl hdl) {
	isConnected = true;
	ofLogNotice("ofWebSocket") << "Connection opened.";
}

void ofWebSocket::onFail(websocketpp::connection_hdl hdl) {
	isConnected = false;
	ofLogError("ofWebSocket") << "Connection failed.";
}

void ofWebSocket::onClose(websocketpp::connection_hdl hdl) {
	isConnected = false;
	ofLogNotice("ofWebSocket") << "Connection closed.";
}

void ofWebSocket::send(const std::string & message) {
	if (!isConnected) {
		ofLogWarning("ofWebSocket") << "Cannot send message: Not connected.";
		return;
	}

	websocketpp::lib::error_code ec;
	wsClient.send(connection, message, websocketpp::frame::opcode::text, ec);
	if (ec) {
		ofLogError("ofWebSocket") << "Send failed: " << ec.message();
	}
}

void ofWebSocket::close() {
	if (!isConnected) return;

	websocketpp::lib::error_code ec;
	wsClient.close(connection, websocketpp::close::status::normal, "Closing", ec);
	if (ec) {
		ofLogError("ofWebSocket") << "Close failed: " << ec.message();
	}

	if (clientThread.joinable()) {
		clientThread.join();
	}

	isConnected = false;
}
