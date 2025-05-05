
#include "AVLNodeSong.h"

class PlaylistTree {
private:
    AVLNodeSong* root;

public:
    PlaylistTree();
    ~PlaylistTree();

    void addSong(int songId, Song* song);
    const Song* getSongById(int songId) const;
    void removeSong(int songId);
    void printInOrder() const;

private:
    AVLNodeSong* insert(AVLNodeSong* root, int key, Song* song);
    AVLNodeSong* search(AVLNodeSong* root, int key) const;
    AVLNodeSong* deleteNode(AVLNodeSong* root, int key);
    AVLNodeSong* findMin(AVLNodeSong* node) const;
    void destroyTree(AVLNodeSong* root);
    void printInOrderHelper(AVLNodeSong* root) const;
};
