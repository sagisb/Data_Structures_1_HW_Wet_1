#include "Song.h"
#include <algorithm>

struct AVLNodeSong {
public:
    int songId;
    Song* song_ptr;
    AVLNodeSong* left;
    AVLNodeSong* right;
    int height;

    AVLNodeSong(int songId, Song* s) : songId(songId), song_ptr(s), left(nullptr), right(nullptr), height(1) {}

private:
    int getHeight(AVLNodeSong* node) const {
        return (node == nullptr) ? 0 : node->height;
    }

    int getBalanceFactor(AVLNodeSong* node) const {
        return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
    }

    void updateHeight(AVLNodeSong* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

public:
    AVLNodeSong* rotateRight(AVLNodeSong* y) {
        AVLNodeSong* x = y->left;
        AVLNodeSong* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);

        return x;
    }

    AVLNodeSong* rotateLeft(AVLNodeSong* x) {
        AVLNodeSong* y = x->right;
        AVLNodeSong* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    AVLNodeSong* insert(AVLNodeSong* root, int key, Song* song) {
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

    AVLNodeSong* search(AVLNodeSong* root, int key) const {
        if (root == nullptr || root->songId == key) {
            return root;
        }

        if (key < root->songId) {
            return search(root->left, key);
        } else {
            return search(root->right, key);
        }
    }

    void destroyAVLTree(AVLNodeSong* root) {
        if (root) {
            destroyAVLTree(root->left);
            destroyAVLTree(root->right);
            delete root;
        }
    }
};