#include "Playlist.h"
#include "SongTreePlaylist.h"
#include <iostream>

PlaylistTreeNode::PlaylistTreeNode(Song *song, SongNodeList *node)
        : songId(song ? song->getSongId() : -1), songPtr(song), linkedListNodePtr(node), left(nullptr), right(nullptr),
          height(1) {}

SongTreePlaylist::SongTreePlaylist() : root(nullptr) {}

SongTreePlaylist::~SongTreePlaylist() {
    destroyTree(root);
}

void SongTreePlaylist::destroyTree(PlaylistTreeNode *current_root) {
    if (current_root) {
        destroyTree(current_root->left);
        destroyTree(current_root->right);
        delete current_root;
    }
}

int SongTreePlaylist::height(PlaylistTreeNode *node) const {
    return (node == nullptr) ? 0 : node->height;
}

int SongTreePlaylist::balanceFactor(PlaylistTreeNode *node) const {
    return (node == nullptr) ? 0 : height(node->left) - height(node->right);
}

void SongTreePlaylist::updateHeight(PlaylistTreeNode *node) {
    if (node) {
        int leftH = height(node->left);
        int rightH = height(node->right);
        node->height = 1 + (leftH > rightH ? leftH : rightH);
    }
}

PlaylistTreeNode *SongTreePlaylist::rotateLeft(PlaylistTreeNode *x) {
    // AVL property guarantees x->right exists when this is called
    PlaylistTreeNode *y = x->right;
    PlaylistTreeNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}

PlaylistTreeNode *SongTreePlaylist::rotateRight(PlaylistTreeNode *y) {
    // AVL property guarantees y->left exists when this is called
    PlaylistTreeNode *x = y->left;
    PlaylistTreeNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateHeight(x);

    return x;
}

PlaylistTreeNode *SongTreePlaylist::insert(PlaylistTreeNode *current_root, Song *song, SongNodeList *node) {
    if (current_root == nullptr) {
        return new PlaylistTreeNode(song, node);
    }

    if (song->getSongId() < current_root->songId) {
        current_root->left = insert(current_root->left, song, node);
    } else if (song->getSongId() > current_root->songId) {
        current_root->right = insert(current_root->right, song, node);
    } else {
        return current_root;
    }

    updateHeight(current_root);
    int balance = balanceFactor(current_root);

    // Check for left->left case (simple right rotation)
    if (balance > 1 && song->getSongId() < current_root->left->songId)
        return rotateRight(current_root);

    // Check for right->right case (simple left rotation)
    if (balance < -1 && song->getSongId() > current_root->right->songId)
        return rotateLeft(current_root);

    // Check for left->right case (left-right rotation)
    if (balance > 1 && song->getSongId() > current_root->left->songId) {
        current_root->left = rotateLeft(current_root->left);
        return rotateRight(current_root);
    }

    // Check for right->left case (right-left rotation)
    if (balance < -1 && song->getSongId() < current_root->right->songId) {
        current_root->right = rotateRight(current_root->right);
        return rotateLeft(current_root);
    }

    return current_root;
}

PlaylistTreeNode *SongTreePlaylist::search(PlaylistTreeNode *current_root, int search_songId) const {
    if (current_root == nullptr || current_root->songId == search_songId) {
        return current_root;
    }
    if (search_songId < current_root->songId) {
        return search(current_root->left, search_songId);
    } else {
        return search(current_root->right, search_songId);
    }
}

PlaylistTreeNode *SongTreePlaylist::deleteNode(PlaylistTreeNode *current_root, int song_id_to_delete) {
    if (current_root == nullptr)
        return current_root;

    if (song_id_to_delete < current_root->songId)
        current_root->left = deleteNode(current_root->left, song_id_to_delete);
    else if (song_id_to_delete > current_root->songId)
        current_root->right = deleteNode(current_root->right, song_id_to_delete);
    else {
        if ((current_root->left == nullptr) || (current_root->right == nullptr)) {
            PlaylistTreeNode *temp = current_root->left ? current_root->left : current_root->right;
            delete current_root;
            return temp;
        } else {
            PlaylistTreeNode *temp = findMin(current_root->right);
            current_root->songId = temp->songId;
            current_root->songPtr = temp->songPtr;
            current_root->linkedListNodePtr = temp->linkedListNodePtr;
            current_root->right = deleteNode(current_root->right, temp->songId);
        }
    }

    if (current_root == nullptr)
        return current_root;

    updateHeight(current_root);
    int balance = balanceFactor(current_root);

    // Left Left Case
    if (balance > 1 && balanceFactor(current_root->left) >= 0)
        return rotateRight(current_root);

    // Left Right Case
    if (balance > 1 && balanceFactor(current_root->left) < 0) {
        current_root->left = rotateLeft(current_root->left);
        return rotateRight(current_root);
    }

    // Right Right Case
    if (balance < -1 && balanceFactor(current_root->right) <= 0)
        return rotateLeft(current_root);

    // Right Left Case
    if (balance < -1 && balanceFactor(current_root->right) > 0) {
        current_root->right = rotateRight(current_root->right);
        return rotateLeft(current_root);
    }

    return current_root;
}

PlaylistTreeNode *SongTreePlaylist::findMin(PlaylistTreeNode *node) const {
    PlaylistTreeNode *current = node;
    while (current != nullptr && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

void SongTreePlaylist::addSong(Song *song, SongNodeList *node) {
    root = insert(root, song, node);
}

Song *SongTreePlaylist::getSongById(int search_songId) const {
    PlaylistTreeNode *node = search(root, search_songId);
    return node ? node->songPtr : nullptr;
}

SongNodeList *SongTreePlaylist::getSongNode(int search_songId) const {
    PlaylistTreeNode *node = search(root, search_songId);
    return node ? node->linkedListNodePtr : nullptr;
}

void SongTreePlaylist::removeSong(int song_id_to_delete) {
    root = deleteNode(root, song_id_to_delete);
}

void getLinkedListFromPlaylistTreeNode(PlaylistTreeNode *root, SongNodeList **listIterator) {
    if (root == nullptr) {
        return;
    }
    getLinkedListFromPlaylistTreeNode(root->left, listIterator);
    try {
        SongNodeList *currentSong = new SongNodeList(root->songPtr);
        if (listIterator) {
            (*listIterator)->next = currentSong;
        } else {
            (*listIterator) = currentSong;
        }
        (*listIterator) = (*listIterator)->next;
        getLinkedListFromPlaylistTreeNode(root->right, listIterator);
    }
    catch (const std::bad_alloc &exc) {
        delete listIterator;
        throw exc;
    }
}

void treeToList(PlaylistTreeNode *node,
                SongNodeList *&head,
                SongNodeList *&tail) {
    if (!node) return;

    treeToList(node->left, head, tail);

    SongNodeList *curr = new SongNodeList(node->songPtr);
    if (!head)
        head = curr;
    if (tail) {
        tail->next = curr;
        curr->prev = tail;
    }
    tail = curr;

    treeToList(node->right, head, tail);
}

SongNodeList *SongTreePlaylist::toLinkedList() {
    SongNodeList *head = nullptr;
    SongNodeList *tail = nullptr;
    try {
        treeToList(this->root, head, tail);
    }
    catch (...) {
        delete head;
        throw;
    }
    return head;
}

PlaylistTreeNode *SongTreePlaylist::getRoot() {
    return this->root;
}

// Function to create a perfectly balanced empty AVL tree with the specified number of nodes
PlaylistTreeNode *createAlmostEmptySongTree(int nodesAmount) {
    if (nodesAmount <= 0) {
        return nullptr;
    }

    // Create a new node
    PlaylistTreeNode *root = new PlaylistTreeNode(nullptr, nullptr);

    // Calculate how many nodes should go to each subtree for perfect balance
    int nodesRemainingAmount = nodesAmount - 1;
    int nodesToLeftAmount = (nodesRemainingAmount / 2) + (nodesRemainingAmount % 2);
    int nodesToRightAmount = (nodesRemainingAmount / 2);

    // Recursively create left and right subtrees
    root->left = createAlmostEmptySongTree(nodesToLeftAmount);
    root->right = createAlmostEmptySongTree(nodesToRightAmount);

    // Update height based on children's heights - critical for maintaining AVL property
    if (root->left && root->right) {
        int leftHeight = root->left->height;
        int rightHeight = root->right->height;
        root->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
    } else if (root->left) {
        root->height = 1 + root->left->height;
    } else if (root->right) {
        root->height = 1 + root->right->height;
    } else {
        root->height = 1;
    }

    return root;
}

SongTreePlaylist::SongTreePlaylist(int nodesAmount) {
    // Create an empty balanced tree structure
    this->root = createAlmostEmptySongTree(nodesAmount);
}

void populateSongsTree(PlaylistTreeNode *root, SongNodeList *&songsList, int height) {
    if (!root || !songsList) {
        return;
    }

    // Process left subtree first (in-order traversal)
    populateSongsTree(root->left, songsList, height + 1);

    // Process current node - set the song data
    root->songPtr = songsList->songPtr;
    root->songId = songsList->songPtr->getSongId();

    // Move to next song in the list
    songsList = songsList->next;

    // Process right subtree
    populateSongsTree(root->right, songsList, height + 1);

    // Calculate proper height based on children, not just the level
    // This ensures the height is correctly set for AVL balancing
    int leftHeight = root->left ? root->left->height : 0;
    int rightHeight = root->right ? root->right->height : 0;
    root->height = 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

void SongTreePlaylist::populateTree(SongNodeList *songsList) {
    SongNodeList *currentSong = songsList;
    populateSongsTree(this->root, currentSong, 0);
}
