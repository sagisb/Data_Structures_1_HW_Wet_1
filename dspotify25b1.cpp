// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b1.h"


DSpotify::DSpotify() : playlist(nullptr), songs(nullptr) {}

DSpotify::~DSpotify() {
    delete playlist;
    playlist = nullptr;
    delete songs;
    songs = nullptr;
}

StatusType DSpotify::add_playlist(int playlistId) {
    if (playlistId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (playlist != nullptr) {
        if (playlist->playlistExists(playlist, playlistId)) {
            return StatusType::FAILURE;
        }
    }
    try {
        if (playlist == nullptr) {
            playlist = new AVLPlaylist(playlistId);
        }
        else {
            playlist = playlist->insert(playlist, playlistId);
        }
        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &a) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::delete_playlist(int playlistId) {
    if (playlistId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    if (playlist == nullptr) {
        return StatusType::FAILURE;
    }
    AVLPlaylist *nodeToDelete = playlist->search(playlist, playlistId);
    if (nodeToDelete == nullptr) {
        return StatusType::FAILURE;
    }
    if (nodeToDelete->playlist_ptr->getNumOfSongs() > 0) {
        return StatusType::FAILURE;
    }
    try {
        playlist = playlist->deleteNode(playlist, playlistId);

        return StatusType::SUCCESS;

    }
    catch (const std::bad_alloc &a) {
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::add_song(int songId, int plays) {
    if (songId <= 0 || plays < 0) {
        return StatusType::INVALID_INPUT;
    }
    if (songs != nullptr && songs->search(songs, songId) != nullptr) {
        return StatusType::FAILURE;
    }
    Song *newSongObject = nullptr;
    try {
        newSongObject = new Song(songId, plays);
        if (this->songs == nullptr) {
            this->songs = new AVLAllSongs(songId, newSongObject);
        }
        else {
            this->songs = this->songs->insert(this->songs, songId, newSongObject);
        }
        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &a) {
        delete newSongObject;
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::add_to_playlist(int playlistId, int songId) {
    return StatusType::FAILURE;
}

StatusType DSpotify::delete_song(int songId) {
    return StatusType::FAILURE;
}

StatusType DSpotify::remove_from_playlist(int playlistId, int songId) {
    return StatusType::FAILURE;
}

output_t<int> DSpotify::get_plays(int songId) {
    return 0;
}

output_t<int> DSpotify::get_num_songs(int playlistId) {
    return 0;
}

output_t<int> DSpotify::get_by_plays(int playlistId, int plays) {
    return 0;
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2) {
    return StatusType::FAILURE;
}
