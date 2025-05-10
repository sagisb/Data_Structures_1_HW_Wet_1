#include "Playlist.h"
#include <iostream>

PlayCountNode::PlayCountNode(int count, Song *s) :
        playCount(count), songPtr(s), left(nullptr), right(nullptr), height(1) {}

SongNodeList::SongNodeList(Song *s) : songPtr(s), next(nullptr), prev(nullptr) {}

Playlist::Playlist(int id) :
        playlistId(id),
        numOfSongs(0),
        AVLPlayCount(nullptr),
        songListHead(nullptr),
        songListTail(nullptr) {
    songsByIdTree = new SongTreePlaylist();
}

Playlist::~Playlist() {
    destroyPlayCountTree(AVLPlayCount);
    AVLPlayCount = nullptr;
    destroyList(songListHead);
    delete songsByIdTree;
    songsByIdTree = nullptr;
}

int Playlist::getPlaylistId() const {
    return playlistId;
}

int Playlist::getNumOfSongs() const {
    return numOfSongs;
}

int Playlist::heightPlayCount(PlayCountNode *node) const {
    return (node == nullptr) ? 0 : node->height;
}

void Playlist::updateHeightPlayCount(PlayCountNode *node) {
    if (node) {
        int leftH = heightPlayCount(node->left);
        int rightH = heightPlayCount(node->right);
        node->height = 1 + (leftH > rightH ? leftH : rightH);
    }
}

int Playlist::balanceFactorPlayCount(PlayCountNode *node) const {
    return (node == nullptr) ? 0 : heightPlayCount(node->left) - heightPlayCount(node->right);
}

PlayCountNode *Playlist::rotateRightPlayCount(PlayCountNode *y) {
    if (!y || !y->left) { return y; }
    PlayCountNode *x = y->left;
    PlayCountNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeightPlayCount(y);
    updateHeightPlayCount(x);

    return x;
}

PlayCountNode *Playlist::rotateLeftPlayCount(PlayCountNode *x) {
    if (!x || !x->right) { return x; }
    PlayCountNode *y = x->right;
    PlayCountNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeightPlayCount(x);
    updateHeightPlayCount(y);

    return y;
}

PlayCountNode *Playlist::findMinPlayCount(PlayCountNode *node) const {
    PlayCountNode *current = node;
    while (current != nullptr && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

PlayCountNode *Playlist::insertByPlayCount(PlayCountNode *root, Song *songToInsert) {
    if (!songToInsert) { return root; }

    int key_playCount = songToInsert->getCountPlayed();
    int key_songId = songToInsert->getSongId();

    if (root == nullptr) {
        return new PlayCountNode(key_playCount, songToInsert);
    }

    if (key_playCount < root->playCount) {
        root->left = insertByPlayCount(root->left, songToInsert);
    }
    else if (key_playCount > root->playCount) {
        root->right = insertByPlayCount(root->right, songToInsert);
    }
    else {
        if (key_songId < root->songPtr->getSongId()) {
            root->left = insertByPlayCount(root->left, songToInsert);
        }
        else if (key_songId > root->songPtr->getSongId()) {
            root->right = insertByPlayCount(root->right, songToInsert);
        }
        else {
            return root;
        }
    }

    updateHeightPlayCount(root);
    int balance = balanceFactorPlayCount(root);

    if (balance > 1 && (key_playCount < root->left->playCount ||
                        (key_playCount == root->left->playCount &&
                         key_songId < root->left->songPtr->getSongId()))) {
        return rotateRightPlayCount(root);
    }

    if (balance < -1 && (key_playCount > root->right->playCount ||
                         (key_playCount == root->right->playCount &&
                          key_songId > root->right->songPtr->getSongId()))) {
        return rotateLeftPlayCount(root);
    }

    if (balance > 1 && (key_playCount > root->left->playCount ||
                        (key_playCount == root->left->playCount &&
                         key_songId > root->left->songPtr->getSongId()))) {
        root->left = rotateLeftPlayCount(root->left);
        return rotateRightPlayCount(root);
    }

    if (balance < -1 && (key_playCount < root->right->playCount ||
                         (key_playCount == root->right->playCount &&
                          key_songId < root->right->songPtr->getSongId()))) {
        root->right = rotateRightPlayCount(root->right);
        return rotateLeftPlayCount(root);
    }

    return root;
}

PlayCountNode *
Playlist::deleteByPlayCount(PlayCountNode *root, int targetPlayCount, int targetSongId) {
    if (root == nullptr) {
        return root;
    }

    if (targetPlayCount < root->playCount) {
        root->left = deleteByPlayCount(root->left, targetPlayCount, targetSongId);
    }
    else if (targetPlayCount > root->playCount) {
        root->right = deleteByPlayCount(root->right, targetPlayCount, targetSongId);
    }
    else {
        if (targetSongId < root->songPtr->getSongId()) {
            root->left = deleteByPlayCount(root->left, targetPlayCount, targetSongId);
        }
        else if (targetSongId > root->songPtr->getSongId()) {
            root->right = deleteByPlayCount(root->right, targetPlayCount, targetSongId);
        }
        else {
            if ((root->left == nullptr) || (root->right == nullptr)) {
                PlayCountNode *node_to_delete = root;
                root = (root->left ? root->left : root->right);
                delete node_to_delete;
            }
            else {
                PlayCountNode *temp = findMinPlayCount(root->right);
                root->playCount = temp->playCount;
                root->songPtr = temp->songPtr;
                root->right = deleteByPlayCount(root->right, temp->playCount,
                                                temp->songPtr->getSongId());
            }
        }
    }

    if (root == nullptr) {
        return root;
    }

    updateHeightPlayCount(root);
    int balance = balanceFactorPlayCount(root);

    if (balance > 1 && balanceFactorPlayCount(root->left) >= 0) {
        return rotateRightPlayCount(root);
    }
    if (balance > 1 && balanceFactorPlayCount(root->left) < 0) {
        root->left = rotateLeftPlayCount(root->left);
        return rotateRightPlayCount(root);
    }
    if (balance < -1 && balanceFactorPlayCount(root->right) <= 0) {
        return rotateLeftPlayCount(root);
    }
    if (balance < -1 && balanceFactorPlayCount(root->right) > 0) {
        root->right = rotateRightPlayCount(root->right);
        return rotateLeftPlayCount(root);
    }

    return root;
}

PlayCountNode *
Playlist::searchByPlayCount(PlayCountNode *root, int searchPlayCount, int searchSongId) const {
    if (root == nullptr) {
        return nullptr;
    }

    if (searchPlayCount < root->playCount) {
        return searchByPlayCount(root->left, searchPlayCount, searchSongId);
    }
    else if (searchPlayCount > root->playCount) {
        return searchByPlayCount(root->right, searchPlayCount, searchSongId);
    }
    else {
        if (searchSongId < root->songPtr->getSongId()) {
            return searchByPlayCount(root->left, searchPlayCount, searchSongId);
        }
        else if (searchSongId > root->songPtr->getSongId()) {
            return searchByPlayCount(root->right, searchPlayCount, searchSongId);
        }
        else {
            return root;
        }
    }
}

void Playlist::destroyPlayCountTree(PlayCountNode *rootNode) {
    if (rootNode) {
        destroyPlayCountTree(rootNode->left);
        destroyPlayCountTree(rootNode->right);
        delete rootNode;
    }
}

void Playlist::addSong(Song *song) {
    if (!song) {
        return;
    }
    appendToList(song);
    if (songListTail) {
        songsByIdTree->addSong(song, songListTail);
    }
    AVLPlayCount = insertByPlayCount(AVLPlayCount, song);
    numOfSongs++;
}

void Playlist::removeSong(int songId) {
    Song *song_to_remove = songsByIdTree->getSongById(songId);
    if (!song_to_remove) {
        return;
    }

    int old_playCount = song_to_remove->getCountPlayed();

    songsByIdTree->removeSong(songId);
    removeFromList(songId);
    AVLPlayCount = deleteByPlayCount(AVLPlayCount, old_playCount, songId);
    numOfSongs--;
}

void Playlist::playSong(int songId) {
    Song *song_to_play = songsByIdTree->getSongById(songId);
    if (song_to_play) {
        int old_playCount = song_to_play->getCountPlayed();
        AVLPlayCount = deleteByPlayCount(AVLPlayCount, old_playCount, songId); // שם המשתנה שונה

        song_to_play->increaseCountPlayed();

        AVLPlayCount = insertByPlayCount(AVLPlayCount, song_to_play); // שם המשתנה שונה
    }
}

Song *Playlist::getSongById(int song_id_search) const {
    return songsByIdTree->getSongById(song_id_search);
}

Song **Playlist::getAllSongs() const {
    int size = numOfSongs;
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

SongNodeList *Playlist::getSongNodeInList(int song_id_search) const {
    SongNodeList *current = songListHead;
    while (current) {
        if (current->songPtr && current->songPtr->getSongId() == song_id_search) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
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

void Playlist::removeFromList(int song_id_to_remove) {
    if (!songListHead) { return; }

    SongNodeList *current = songListHead;
    while (current != nullptr) {
        if (current->songPtr && current->songPtr->getSongId() == song_id_to_remove) {
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

    if (songListHead == nullptr) {
        songListTail = nullptr;
    }
}

void Playlist::destroyList(SongNodeList *head_node) {
    SongNodeList *current = songListHead;
    while (current) {
        SongNodeList *nextNode = current->next;
        delete current;
        current = nextNode;
    }
    songListHead = nullptr;
    songListTail = nullptr;
}
