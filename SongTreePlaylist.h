#pragma once

#include "Song.h"

struct SongNodeList;

struct PlaylistTreeNode {
    int songId;
    Song *songPtr;
    SongNodeList *linkedListNodePtr;
    PlaylistTreeNode *left;
    PlaylistTreeNode *right;
    int height;

    PlaylistTreeNode(Song *song, SongNodeList *node);
};

class SongTreePlaylist {
private:
    PlaylistTreeNode *root;

    int height(PlaylistTreeNode *node) const;

    int balanceFactor(PlaylistTreeNode *node) const;

    void updateHeight(PlaylistTreeNode *node);

    PlaylistTreeNode *rotateLeft(PlaylistTreeNode *x);

    PlaylistTreeNode *rotateRight(PlaylistTreeNode *y);

    PlaylistTreeNode *insert(PlaylistTreeNode *root, Song *song, SongNodeList *node);

    PlaylistTreeNode *search(PlaylistTreeNode *root, int songId) const;

    PlaylistTreeNode *deleteNode(PlaylistTreeNode *root, int songId);

    PlaylistTreeNode *findMin(PlaylistTreeNode *node) const;

    void destroyTree(PlaylistTreeNode *root);


public:
    SongTreePlaylist();

    explicit SongTreePlaylist(int nodesAmount);

    ~SongTreePlaylist();

    void addSong(Song *song, SongNodeList *node);

    Song *getSongById(int songId) const;

    SongNodeList *getSongNode(int songId) const;

    void removeSong(int songId);

    SongNodeList *toLinkedList();

    PlaylistTreeNode *getRoot();

    void populateTree(SongNodeList *songsList);
};