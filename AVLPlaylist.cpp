#include "AVLPlaylist.h"

AVLPlaylist::AVLPlaylist(int PlaylistId, Playlist *s) : playlistId(PlaylistId), playlist_ptr(s),
                                                        left(nullptr),
                                                        right(nullptr), height(1) {}

int AVLPlaylist::getHeight(AVLPlaylist *node) const {
    return (node == nullptr) ? 0 : node->height;
}

int AVLPlaylist::getBalanceFactor(AVLPlaylist *node) const {
    return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
}

void AVLPlaylist::updateHeight(AVLPlaylist *node) {
    if (node) {
        int leftHeight = getHeight(node->left);
        int rightHeight = getHeight(node->right);
        node->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    }
}

AVLPlaylist *AVLPlaylist::rotateRight(AVLPlaylist *y) {
    AVLPlaylist *x = y->left;
    AVLPlaylist *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

AVLPlaylist *AVLPlaylist::rotateLeft(AVLPlaylist *x) {
    AVLPlaylist *y = x->right;
    AVLPlaylist *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

AVLPlaylist *AVLPlaylist::insert(AVLPlaylist *root, int key, Playlist *playlist) {
    if (root == nullptr) {
        return new AVLPlaylist(key, playlist);
    }

    if (key < root->playlistId) {
        root->left = insert(root->left, key, playlist);
    }
    else if (key > root->playlistId) {
        root->right = insert(root->right, key, playlist);
    }
    else {
        return root;
    }

    updateHeight(root);

    int balance = getBalanceFactor(root);

    if (balance > 1 && key < root->left->playlistId) {
        return rotateRight(root);
    }

    if (balance < -1 && key > root->right->playlistId) {
        return rotateLeft(root);
    }

    if (balance > 1 && key > root->left->playlistId) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    if (balance < -1 && key < root->right->playlistId) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

AVLPlaylist *AVLPlaylist::search(AVLPlaylist *root, int key) const {
    if (root == nullptr || root->playlistId == key) {
        return root;
    }

    if (key < root->playlistId) {
        return search(root->left, key);
    }
    else {
        return search(root->right, key);
    }
}

void AVLPlaylist::destroyAVLTree(AVLPlaylist *root) {
    if (root) {
        destroyAVLTree(root->left);
        destroyAVLTree(root->right);
        delete root;
    }
}