#include "AVLPlaylist.h"

AVLPlaylist::AVLPlaylist(int id) : playlistId(id),
                                   playlist_ptr(new Playlist(id)),
                                   left(nullptr),
                                   right(nullptr),
                                   height(1) {}

AVLPlaylist::~AVLPlaylist() {
    delete playlist_ptr;
    playlist_ptr = nullptr;
    delete left;
    left = nullptr;
    delete right;
    right = nullptr;
}

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

AVLPlaylist *AVLPlaylist::insert(AVLPlaylist *current_root, int keyId) {
    if (current_root == nullptr) {
        return new AVLPlaylist(keyId);
    }

    if (keyId < current_root->playlistId) {
        current_root->left = insert(current_root->left, keyId);
    } else if (keyId > current_root->playlistId) {
        current_root->right = insert(current_root->right, keyId);
    } else {
        return current_root;
    }

    updateHeight(current_root);

    int balance = getBalanceFactor(current_root);

    if (balance > 1 && keyId < current_root->left->playlistId) {
        return rotateRight(current_root);
    }

    if (balance < -1 && keyId > current_root->right->playlistId) {
        return rotateLeft(current_root);
    }

    if (balance > 1 && keyId > current_root->left->playlistId) {
        current_root->left = rotateLeft(current_root->left);
        return rotateRight(current_root);
    }

    if (balance < -1 && keyId < current_root->right->playlistId) {
        current_root->right = rotateRight(current_root->right);
        return rotateLeft(current_root);
    }

    return current_root;
}

AVLPlaylist *AVLPlaylist::search(AVLPlaylist *root, int key) const {
    if (root == nullptr || root->playlistId == key) {
        return root;
    }

    if (key < root->playlistId) {
        return search(root->left, key);
    } else {
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

bool AVLPlaylist::playlistExists(AVLPlaylist *current_root, int searchId) const {
    if (search(current_root, searchId) != nullptr) {
        return true;
    }
    return false;
}

AVLPlaylist *AVLPlaylist::deleteNode(AVLPlaylist *root, int idToDelete) {
    if (root == nullptr)
        return nullptr;

    if (idToDelete < root->playlistId) {
        root->left = deleteNode(root->left, idToDelete);
    } else if (idToDelete > root->playlistId) {
        root->right = deleteNode(root->right, idToDelete);
    } else {
        // Found the node to delete
        if (root->left == nullptr || root->right == nullptr) {
            AVLPlaylist *child = root->left ? root->left : root->right;

            // Disconnect children to prevent recursive deletion
            root->left = nullptr;
            root->right = nullptr;

            // Free this node
            delete root;
            return child; // may be nullptr
        } else {
            // Two children — replace with in-order successor
            AVLPlaylist *successor = getMinValueNode(root->right);

            // Move successor's data into current node
            root->playlistId = successor->playlistId;

            // Delete the current playlist and take over the successor's one
            delete root->playlist_ptr;
            root->playlist_ptr = successor->playlist_ptr;
            successor->playlist_ptr = nullptr; // avoid double free

            // Delete the duplicate node from the right subtree
            root->right = deleteNode(root->right, successor->playlistId);
        }
    }

    // Update height and rebalance
    updateHeight(root);
    int balance = getBalanceFactor(root);

    // Left-heavy
    if (balance > 1) {
        if (getBalanceFactor(root->left) >= 0) {
            return rotateRight(root);
        } else {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
    }

    // Right-heavy
    if (balance < -1) {
        if (getBalanceFactor(root->right) <= 0) {
            return rotateLeft(root);
        } else {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
    }

    return root;
}

AVLPlaylist *AVLPlaylist::getMinValueNode(AVLPlaylist *node) {
    AVLPlaylist *current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

void AVLPlaylist::setPlaylist(Playlist *newPlaylistPtr) {
    delete this->playlist_ptr;
    this->playlist_ptr = newPlaylistPtr;
}
