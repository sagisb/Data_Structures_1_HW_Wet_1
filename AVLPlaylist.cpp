#include "AVLPlaylist.h"

AVLPlaylist::AVLPlaylist(int id) :
        playlistId(id),
        playlist_ptr(new Playlist(id)),
        left(nullptr),
        right(nullptr),
        height(1) {}

AVLPlaylist::~AVLPlaylist() {
    delete playlist_ptr;
    playlist_ptr = nullptr;
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
    }
    else if (keyId > current_root->playlistId) {
        current_root->right = insert(current_root->right, keyId);
    }
    else {
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

bool AVLPlaylist::playlistExists(AVLPlaylist *current_root, int searchId) const {
    if (search(current_root, searchId) != nullptr) {
        return true;
    }
    return false;
}

AVLPlaylist *AVLPlaylist::deleteNode(AVLPlaylist *current_root, int IdToDelete) {
    if (current_root == nullptr) {
        return current_root;
    }

    if (IdToDelete < current_root->playlistId) {
        current_root->left = deleteNode(current_root->left, IdToDelete);
    }
    else if (IdToDelete > current_root->playlistId) {
        current_root->right = deleteNode(current_root->right, IdToDelete);
    }
    else {
        if (current_root->left == nullptr || current_root->right == nullptr) {
            AVLPlaylist *temp = current_root->left ? current_root->left : current_root->right;

            if (temp == nullptr) {
                temp = current_root;
                current_root = nullptr;
            }
            else {
                AVLPlaylist *node_to_free = current_root;
                current_root = temp;
                temp = node_to_free;
            }

            delete temp;
        }
        else {
            AVLPlaylist *successor = getMinValueNode(current_root->right);

            current_root->playlistId = successor->playlistId;


            delete current_root->playlist_ptr;
            current_root->playlist_ptr = successor->playlist_ptr;
            successor->playlist_ptr = nullptr;

            current_root->right = deleteNode(current_root->right, successor->playlistId);
        }
    }

    if (current_root == nullptr) {
        return current_root;
    }

    updateHeight(current_root);

    int balance = getBalanceFactor(current_root);


    if (balance > 1 && getBalanceFactor(current_root->left) >= 0) {
        return rotateRight(current_root);
    }

    if (balance > 1 && getBalanceFactor(current_root->left) < 0) {
        current_root->left = rotateLeft(current_root->left);
        return rotateRight(current_root);
    }

    if (balance < -1 && getBalanceFactor(current_root->right) <= 0) {
        return rotateLeft(current_root);
    }

    if (balance < -1 && getBalanceFactor(current_root->right) > 0) {
        current_root->right = rotateRight(current_root->right);
        return rotateLeft(current_root);
    }

    return current_root;
}

AVLPlaylist* AVLPlaylist::getMinValueNode(AVLPlaylist* node) {
    AVLPlaylist* current = node;
    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}