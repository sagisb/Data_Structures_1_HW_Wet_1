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
        relevantPlaylist->playlist_ptr->addSong(relevantSong);


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

    Song *closestSongByPlays = relevantPlaylistNode->playlist_ptr->getSongByPlayCount(plays);
    if (!closestSongByPlays) {
        return StatusType::FAILURE;
    }

    return closestSongByPlays->getSongId();
}

// create a linked list of songs by id without duplicates
SongNodeList *mergeSongLinkedListsById(SongNodeList *firsList, SongNodeList *secondList) {
    SongNodeList *mergedList = new SongNodeList(
            nullptr); // initiate to dummy node to avoid annoying "if (mergedList)" checks
    SongNodeList *listIterator = mergedList;
    SongNodeList *firstListIterator = firsList;
    SongNodeList *secondListIterator = secondList;
    while (firstListIterator && secondListIterator) {
        if (firstListIterator->songPtr->getSongId() < secondListIterator->songPtr->getSongId()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            firstListIterator = firstListIterator->next;
        } else if (firstListIterator->songPtr->getSongId() > secondListIterator->songPtr->getSongId()) {
            listIterator->next = new SongNodeList(secondListIterator->songPtr);
            secondListIterator = secondListIterator->next;
        } else if (firstListIterator->songPtr->getSongId() == secondListIterator->songPtr->getSongId()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            firstListIterator = firstListIterator->next;
            secondListIterator = secondListIterator->next;
        }
        listIterator = listIterator->next;
    }

    while (firstListIterator) {
        listIterator->next = new SongNodeList(firstListIterator->songPtr);
        firstListIterator = firstListIterator->next;
        listIterator = listIterator->next;
    }

    while (secondListIterator) {
        listIterator->next = new SongNodeList(secondListIterator->songPtr);
        secondListIterator = secondListIterator->next;
        listIterator = listIterator->next;
    }

    mergedList = mergedList->next;
    SongNodeList *dummy = mergedList->prev;
    mergedList->prev = nullptr;
    delete dummy;
    return mergedList;
}

// TODO: I know this is a huge duplication with "mergeSongLinkedListsById" but the alternative is a make a generic tree class
// create a linked list of songs by id without duplicates
SongNodeList *mergeSongLinkedListsByPlayCount(SongNodeList *firsList, SongNodeList *secondList) {
    SongNodeList *mergedList = new SongNodeList(
            nullptr); // initiate to dummy node to avoid annoying "if (mergedList)" checks
    SongNodeList *listIterator = mergedList;
    SongNodeList *firstListIterator = firsList;
    SongNodeList *secondListIterator = secondList;
    while (firstListIterator && secondListIterator) {
        if (firstListIterator->songPtr->getCountPlayed() < secondListIterator->songPtr->getCountPlayed()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            firstListIterator = firstListIterator->next;
        } else if (firstListIterator->songPtr->getCountPlayed() > secondListIterator->songPtr->getCountPlayed()) {
            listIterator->next = new SongNodeList(secondListIterator->songPtr);
            secondListIterator = secondListIterator->next;
        } else if (firstListIterator->songPtr->getCountPlayed() == secondListIterator->songPtr->getCountPlayed()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            firstListIterator = firstListIterator->next;
            secondListIterator = secondListIterator->next;
        }
        listIterator = listIterator->next;
    }

    while (firstListIterator) {
        listIterator->next = new SongNodeList(firstListIterator->songPtr);
        firstListIterator = firstListIterator->next;
        listIterator = listIterator->next;
    }

    while (secondListIterator) {
        listIterator->next = new SongNodeList(secondListIterator->songPtr);
        secondListIterator = secondListIterator->next;
        listIterator = listIterator->next;
    }

    mergedList = mergedList->next;
    SongNodeList *dummy = mergedList->prev;
    mergedList->prev = nullptr;
    delete dummy;
    return mergedList;
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2) {
    if (playlistId1 <= 0 || playlistId2 <= 0 || playlistId1 == playlistId2) {
        return StatusType::INVALID_INPUT;
    }


    // find playlists
    AVLPlaylist *playlistOneNode = playlist->search(playlist, playlistId1);
    AVLPlaylist *playlistTwoNode = playlist->search(playlist, playlistId2);
    if (!playlistOneNode || !playlistTwoNode) { return StatusType::FAILURE; }

    Playlist *playlistOne = playlistOneNode->playlist_ptr;
    Playlist *playlistTwo = playlistTwoNode->playlist_ptr;

    // handle list one or two empty
    if (playlistTwo->getNumOfSongs() == 0) {
        return StatusType::SUCCESS;
    } else if (playlistOne->getNumOfSongs() == 0) {
        // set playlistOne to playlistTwo
        // TODO: finish
        playlistOne->setAVLPlayCount(playlistTwo->getAVLPlayCount());
        return StatusType::SUCCESS;
    }

    try {
        // ======================== songsByIdTree ========================
        // convert song trees to Song* arrays sorted by songId
        SongNodeList *playlistOneByIdLinkedList = playlistOne->getSongsByIdTree()->toLinkedList();
        SongNodeList *playlistTwoByIdLinkedList = playlistTwo->getSongsByIdTree()->toLinkedList();


        // merge arrays to one array
        SongNodeList *mergedSongsById = mergeSongLinkedListsById(playlistOneByIdLinkedList,
                                                                 playlistTwoByIdLinkedList);


        // create almost empty SongTreePlaylist Tree (with n1+n2 nodes)
        int newSongsAmount = 0;
        SongNodeList *mergedSongsIterator = mergedSongsById;
        while (mergedSongsIterator) {
            newSongsAmount++;
            mergedSongsIterator = mergedSongsIterator->next;

        }

        SongTreePlaylist *newSongsByIdTree = new SongTreePlaylist(newSongsAmount);

        // traverse Tree inorder and insert songs pointers
        newSongsByIdTree->populateTree(mergedSongsById);

        // ======================== songsByPlayCountTree ========================
        // TODO: !!! FIGURE OUT HOW TO REMOVE DUPLICATES - CONSIDER BUILDING THEM ALREADY SORTED BY ID (AFTER COUNT)!!!
        // convert song trees to Song* arrays sorted by countPlay
        SongNodeList *playlistOneByPlayCountLinkedList = playlistOne->getAVLPlayCount()->toLinkedList();
        SongNodeList *playlistTwoByPlayCountLinkedList = playlistTwo->getAVLPlayCount()->toLinkedList();

        // merge arrays to one array
        SongNodeList *mergedSongsByPlayCount = mergeSongLinkedListsByPlayCount(playlistOneByPlayCountLinkedList,
                                                                               playlistTwoByPlayCountLinkedList
        );


        // create almost empty SongTreePlaylist Tree (with n1+n2 nodes)
        PlayCountNode *newSongsByPlayCountTree = new PlayCountNode(newSongsAmount);


        // traverse Tree inorder and insert songs pointers
        newSongsByPlayCountTree->populateCountNodeTree(mergedSongsByPlayCount);

        // ======================== merge linked lists ========================
        // TODO: consider just creating a new list since in this methods, if a song is in 2 of the playlists it will have 2 nodes in the list
        playlistTwo->setListHead(mergedSongsById);
        // TODO: ADD TAIL SETTING, MAYBE RETURN A POINTER TO TAIL WHEN CREATING THE LIST

        // ======================== Set playlistOne properties to newly created objects ========================
        playlistOneNode->playlist_ptr->setSongsByIdTree(newSongsByIdTree);
        playlistOneNode->playlist_ptr->setAVLPlayCount(newSongsByPlayCountTree);
        playlistOneNode->playlist_ptr->setNumOfSongs(newSongsByPlayCountTree->height);
    }

    catch (const std::bad_alloc &exc) { // TODO: COMPLETE
        return StatusType::ALLOCATION_ERROR;
    }




    // TODO: DELETE
//    playListOne->getListTTail()->next = playlistTwo->getListHead();
//    playlistTwo->setListHead(nullptr);
//    playlistTwo->setListTail(nullptr);

    // ======================== remove playlistTwoNode and delete playlistTwo ========================
    this->playlist->deleteNode(this->playlist, playlistId2);


    return StatusType::SUCCESS;

}
