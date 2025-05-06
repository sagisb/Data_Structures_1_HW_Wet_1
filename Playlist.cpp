#include "Playlist.h"
#include <iostream>

PlayCountNode::PlayCountNode(int count, Song *s) :
        playCount(count), songPtr(s), left(nullptr), right(nullptr), height(1) {}

SongNodeList::SongNodeList(Song *s) : songPtr(s), next(nullptr), prev(nullptr) {}

Playlist::Playlist(int id) :
        playlistId(id),
        AVLPlayCount(nullptr),
        songListHead(nullptr),
        songListTail(nullptr) {
    songsByIdTree = new SongTreePlaylist();
}

Playlist::~Playlist() {
    destroyPlayCountTree(AVLPlayCount); // שם המשתנה שונה
    AVLPlayCount = nullptr;
    destroyList(songListHead);
    delete songsByIdTree;
    songsByIdTree = nullptr;
}

int Playlist::getPlaylistId() const {
    return playlistId;
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

PlayCountNode *Playlist::insertByPlayCount(PlayCountNode *root, Song *song_to_insert) {
    if (!song_to_insert) { return root; }

    int key_playCount = song_to_insert->getCountPlayed();
    int key_songId = song_to_insert->getSongId();

    if (root == nullptr) {
        return new PlayCountNode(key_playCount, song_to_insert);
    }

    if (key_playCount < root->playCount) {
        root->left = insertByPlayCount(root->left, song_to_insert);
    }
    else if (key_playCount > root->playCount) {
        root->right = insertByPlayCount(root->right, song_to_insert);
    }
    else {
        if (key_songId < root->songPtr->getSongId()) {
            root->left = insertByPlayCount(root->left, song_to_insert);
        }
        else if (key_songId > root->songPtr->getSongId()) {
            root->right = insertByPlayCount(root->right, song_to_insert);
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
Playlist::deleteByPlayCount(PlayCountNode *root, int target_playCount, int target_songId) {
    if (root == nullptr) {
        return root;
    }

    if (target_playCount < root->playCount) {
        root->left = deleteByPlayCount(root->left, target_playCount, target_songId);
    }
    else if (target_playCount > root->playCount) {
        root->right = deleteByPlayCount(root->right, target_playCount, target_songId);
    }
    else {
        if (target_songId < root->songPtr->getSongId()) {
            root->left = deleteByPlayCount(root->left, target_playCount, target_songId);
        }
        else if (target_songId > root->songPtr->getSongId()) {
            root->right = deleteByPlayCount(root->right, target_playCount, target_songId);
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
Playlist::searchByPlayCount(PlayCountNode *root, int search_playCount, int search_songId) const {
    if (root == nullptr) {
        return nullptr;
    }

    if (search_playCount < root->playCount) {
        return searchByPlayCount(root->left, search_playCount, search_songId);
    }
    else if (search_playCount > root->playCount) {
        return searchByPlayCount(root->right, search_playCount, search_songId);
    }
    else {
        if (search_songId < root->songPtr->getSongId()) {
            return searchByPlayCount(root->left, search_playCount, search_songId);
        }
        else if (search_songId > root->songPtr->getSongId()) {
            return searchByPlayCount(root->right, search_playCount, search_songId);
        }
        else {
            return root;
        }
    }
}

void Playlist::destroyPlayCountTree(PlayCountNode *root_node) {
    if (root_node) {
        destroyPlayCountTree(root_node->left);
        destroyPlayCountTree(root_node->right);
        delete root_node;
    }
}

void Playlist::addSong(Song *song) {
    if (!song) { return; }
    appendToList(song);
    if (songListTail) {
        songsByIdTree->addSong(song, songListTail);
    }
    AVLPlayCount = insertByPlayCount(AVLPlayCount, song); // שם המשתנה שונה
}

void Playlist::removeSong(int songId) {
    Song *song_to_remove = songsByIdTree->getSongById(songId);
    if (!song_to_remove) { return; }

    int old_playCount = song_to_remove->getCountPlayed();

    songsByIdTree->removeSong(songId);
    removeFromList(songId);
    AVLPlayCount = deleteByPlayCount(AVLPlayCount, old_playCount, songId); // שם המשתנה שונה
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

int Playlist::getListSize() const {
    int count = 0;
    SongNodeList *current = songListHead;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}