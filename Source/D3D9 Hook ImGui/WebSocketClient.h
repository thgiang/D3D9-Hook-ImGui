#pragma once
#include <ixwebsocket/IXWebSocket.h>
#include <string>
#include <functional>

#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Bcrypt.lib")

class WebSocketClient {
public:
    using MessageCallback = std::function<void(const std::string&)>;
    using StatusCallback = std::function<void(bool connected)>;

    WebSocketClient(const std::string& url, const std::string& token);
    ~WebSocketClient();

    void start();
    void stop();
    void send(const std::string& message);
    void setMessageCallback(MessageCallback callback);
    void setStatusCallback(StatusCallback callback);

private:
    std::string url;
    std::string authToken;
    ix::WebSocket webSocket;
    MessageCallback onMessage;
    StatusCallback onStatus;

    void setupCallbacks();
};
