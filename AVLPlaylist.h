#include "Playlist.h"

struct AVLPlaylist {
public:
    int playlistId;
    Playlist *playlist_ptr;
    AVLPlaylist *left;
    AVLPlaylist *right;
    int height;

    AVLPlaylist(int songId, Song *s);

    AVLPlaylist *rotateRight(AVLPlaylist *y);
    AVLPlaylist *rotateLeft(AVLPlaylist *x);
    AVLPlaylist *insert(AVLPlaylist *root, int key, Song *song);
    AVLPlaylist *search(AVLPlaylist *root, int key) const;
    void destroyAVLTree(AVLPlaylist *root);

private:
    int getHeight(AVLPlaylist *node) const;
    int getBalanceFactor(AVLPlaylist *node) const;
    void updateHeight(AVLPlaylist *node);
};
