
#include "Song.h"

struct AVLNodeSong {
public:
    int songId;
    Song* song_ptr;
    AVLNodeSong* left;
    AVLNodeSong* right;
    int height;

    AVLNodeSong(int songId, Song* s);

    AVLNodeSong* rotateRight(AVLNodeSong* y);
    AVLNodeSong* rotateLeft(AVLNodeSong* x);
    AVLNodeSong* insert(AVLNodeSong* root, int key, Song* song);
    AVLNodeSong* search(AVLNodeSong* root, int key) const;
    void destroyAVLTree(AVLNodeSong* root);

private:
    int getHeight(AVLNodeSong* node) const;
    int getBalanceFactor(AVLNodeSong* node) const;
    void updateHeight(AVLNodeSong* node);
};
