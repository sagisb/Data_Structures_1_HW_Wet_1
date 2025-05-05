#include "SongTreePlaylist.h"
#include <algorithm>
#include <iostream>

PlaylistTreeNode::PlaylistTreeNode(Song* song, SongNodeList* node)
        : songId(song->getSongId()), songPtr(song), linkedListNodePtr(node), left(nullptr), right(nullptr), height(1) {}

SongTreePlaylist::SongTreePlaylist() : root(nullptr) {}

SongTreePlaylist::~SongTreePlaylist() {
    destroyTree(root);
}

void SongTreePlaylist::destroyTree(PlaylistTreeNode* root) {
    if (root) {
        destroyTree(root->left);
        destroyTree(root->right);
        delete root;
    }
}

int SongTreePlaylist::height(PlaylistTreeNode* node) const {
    return (node == nullptr) ? 0 : node->height;
}

int SongTreePlaylist::balanceFactor(PlaylistTreeNode* node) const {
    return (node == nullptr) ? 0 : height(node->left) - height(node->right);
}

void SongTreePlaylist::updateHeight(PlaylistTreeNode* node) {
    if (node) {
        node->height = 1 + std::max(height(node->left), height(node->right));
    }
}

PlaylistTreeNode* SongTreePlaylist::rotateLeft(PlaylistTreeNode* x) {
    PlaylistTreeNode* y = x->right;
    PlaylistTreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

PlaylistTreeNode* SongTreePlaylist::rotateRight(PlaylistTreeNode* y) {
    PlaylistTreeNode* x = y->left;
    PlaylistTreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

PlaylistTreeNode* SongTreePlaylist::insert(PlaylistTreeNode* root, Song* song, SongNodeList* node) {
    if (root == nullptr) {
        return new PlaylistTreeNode(song, node);
    }

    if (song->getSongId() < root->songId) {
        root->left = insert(root->left, song, node);
    } else if (song->getSongId() > root->songId) {
        root->right = insert(root->right, song, node);
    } else {
        return root; // Duplicate keys not allowed
    }

    updateHeight(root);
    int balance = balanceFactor(root);

    // Left Left Case
    if (balance > 1 && song->getSongId() < root->left->songId)
        return rotateRight(root);

    // Right Right Case
    if (balance < -1 && song->getSongId() > root->right->songId)
        return rotateLeft(root);

    // Left Right Case
    if (balance > 1 && song->getSongId() > root->left->songId) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right Left Case
    if (balance < -1 && song->getSongId() < root->right->songId) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

PlaylistTreeNode* SongTreePlaylist::search(PlaylistTreeNode* root, int songId) const {
    if (root == nullptr || root->songId == songId) {
        return root;
    }
    if (songId < root->songId) {
        return search(root->left, songId);
    } else {
        return search(root->right, songId);
    }
}

PlaylistTreeNode* SongTreePlaylist::deleteNode(PlaylistTreeNode* root, int songId) {
    if (root == nullptr)
        return root;

    if (songId < root->songId)
        root->left = deleteNode(root->left, songId);
    else if (songId > root->songId)
        root->right = deleteNode(root->right, songId);
    else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            PlaylistTreeNode* temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        } else {
            PlaylistTreeNode* temp = findMin(root->right);
            root->songId = temp->songId;
            root->songPtr = temp->songPtr;
            root->linkedListNodePtr = temp->linkedListNodePtr;
            root->right = deleteNode(root->right, temp->songId);
        }
    }

    if (root == nullptr)
        return root;

    updateHeight(root);
    int balance = balanceFactor(root);

    // Left Left Case
    if (balance > 1 && balanceFactor(root->left) >= 0)
        return rotateRight(root);

    // Left Right Case
    if (balance > 1 && balanceFactor(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right Right Case
    if (balance < -1 && balanceFactor(root->right) <= 0)
        return rotateLeft(root);

    // Right Left Case
    if (balance < -1 && balanceFactor(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

PlaylistTreeNode* SongTreePlaylist::findMin(PlaylistTreeNode* node) const {
    PlaylistTreeNode* current = node;
    while (current->left != nullptr)
        current = current->left;
    return current;
}

void SongTreePlaylist::addSong(Song* song, SongNodeList* node) {
    root = insert(root, song, node);
}

Song* SongTreePlaylist::getSongById(int songId) const {
    PlaylistTreeNode* node = search(root, songId);
    return node ? node->songPtr : nullptr;
}

SongNodeList* SongTreePlaylist::getSongNode(int songId) const {
    PlaylistTreeNode* node = search(root, songId);
    return node ? node->linkedListNodePtr : nullptr;
}

void SongTreePlaylist::removeSong(int songId) {
    root = deleteNode(root, songId);
}