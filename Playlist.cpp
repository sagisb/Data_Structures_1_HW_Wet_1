#include "Playlist.h"
#include <iostream>
#include <algorithm>

PlayCountNode::PlayCountNode(int count, Song* song) : playCount(count), songPtr(song), left(nullptr), right(nullptr) {}
SongNodeList::SongNodeList(Song* song) : songPtr(song), next(nullptr) {}

Playlist::Playlist(int id) : playlistId(id), songsByIdRoot(nullptr), songsByPlayCountRoot(nullptr), songListHead(nullptr) {}

Playlist::~Playlist() {
    destroyIdTree(songsByIdRoot);
    destroyPlayCountTree(songsByPlayCountRoot);
    destroyList(songListHead);
}

int Playlist::getPlaylistId() const {
    return playlistId;
}

void Playlist::addSong(Song* song) {
    if (!searchById(songsByIdRoot, song->getSongId())) {
        songsByIdRoot = insertById(songsByIdRoot, song);
        songsByPlayCountRoot = insertByPlayCount(songsByPlayCountRoot, song);
        appendToList(song);
    }
}

void Playlist::removeSong(int songId) {
    songsByIdRoot = deleteById(songsByIdRoot, songId);
    removeFromList(songId);
}

void Playlist::playSong(int songId) {
    AVLNodeSong* node = searchById(songsByIdRoot, songId);
    if (node && node->song_ptr) {
        node->song_ptr->increaseCountPlayed();
    }
}

Song* Playlist::getSongById(int songId) const {
    AVLNodeSong* node = searchById(songsByIdRoot, songId);
    return node ? node->song_ptr : nullptr;
}

Song** Playlist::getAllSongs() const {
    int size = getListSize();
    if (size == 0) return nullptr;
    Song** allSongs = new Song*[size];
    SongNodeList* current = songListHead;
    for (int i = 0; i < size; ++i) {
        allSongs[i] = current->songPtr;
        current = current->next;
    }
    return allSongs;
}

AVLNodeSong* Playlist::insertById(AVLNodeSong* root, Song* song) {
    if (!root) return new AVLNodeSong(song->getSongId(), song);
    if (song->getSongId() < root->songId) {
        root->left = insertById(root->left, song);
    } else if (song->getSongId() > root->songId) {
        root->right = insertById(root->right, song);
    }
    return root;
}

AVLNodeSong* Playlist::searchById(AVLNodeSong* root, int songId) const {
    if (!root || root->songId == songId) return root;
    if (songId < root->songId) return searchById(root->left, songId);
    else return searchById(root->right, songId);
}

AVLNodeSong* Playlist::deleteById(AVLNodeSong* root, int songId) {
    return nullptr;
}

void Playlist::destroyIdTree(AVLNodeSong* root) {
    if (root) {
        destroyIdTree(root->left);
        destroyIdTree(root->right);
        delete root;
    }
}

PlayCountNode* Playlist::insertByPlayCount(PlayCountNode* root, Song* song) {
    if (!root) return new PlayCountNode(song->getCountPlayed(), song);
    if (song->getCountPlayed() < root->playCount) {
        root->left = insertByPlayCount(root->left, song);
    } else if (song->getCountPlayed() > root->playCount) {
        root->right = insertByPlayCount(root->right, song);
    } else {
        root->right = insertByPlayCount(root->right, song);
    }
    return root;
}

PlayCountNode* Playlist::searchByPlayCount(PlayCountNode* root, int playCount, int songId) const {
    if (!root) return nullptr;
    if (root->playCount == playCount && root->songPtr->getSongId() == songId) return root;
    if (playCount < root->playCount) return searchByPlayCount(root->left, playCount, songId);
    else return searchByPlayCount(root->right, playCount, songId);
}

PlayCountNode* Playlist::deleteByPlayCount(PlayCountNode* root, int playCount, int songId) {
    return nullptr;
}

void Playlist::destroyPlayCountTree(PlayCountNode* root) {
    if (root) {
        destroyPlayCountTree(root->left);
        destroyPlayCountTree(root->right);
        delete root;
    }
}

void Playlist::appendToList(Song* song) {
    SongNodeList* newNode = new SongNodeList(song);
    if (!songListHead) {
        songListHead = newNode;
        return;
    }
    SongNodeList* current = songListHead;
    while (current->next) {
        current = current->next;
    }
    current->next = newNode;
}

void Playlist::removeFromList(int songId) {
    if (!songListHead) return;
    if (songListHead->songPtr->getSongId() == songId) {
        SongNodeList* temp = songListHead;
        songListHead = songListHead->next;
        delete temp;
        return;
    }
    SongNodeList* current = songListHead;
    while (current->next && current->next->songPtr->getSongId() != songId) {
        current = current->next;
    }
    if (current->next) {
        SongNodeList* temp = current->next;