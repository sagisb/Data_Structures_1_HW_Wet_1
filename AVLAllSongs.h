#include "Song.h"

struct AVLAllSongs {
public:
    int songId;
    Song *song_ptr;
    AVLAllSongs *left;
    AVLAllSongs *right;
    int height;

    AVLAllSongs(int songId, Song *s);

    AVLAllSongs *rotateRight(AVLAllSongs *y);
    AVLAllSongs *rotateLeft(AVLAllSongs *x);
    AVLAllSongs *insert(AVLAllSongs *root, int key, Song *song);
    AVLAllSongs *search(AVLAllSongs *root, int key) const;
    void destroyAVLTree(AVLAllSongs *root);

private:
    int getHeight(AVLAllSongs *node) const;
    int getBalanceFactor(AVLAllSongs *node) const;
    void updateHeight(AVLAllSongs *node);
};
