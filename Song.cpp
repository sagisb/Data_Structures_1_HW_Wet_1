#include "Song.h"
#include <stdexcept>

Song::Song(int songId) : songId(songId), countPlayed(0), countConnectionPlaylist(0) {
    if (songId <= 0) {
        throw std::invalid_argument("Song ID must be a positive integer.");
    }
}

int Song::getSongId() const {
    return songId;
}

int Song::getCountPlayed() const {
    return countPlayed;
}

int Song::getCountPlaylist() const {
    return countConnectionPlaylist;
}

void Song::increaseCountPlayed() {
    countPlayed++;
}

void Song::increaseSongToPlaylist() {
    countConnectionPlaylist++;
}

void Song::decrementingSongFromPlaylist() {
    countConnectionPlaylist--;
}