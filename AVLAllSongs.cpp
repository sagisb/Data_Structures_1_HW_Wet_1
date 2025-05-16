#include "AVLAllSongs.h"

AVLAllSongs::AVLAllSongs(int songId, Song *s) : songId(songId), song_ptr(s), left(nullptr),
                                                right(nullptr), height(1) {}

AVLAllSongs::AVLAllSongs(int songId, int played = 0) : songId(songId),
                                                       song_ptr(new Song(songId, played)),
                                                       left(nullptr),
                                                       right(nullptr),
                                                       height(1) {}

AVLAllSongs::~AVLAllSongs() {
    delete song_ptr;
    song_ptr = nullptr;
    delete left;
    left = nullptr;
    delete right;
    right = nullptr;
}

int AVLAllSongs::getHeight(AVLAllSongs *node) const {
    return (node == nullptr) ? 0 : node->height;
}

int AVLAllSongs::getBalanceFactor(AVLAllSongs *node) const {
    return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
}

void AVLAllSongs::updateHeight(AVLAllSongs *node) {
    if (node) {
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }
}

AVLAllSongs *AVLAllSongs::rotateRight(AVLAllSongs *y) {
    AVLAllSongs *x = y->left;
    AVLAllSongs *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

AVLAllSongs *AVLAllSongs::rotateLeft(AVLAllSongs *x) {
    AVLAllSongs *y = x->right;
    AVLAllSongs *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

AVLAllSongs *AVLAllSongs::insert(AVLAllSongs *root, int key, Song *song) {
    if (root == nullptr) {
        return new AVLAllSongs(key, song);
    }

    if (key < root->songId) {
        root->left = insert(root->left, key, song);
    } else if (key > root->songId) {
        root->right = insert(root->right, key, song);
    } else {
        return root;
    }

    updateHeight(root);

    int balance = getBalanceFactor(root);

    if (balance > 1 && key < root->left->songId) {
        return rotateRight(root);
    }

    if (balance < -1 && key > root->right->songId) {
        return rotateLeft(root);
    }

    if (balance > 1 && key > root->left->songId) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && key < root->right->songId) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

AVLAllSongs *AVLAllSongs::search(AVLAllSongs *root, int key) const {
    if (root == nullptr || root->songId == key) {
        return root;
    }
    if (key < root->songId) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

void AVLAllSongs::destroyAVLTree(AVLAllSongs *root) {
    if (root) {
        destroyAVLTree(root->left);
        destroyAVLTree(root->right);
        delete root;
    }
}

void AVLAllSongs::removeNode(int removedSongId) {

}
