#include "Playlist.h"
#include <iostream>
#include <algorithm>

PlayCountNode::PlayCountNode(int count, Song *song) : playCount(count), songPtr(song),
                                                      left(nullptr), right(nullptr) {}

SongNodeList::SongNodeList(Song *song) : songPtr(song), next(nullptr), prev(nullptr) {}

Playlist::Playlist(int id) : playlistId(id), songsByPlayCountRoot(nullptr), songListHead(nullptr),
                             songListTail(nullptr) {}

Playlist::~Playlist() {
    destroyPlayCountTree(songsByPlayCountRoot);
    destroyList(songListHead);
}

int Playlist::getPlaylistId() const {
    return playlistId;
}

void Playlist::addSong(Song *song) {
    appendToList(song);
    SongNodeList *newNode = songListTail;
    songsByIdTree.addSong(song, newNode);
}

void Playlist::removeSong(int songId) {
    songsByIdTree.removeSong(songId);
    removeFromList(songId);
}

void Playlist::playSong(int songId) {
    Song *song = songsByIdTree.getSongById(songId);
    if (song) {
        song->increaseCountPlayed();
    }
}

Song *Playlist::getSongById(int songId) const {
    return songsByIdTree.getSongById(songId);
}

Song **Playlist::getAllSongs() const {
    int size = getListSize();
    if (size == 0) { return nullptr; }
    Song **allSongs = new Song *[size];
    SongNodeList *current = songListHead;
    for (int i = 0; i < size; ++i) {
        allSongs[i] = current->songPtr;
        current = current->next;
    }
    return allSongs;
}

SongNodeList *Playlist::getSongNodeInList(int songId) const {
    SongNodeList *current = songListHead;
    while (current) {
        if (current->songPtr->getSongId() == songId) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

PlayCountNode *Playlist::insertByPlayCount(PlayCountNode *root, Song *song) {
    if (!root) { return new PlayCountNode(song->getCountPlayed(), song); }
    if (song->getCountPlayed() < root->playCount) {
        root->left = insertByPlayCount(root->left, song);
    }
    else if (song->getCountPlayed() > root->playCount) {
        root->right = insertByPlayCount(root->right, song);
    }
    else {
        root->right = insertByPlayCount(root->right, song);
    }
    return root;
}

PlayCountNode *Playlist::searchByPlayCount(PlayCountNode *root, int playCount, int songId) const {
    if (!root) { return nullptr; }
    if (root->playCount == playCount && root->songPtr->getSongId() == songId) { return root; }
    if (playCount < root->playCount) { return searchByPlayCount(root->left, playCount, songId); }
    else { return searchByPlayCount(root->right, playCount, songId); }
}

PlayCountNode *Playlist::deleteByPlayCount(PlayCountNode *root, int playCount, int songId) {
    return nullptr;
}

void Playlist::destroyPlayCountTree(PlayCountNode *root) {
    if (root) {
        destroyPlayCountTree(root->left);
        destroyPlayCountTree(root->right);
        delete root;
    }
}

void Playlist::appendToList(Song *song) {
    SongNodeList *newNode = new SongNodeList(song);
    if (!songListHead) {
        songListHead = tail = newNode;
    }
    else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
}

void Playlist::removeFromList(int songId) {
    if (!songListHead) { return; }
    if (songListHead->songPtr->getSongId() == songId) {
        SongNodeList *temp = songListHead;
        songListHead = songListHead->next;
        if (songListHead) { songListHead->prev = nullptr; }
        delete temp;
        return;
    }
    SongNodeList *current = songListHead;
    while (current->next && current->next->songPtr->getSongId() != songId) {
        current = current->next;
    }
    if (current->next) {
        SongNodeList *temp = current->next;
        current->next = temp->next;
        if (temp->next) { temp->next->prev = current; }
        if (temp == tail) { tail = current; }
        delete temp;
    }
}

void Playlist::destroyList(SongNodeList *head) {
    SongNodeList *current = head;
    while (current) {
        SongNodeList *next = current->next;
        delete current;
        current = next;
    }
    songListHead = songListTail = nullptr;
}

int Playlist::getListSize() const {
    int count = 0;
    SongNodeList *current = songListHead;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}