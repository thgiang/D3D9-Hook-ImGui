#pragma once
#include <string>

using std::string;

class MusicCheoro {
public:
    int id;
    string author;
    string title;
    float bpm;
    string fileName;
    string owner;
    int duration;
    bool isPrivate;

    MusicCheoro(int id, string author, string title, float bpm,
                string fileName, string owner, int duration, bool isPrivate)
        : id(id), author(author), title(title), bpm(bpm),
          fileName(fileName), owner(owner), duration(duration), isPrivate(isPrivate) {}
};
