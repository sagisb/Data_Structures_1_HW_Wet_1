// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b1.h"

const int PLAYS_NUM_DEFAULT_VALUE = 0;

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
        } else {
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
        } else {
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
    if (songId <= 0 || playlistId < 0 || this->playlist == nullptr
        || this->songs == nullptr
        || this->songs->search(this->songs, songId) == nullptr
        || this->playlist->search(this->playlist, playlistId) ==
           nullptr) { // TODO: make checks for null values later to avoid time addition for duplicated operations
        return StatusType::INVALID_INPUT;
    }

    try {
        Song *relevantSong = this->songs->search(this->songs, songId)->song_ptr;
        AVLPlaylist *relevantPlaylist = this->playlist->search(this->playlist, playlistId);
        relevantPlaylist->insert(this->playlist, relevantSong->getSongId());
        relevantSong->increaseCountPlayed();
    }
    catch (const std::bad_alloc &a) {
        return StatusType::ALLOCATION_ERROR;
    }
    return StatusType::SUCCESS;
}

StatusType DSpotify::delete_song(int songId) {
    if (songId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    AVLAllSongs *songToRemove = this->songs->search(this->songs, songId);
    if (songToRemove == nullptr || songToRemove->song_ptr->getCountPlayed() > 0) {
        return StatusType::FAILURE;
    }
    this->songs->removeNode(songId); // TODO: implement
    return StatusType::SUCCESS;
}

StatusType DSpotify::remove_from_playlist(int playlistId, int songId) {
    if (songId <= 0 || playlistId <= 0) {
        return StatusType::INVALID_INPUT;
    }
    AVLPlaylist *relevantPlaylist = this->playlist->search(this->playlist, playlistId);
    if (!relevantPlaylist) {
        return StatusType::FAILURE;
    }
    Song *relevantSong = relevantPlaylist->playlist_ptr->getSongById(songId);
    if (!relevantSong) {
        return StatusType::FAILURE;
    }

    relevantPlaylist->playlist_ptr->removeSong(songId);
    relevantSong->decrementingSongFromPlaylist();
    return StatusType::SUCCESS;
}

output_t<int> DSpotify::get_plays(int songId) {
    if (songId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    AVLAllSongs *relevantSong = this->songs->search(this->songs, songId);
    if (!relevantSong) {
        return StatusType::FAILURE;
    }

    return relevantSong->song_ptr->getCountPlayed();
}

output_t<int> DSpotify::get_num_songs(int playlistId) {
    if (playlistId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    AVLPlaylist *relevantPlaylist = this->playlist->search(this->playlist, playlistId);
    if (!relevantPlaylist) {
        return StatusType::FAILURE;
    }

    return relevantPlaylist->playlist_ptr->getNumOfSongs();
}

output_t<int> DSpotify::get_by_plays(int playlistId, int plays) {
    if (playlistId <= 0 || plays < 0) {
        return StatusType::INVALID_INPUT;
    }
    AVLPlaylist *relevantPlaylistNode = this->playlist->search(this->playlist, playlistId);
    if (!relevantPlaylistNode) {
        return StatusType::FAILURE;
    }

    Song *closestSongByPlays = relevantPlaylistNode->playlist_ptr->getSongByPlayCount(plays); // TODO: implement
    if (!closestSongByPlays) {
        return StatusType::FAILURE;
    }

    return closestSongByPlays->getSongId();
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2) {
    return StatusType::FAILURE;
}
