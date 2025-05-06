#include "Playlist.h"
#include <iostream>

PlayCountNode::PlayCountNode(int count, Song *song) : playCount(count), songPtr(song),
                                                      left(nullptr), right(nullptr) {}

SongNodeList::SongNodeList(Song *song) : songPtr(song), next(nullptr), prev(nullptr) {}

Playlist::Playlist(int playlistId) : playlistId(id), songsByPlayCountRoot(nullptr), songListHead(nullptr),
                             songListTail(nullptr) {
    songsByIdTree = new SongTreePlaylist();
}

Playlist::~Playlist() {
    destroyPlayCountTree(songsByPlayCountRoot);
    songsByPlayCountRoot = nullptr;
    destroyList(songListHead);
    delete songsByIdTree;
    songsByIdTree = nullptr;
}

int Playlist::getPlaylistId() const {
    return playlistId;
}

void Playlist::addSong(Song *song) {
    appendToList(song);
    if (songListTail) {
        songsByIdTree->addSong(song, songListTail);
    }
}

void Playlist::removeSong(int songId) {
    songsByIdTree->removeSong(songId);
    removeFromList(songId);
}

void Playlist::playSong(int songId) {
    Song *song = songsByIdTree->getSongById(songId);
    if (song) {
        song->increaseCountPlayed();
    }
}

Song *Playlist::getSongById(int songId) const {
    return songsByIdTree->getSongById(songId);
}

Song **Playlist::getAllSongs() const {
    int size = getListSize();
    if (size == 0) { return nullptr; }
    Song **allSongs = new Song *[size];
    SongNodeList *current = songListHead;
    for (int i = 0; i < size; ++i) {
        if (current == nullptr) {
            delete[] allSongs;
            return nullptr;
        }
        allSongs[i] = current->songPtr;
        current = current->next;
    }
    return allSongs;
}

SongNodeList *Playlist::getSongNodeInList(int songId) const {
    SongNodeList *current = songListHead;
    while (current) {
        if (current->songPtr && current->songPtr->getSongId() == songId) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

PlayCountNode *Playlist::insertByPlayCount(PlayCountNode *root, Song *song) {
    if (!song) { return root; }
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

    if (playCount == root->playCount) {
        if (root->songPtr && root->songPtr->getSongId() == songId) {
            return root;
        }
        PlayCountNode *found_in_left = searchByPlayCount(root->left, playCount, songId);
        if (found_in_left) { return found_in_left; }
        return searchByPlayCount(root->right, playCount, songId);
    }
    else if (playCount < root->playCount) {
        return searchByPlayCount(root->left, playCount, songId);
    }
    else {
        return searchByPlayCount(root->right, playCount, songId);
    }
}

PlayCountNode *Playlist::deleteByPlayCount(PlayCountNode *root, int playCount, int songId) {
    if (!root) { return nullptr; }

    if (playCount < root->playCount) {
        root->left = deleteByPlayCount(root->left, playCount, songId);
    }
    else if (playCount > root->playCount ||
             (root->songPtr && root->songPtr->getSongId() != songId &&
              playCount == root->playCount)) {
        if (playCount == root->playCount) {
            if (root->songPtr && songId < root->songPtr->getSongId()) {
                root->left = deleteByPlayCount(root->left, playCount, songId);
            }
            else {
                root->right = deleteByPlayCount(root->right, playCount, songId);
            }
        }
        else {
            root->right = deleteByPlayCount(root->right, playCount, songId);
        }
    }
    else {
        if (root->left == nullptr) {
            PlayCountNode *temp = root->right;
            delete root;
            return temp;
        }
        else if (root->right == nullptr) {
            PlayCountNode *temp = root->left;
            delete root;
            return temp;
        }

        PlayCountNode *temp = root->right;
        while (temp && temp->left != nullptr) {
            temp = temp->left;
        }
        root->playCount = temp->playCount;
        root->songPtr = temp->songPtr;
        root->right = deleteByPlayCount(root->right, temp->playCount, temp->songPtr->getSongId());
    }
    return root;
}


void Playlist::destroyPlayCountTree(PlayCountNode *root) {
    if (root) {
        destroyPlayCountTree(root->left);
        destroyPlayCountTree(root->right);
        delete root;
    }
}

void Playlist::appendToList(Song *song) {
    if (!song) { return; }
    SongNodeList *newNode = new SongNodeList(song);
    if (!songListHead) {
        songListHead = songListTail = newNode;
    }
    else {
        songListTail->next = newNode;
        newNode->prev = songListTail;
        songListTail = newNode;
    }
}

void Playlist::removeFromList(int songId) {
    if (!songListHead) { return; }

    SongNodeList *current = songListHead;

    while (current != nullptr) {
        if (current->songPtr && current->songPtr->getSongId() == songId) {
            break;
        }
        current = current->next;
    }

    if (current == nullptr) {
        return;
    }

    if (current == songListHead) {
        songListHead = current->next;
    }
    if (current == songListTail) {
        songListTail = current->prev;
    }
    if (current->prev) {
        current->prev->next = current->next;
    }
    if (current->next) {
        current->next->prev = current->prev;
    }
    delete current;
}

void Playlist::destroyList(SongNodeList *head_param) {
    SongNodeList *current = songListHead;
    while (current) {
        SongNodeList *nextNode = current->next;
        delete current;
        current = nextNode;
    }
    songListHead = nullptr;
    songListTail = nullptr;
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