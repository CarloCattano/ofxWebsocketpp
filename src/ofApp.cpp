#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	webSocket.onMessage = [&](const std::string & msg) {
		ofLogNotice() << "Received: " << msg;
		// Handle the message as needed
	};

	webSocket.connect("ws://ws.42ls.online/of-ws");
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (webSocket.isConnected) {
		ofWebSocket::ParsedData data = webSocket.parsedData;

		ofSetBackgroundColor(0, 0, 0);

		static int x = 0;
		static int y = 0;
		static float radius = 0.0f;

		if (data.id == "slider_0") {
			x = data.param;
		} else if (data.id == "slider_1") {
			y = data.param;
		} else if (data.id == "slider_2") {
			radius = data.param;
		} else if (data.id == "toggle_0") {
			if (data.param == 1) {
				ofSetBackgroundColor(0, 255, 0);
			} else {
				ofSetBackgroundColor(255, 0, 0);
			}
		}

		ofSetColor(255, 0, 0);
		ofDrawCircle(x, y, radius);
		ofSetColor(255);

		ofTrueTypeFont font;

		font.load("verdana.ttf", 32, true, true);
		// font.setLetterSpacing(1.037);
		// font.setLineHeight(34.0f);
		font.drawString("x: " + ofToString(x), 10, 50);
		font.drawString("y: " + ofToString(y), 10, 100);
		font.drawString("radius: " + ofToString(radius), 10, 150);
		font.drawString("Press 'q' to quit", 10, 200);

	} else {
		ofSetBackgroundColor(42, 42, 42);
		ofSetColor(255);
		ofDrawBitmapString("Connection failed. Press 'q' to quit.", 10, 20);
		ofDrawBitmapString("Check if the server is running.", 10, 40);
		ofDrawBitmapString("Press 'q' to quit", 10, 60);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'q') {
		webSocket.close();
		ofExit();
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {
}

// void ofApp::exit() {
// 	webSocket.close();
// }
