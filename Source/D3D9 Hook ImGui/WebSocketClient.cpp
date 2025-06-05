#include "WebSocketClient.h"
#include <iostream>

WebSocketClient::WebSocketClient(const std::string& url, const std::string& token)
    : url(url), authToken(token)
{
    webSocket.setUrl(url);
    webSocket.setPingInterval(15); // ping every 15s
    setupCallbacks();
}

WebSocketClient::~WebSocketClient() {
    stop();
}

void WebSocketClient::start() {
    webSocket.start();
}

void WebSocketClient::stop() {
    webSocket.stop();
}

void WebSocketClient::send(const std::string& message) {
    webSocket.send(message);
}

void WebSocketClient::setMessageCallback(MessageCallback callback) {
    onMessage = callback;
}

void WebSocketClient::setStatusCallback(StatusCallback callback) {
    onStatus = callback;
}

void WebSocketClient::setupCallbacks() {
    webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
        switch (msg->type) {
        case ix::WebSocketMessageType::Open:
            std::cout << "[WebSocket] Connected to server\n";
            webSocket.send("AUTH:" + authToken);
            if (onStatus) onStatus(true);
            break;

        case ix::WebSocketMessageType::Message:
            if (onMessage) {
                onMessage(msg->str);
            }
            break;

        case ix::WebSocketMessageType::Close:
            std::cout << "[WebSocket] Disconnected\n";
            if (onStatus) onStatus(false);
            break;

        case ix::WebSocketMessageType::Error:
            std::cerr << "[WebSocket] Error: " << msg->errorInfo.reason << "\n";
            if (onStatus) onStatus(false);
            break;

        default:
            break;
        }
        });
}
