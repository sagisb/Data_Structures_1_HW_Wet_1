//#include "Song.h"
#include "SongTreePlaylist.h"

struct PlayCountNode;
struct SongNodeList;

class Playlist {
private:
    int playlistId;
    int numOfSongs;
    SongTreePlaylist *songsByIdTree;
    PlayCountNode *AVLPlayCount;
    SongNodeList *songListHead;
    SongNodeList *songListTail;


public:
    Playlist(int id);
    ~Playlist();

    int getPlaylistId() const;
    int getNumOfSongs() const;

    void addSong(Song *song);
    void removeSong(int songId);
    void playSong(int songId);
    Song *getSongById(int songId) const;
    Song **getAllSongs() const;
    SongNodeList *getSongNodeInList(int songId) const;

private:
    int heightPlayCount(PlayCountNode* node) const;
    int balanceFactorPlayCount(PlayCountNode* node) const;
    void updateHeightPlayCount(PlayCountNode* node);
    PlayCountNode* rotateLeftPlayCount(PlayCountNode* x);
    PlayCountNode* rotateRightPlayCount(PlayCountNode* y);
    PlayCountNode* findMinPlayCount(PlayCountNode* node) const;

    PlayCountNode *insertByPlayCount(PlayCountNode *root, Song *song);
    PlayCountNode *searchByPlayCount(PlayCountNode *root, int playCount, int songId) const;
    PlayCountNode *deleteByPlayCount(PlayCountNode *root, int playCount, int songId);
    void destroyPlayCountTree(PlayCountNode *root);

    void appendToList(Song *song);
    void removeFromList(int songId);
    void destroyList(SongNodeList *head);
    //int getListSize() const;
};

struct PlayCountNode {
    int playCount;
    Song *songPtr;
    PlayCountNode *left;
    PlayCountNode *right;
    int height;

    PlayCountNode(int count, Song *song);
};

struct SongNodeList {
    Song *songPtr;
    SongNodeList *next;
    SongNodeList *prev;
    SongNodeList(Song *song);
};
