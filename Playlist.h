#pragma once

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

    Song *getSongByPlayCount(int songId) const;

    SongNodeList *getListHead();

    void setListHead(SongNodeList *newHead);

    SongNodeList *getListTTail();

    void setListTail(SongNodeList *newTail);

    void setSongsByIdTree(SongTreePlaylist *newSongsByIdTree);

    void setAVLPlayCount(PlayCountNode *newAVLPlayCount);

    void setNumOfSongs(int newNumOfSongs);

    SongTreePlaylist *getSongsByIdTree();

    PlayCountNode *getAVLPlayCount();


private:
    int heightPlayCount(PlayCountNode *node) const;

    int balanceFactorPlayCount(PlayCountNode *node) const;

    void updateHeightPlayCount(PlayCountNode *node);

    PlayCountNode *rotateLeftPlayCount(PlayCountNode *x);

    PlayCountNode *rotateRightPlayCount(PlayCountNode *y);

    PlayCountNode *findMinPlayCount(PlayCountNode *node) const;

    PlayCountNode *insertByPlayCount(PlayCountNode *root, Song *songToInsert);

    PlayCountNode *searchByPlayCount(PlayCountNode *root, int searchPlayCount, int searchSongId) const;

    PlayCountNode *deleteByPlayCount(PlayCountNode *root, int targetPlayCount, int targetSongId);

    void destroyPlayCountTree(PlayCountNode *rootNode);

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

    explicit PlayCountNode(int nodesAmount);

    SongNodeList *toLinkedList();

    void populateCountNodeTree(PlayCountNode *root, SongNodeList *songsList);

    PlayCountNode *findMinimalUpperPlayCount(int playCount);
};

struct SongNodeList {
    Song *songPtr;
    SongNodeList *next;
    SongNodeList *prev;

    explicit SongNodeList(Song *song);

    ~SongNodeList() {
        SongNodeList *p = next;
        while (p) {
            SongNodeList *tmp = p->next;
            p->next = nullptr;
            p->prev = nullptr;
            delete p;
            p = tmp;
        }
        next = prev = nullptr;
        songPtr = nullptr;
    }
};
