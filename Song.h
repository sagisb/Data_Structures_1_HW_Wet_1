#pragma once

class Song {
private:
    int songId;
    int countPlayed;
    int countConnectionPlaylist;

public:
    Song(int songId, int played);

    int getSongId() const;
    int getCountPlayed() const;
    int getCountPlaylist() const;

    void increaseCountPlayed();
    void increaseSongToPlaylist();
    void decrementingSongFromPlaylist();
};
