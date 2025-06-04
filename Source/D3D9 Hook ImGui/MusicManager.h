#pragma once
#pragma once
#include <vector>
#include <string>
#include "MusicCheoro.h"

class MusicManager {
public:
    static MusicManager& getInstance() {
        static MusicManager instance; // C++11 thread-safe singleton
        return instance;
    }

    const std::vector<MusicCheoro>& getMusicList() const {
        return cheoroList;
    }

private:
    std::vector<MusicCheoro> cheoroList;

    MusicManager() {
        loadMusicList("music.txt");
    }

    void loadMusicList(const std::string& fileName) {
        // Sample data
        cheoroList.emplace_back(1, "John Doe", "Song 1", 120.0f, "song1.mp3", "vukaka1988", 180, true);
        cheoroList.emplace_back(2, "Jane Smith", "Song 2", 130.0f, "song2.mp3", "anti.shortbrain", 200, false);
		cheoroList.emplace_back(3, "Alice Johnson", "Song 3", 140.0f, "song3.mp3", "vukaka1988", 220, true);
		cheoroList.emplace_back(4, "Bob Brown", "Song 4", 150.0f, "song4.mp3", "anti.shortbrain", 240, false);
		cheoroList.emplace_back(5, "Charlie White", "Song 5", 160.0f, "song5.mp3", "vukaka1988", 260, true);
    }

    // Không cho phép copy/gán
    MusicManager(const MusicManager&) = delete;
    MusicManager& operator=(const MusicManager&) = delete;
};
