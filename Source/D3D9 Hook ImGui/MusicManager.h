#pragma once
#include <vector>
#include <string>
#include "MusicCheoro.h"
#include "WebSocketClient.h"
#include <iostream>

class MusicManager {
public:
    static MusicManager& getInstance(WebSocketClient* socket = nullptr) {
        static MusicManager instance(socket);
        return instance;
    }

    const std::vector<MusicCheoro>& getCheoroList() const {
        return cheoroList;
    }

private:
    std::vector<MusicCheoro> cheoroList;
    WebSocketClient* socket;

    MusicManager(WebSocketClient* clientSocket)
        : socket(clientSocket)
    {
        if (socket) {
            socket->setMessageCallback([this](const std::string& msg) {
                handleServerMessage(msg);
                });

            socket->send(R"({"event": "get_music_list"})");
        }

        // Optional fallback
        if (!socket) {
            cheoroList.emplace_back(1, "John Doe", "Song 1", 120.0f, "song1.mp3", "vukaka1988", 180, true);
        }
    }

    void handleServerMessage(const std::string& msg);

    // singleton
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
};
