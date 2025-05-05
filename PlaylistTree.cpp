#include "PlaylistTree.h"
#include <iostream>
#include <algorithm>

PlaylistTree::PlaylistTree() : root(nullptr) {}

PlaylistTree::~PlaylistTree() {
    destroyTree(root);
}

void PlaylistTree::destroyTree(AVLNodeSong* root) {
    if (root) {
        destroyTree(root->left);
        destroyTree(root->right);
        delete root;
    }
}

void PlaylistTree::addSong(int songId, Song* song) {
    root = insert(root, songId, song);
}

const Song* PlaylistTree::getSongById(int songId) const {
    AVLNodeSong* node = search(root, songId);
    return (node != nullptr) ? node->song_ptr : nullptr;
}

AVLNodeSong* PlaylistTree::insert(AVLNodeSong* root, int key, Song* song) {
    if (root == nullptr) {
        return new AVLNodeSong(key, song);
    }
    if (key < root->songId) {
        root->left = insert(root->left, key, song);
    } else if (key > root->songId) {
        root->right = insert(root->right, key, song);
    } else {
        return root;
    }
    root->height = 1 + std::max(height(root->left), height(root->right));
    int balance = balanceFactor(root);
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

AVLNodeSong* PlaylistTree::search(AVLNodeSong* root, int key) const {
    if (root == nullptr || root->songId == key) {
        return root;
    }
    if (key < root->songId) {
        return search(root->left, key);
    } else {
        return search(root->right, key);
    }
}

void PlaylistTree::removeSong(int songId) {
    root = deleteNode(root, songId);
}

AVLNodeSong* PlaylistTree::deleteNode(AVLNodeSong* root, int key) {
    if (root == nullptr) {
        return root;
    }
    if (key < root->songId) {
        root->left = deleteNode(root->left, key);
    } else if (key > root->songId) {
        root->right = deleteNode(root->right, key);
    } else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            AVLNodeSong* temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        } else {
            AVLNodeSong* temp = findMin(root->right);
            root->songId = temp->songId;
            root->song_ptr = temp->song_ptr;
            root->right = deleteNode(root->right, temp->songId);
        }
    }
    if (root == nullptr) {
        return root;
    }
    root->height = 1 + std::max(height(root->left), height(root->right));
    int balance = balanceFactor(root);
    if (balance > 1 && balanceFactor(root->left) >= 0) {
        return rotateRight(root);
    }
    if (balance > 1 && balanceFactor(root->left) < 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    if (balance < -1 && balanceFactor(root->right) <= 0) {
        return rotateLeft(root);
    }
    if (balance < -1 && balanceFactor(root->right) > 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}

AVLNodeSong* PlaylistTree::findMin(AVLNodeSong* node) const {
    AVLNodeSong* current = node;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

int PlaylistTree::height(AVLNodeSong* node) const {
    return (node == nullptr) ? 0 : node->height;
}

int PlaylistTree::balanceFactor(AVLNodeSong* node) const {
    return (node == nullptr) ? 0 : height(node->left) - height(node->right);
}

AVLNodeSong* PlaylistTree::rotateRight(AVLNodeSong* y) {
    AVLNodeSong* x = y->left;
    AVLNodeSong* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + std::max(height(y->left), height(y->right));
    x->height = 1 + std::max(height(x->left), height(x->right));
    return x;
}

AVLNodeSong* PlaylistTree::rotateLeft(AVLNodeSong* x) {
    AVLNodeSong* y = x->right;
    AVLNodeSong* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + std::max(height(x->left), height(x->right));
    y->height = 1 + std::max(height(y->left), height(y->right));
    return y;
}

void PlaylistTree::printInOrder() const {
    printInOrderHelper(root);
    std::cout << std::endl;
}

void PlaylistTree::printInOrderHelper(AVLNodeSong* root) const {
    if (root != nullptr) {
        printInOrderHelper(root->left);
        std::cout << root->songId << " ";
        printInOrderHelper(root->right);
    }
}