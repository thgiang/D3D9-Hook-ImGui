#include "MusicManager.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void MusicManager::requestCheoroMusic() {
    this->socket->send(R"({"event": "get_music_cheoro_list"})");
}

void MusicManager::handleConnectionStatus(const bool connected) {
    if (connected) {
        this->socket->send(R"({"event": "get_music_cheoro_list"})");
    }
}
void MusicManager::handleServerMessage(const std::string& msg) {
    try {
        auto j = json::parse(msg);

        if (j.contains("event") && j["event"] == "music_cheoro_list") {
            cheoroList.clear();

            for (const auto& item : j["data"]) {
                cheoroList.emplace_back(
                    item["id"],
                    item["author"],
                    item["title"],
                    item["bpm"],
                    item["fileName"],
                    item["owner"],
                    item["duration"],
                    item["isPrivate"]
                );
            }

            std::cout << "[MusicManager] Received " << cheoroList.size() << " cheoro songs from server\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[MusicManager] Failed to parse message: " << e.what() << "\n";
    }
}
