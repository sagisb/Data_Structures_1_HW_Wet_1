#include "Playlist.h"

struct AVLPlaylist {
public:
    int playlistId;
    Playlist *playlist_ptr;
    AVLPlaylist *left;
    AVLPlaylist *right;
    int height;

    explicit AVLPlaylist(int playlistId);
    ~AVLPlaylist();

    AVLPlaylist *rotateRight(AVLPlaylist *y);
    AVLPlaylist *rotateLeft(AVLPlaylist *x);
    AVLPlaylist *insert(AVLPlaylist *currentRoot, int keyId);
    AVLPlaylist *search(AVLPlaylist *root, int key) const;
    //void destroyAVLTree(AVLPlaylist *root);

private:
    int getHeight(AVLPlaylist *node) const;
    int getBalanceFactor(AVLPlaylist *node) const;
    void updateHeight(AVLPlaylist *node);
};
