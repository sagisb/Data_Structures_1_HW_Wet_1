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
    if (songId <= 0 || playlistId < 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!this->songs || !this->playlist) {
        return StatusType::FAILURE;
    }

    try {
        AVLAllSongs *relevantSongNode = this->songs->search(this->songs, songId);
        AVLPlaylist *relevantPlaylist = this->playlist->search(this->playlist, playlistId);

        if (!relevantSongNode || !relevantPlaylist || relevantPlaylist->playlist_ptr->getSongById(songId)) {
            return StatusType::FAILURE;
        }

        relevantPlaylist->playlist_ptr->addSong(relevantSongNode->song_ptr);
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

    if (!this->songs) {
        return StatusType::FAILURE;
    }
    AVLAllSongs *songNodeInTree = this->songs->search(this->songs, songId);
    if (!songNodeInTree || (songNodeInTree->song_ptr && songNodeInTree->song_ptr->getCountPlaylist() > 0)) {
        return StatusType::FAILURE;
    }

    this->songs = this->songs->deleteNode(this->songs, songId); // deleteNode is now public/wrapper
    // and it correctly returns the new root
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
            listIterator->next->prev = listIterator; // Set prev pointer
            firstListIterator = firstListIterator->next;
        } else if (firstListIterator->songPtr->getSongId() > secondListIterator->songPtr->getSongId()) {
            listIterator->next = new SongNodeList(secondListIterator->songPtr);
            listIterator->next->prev = listIterator; // Set prev pointer
            secondListIterator = secondListIterator->next;
        } else if (firstListIterator->songPtr->getSongId() == secondListIterator->songPtr->getSongId()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            listIterator->next->prev = listIterator; // Set prev pointer
            firstListIterator = firstListIterator->next;
            secondListIterator = secondListIterator->next;
        }
        listIterator = listIterator->next;
    }

    while (firstListIterator) {
        listIterator->next = new SongNodeList(firstListIterator->songPtr);
        listIterator->next->prev = listIterator; // Set prev pointer
        firstListIterator = firstListIterator->next;
        listIterator = listIterator->next;
    }

    while (secondListIterator) {
        listIterator->next = new SongNodeList(secondListIterator->songPtr);
        listIterator->next->prev = listIterator; // Set prev pointer
        secondListIterator = secondListIterator->next;
        listIterator = listIterator->next;
    }

    mergedList = mergedList->next;
    SongNodeList *dummy = mergedList->prev;
    mergedList->prev = nullptr;
    delete dummy;
    return mergedList;
}

// create a linked list of songs sorted by play count, and then song ID
SongNodeList *mergeSongLinkedListsByPlayCount(SongNodeList *firsList, SongNodeList *secondList) {
    // Handle null cases
    if (!firsList)
        return secondList;
    if (!secondList)
        return firsList;

    SongNodeList *mergedList = new SongNodeList(nullptr); // dummy head node
    SongNodeList *listIterator = mergedList;
    SongNodeList *firstListIterator = firsList;
    SongNodeList *secondListIterator = secondList;

    // Merge the two lists
    while (firstListIterator && secondListIterator) {
        if (firstListIterator->songPtr->getCountPlayed() < secondListIterator->songPtr->getCountPlayed()) {
            listIterator->next = new SongNodeList(firstListIterator->songPtr);
            firstListIterator = firstListIterator->next;
        } else if (firstListIterator->songPtr->getCountPlayed() > secondListIterator->songPtr->getCountPlayed()) {
            listIterator->next = new SongNodeList(secondListIterator->songPtr);
            secondListIterator = secondListIterator->next;
        } else { // Equal play counts, sort by song ID
            if (firstListIterator->songPtr->getSongId() == secondListIterator->songPtr->getSongId()) {
                // Add one song and advance both pointers
                listIterator->next = new SongNodeList(firstListIterator->songPtr);
                firstListIterator = firstListIterator->next;
                secondListIterator = secondListIterator->next;
            } else if (firstListIterator->songPtr->getSongId() < secondListIterator->songPtr->getSongId()) {
                listIterator->next = new SongNodeList(firstListIterator->songPtr);
                firstListIterator = firstListIterator->next;
            } else {
                listIterator->next = new SongNodeList(secondListIterator->songPtr);
                secondListIterator = secondListIterator->next;
            }
        }

        if (listIterator->next) {
            listIterator->next->prev = listIterator;
            listIterator = listIterator->next;
        }
    }

    // Add remaining elements from first list
    while (firstListIterator) {
        listIterator->next = new SongNodeList(firstListIterator->songPtr);
        if (listIterator->next) {
            listIterator->next->prev = listIterator;
            listIterator = listIterator->next;
        }
        firstListIterator = firstListIterator->next;
    }

    // Add remaining elements from second list
    while (secondListIterator) {
        listIterator->next = new SongNodeList(secondListIterator->songPtr);
        if (listIterator->next) {
            listIterator->next->prev = listIterator;
            listIterator = listIterator->next;
        }
        secondListIterator = secondListIterator->next;
    }

    // Remove the dummy head
    SongNodeList *result = mergedList->next;
    if (result) {
        result->prev = nullptr;
    }
    delete mergedList;

    return result;
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2) {
    if (playlistId1 <= 0 || playlistId2 <= 0 || playlistId1 == playlistId2) {
        return StatusType::INVALID_INPUT;
    }

    // Find playlists
    AVLPlaylist *playlistOneNode = playlist->search(playlist, playlistId1);
    AVLPlaylist *playlistTwoNode = playlist->search(playlist, playlistId2);
    if (!playlistOneNode || !playlistTwoNode) {
        return StatusType::FAILURE;
    }

    Playlist *playlistOne = playlistOneNode->playlist_ptr;
    Playlist *playlistTwo = playlistTwoNode->playlist_ptr;

    // Handle empty playlists
    if (playlistTwo->getNumOfSongs() == 0) {
        // Nothing to merge, playlist2 is empty
        this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);
        return StatusType::SUCCESS;
    } else if (playlistOne->getNumOfSongs() == 0) {
        // If playlist1 is empty, just copy all data structures from playlist2
        try {
            // Create a deep copy of playlist2's song tree
            SongNodeList *playlist2Songs = playlistTwo->getSongsByIdTree()->toLinkedList();
            SongTreePlaylist *newTree = new SongTreePlaylist(playlistTwo->getNumOfSongs());
            newTree->populateTree(playlist2Songs);

            // Create a deep copy of playlist2's play count tree
            SongNodeList *playlist2SongsByCount = playlistTwo->getAVLPlayCount()->toLinkedList();
            PlayCountNode *newCountTree = new PlayCountNode(playlistTwo->getNumOfSongs());
            // Make a copy of the pointer since it'll be modified by populateCountNodeTree
            SongNodeList *tempCountList = playlist2SongsByCount;
            newCountTree->populateCountNodeTree(tempCountList);

            // Set playlist1's data structures
            playlistOne->setSongsByIdTree(newTree);
            playlistOne->setAVLPlayCount(newCountTree);
            playlistOne->setNumOfSongs(playlistTwo->getNumOfSongs());

            // Create a linked list for playlist1
            SongNodeList *head = nullptr;
            SongNodeList *tail = nullptr;
            SongNodeList *current = playlist2Songs;

            while (current) {
                SongNodeList *newNode = new SongNodeList(current->songPtr);
                if (!head) {
                    head = tail = newNode;
                } else {
                    tail->next = newNode;
                    newNode->prev = tail;
                    tail = newNode;
                }
                current = current->next;
            }

            // Set the linked list head and tail
            playlistOne->setListHead(head);
            playlistOne->setListTail(tail);

            // Clean up temporary lists
            current = playlist2Songs;
            while (current) {
                SongNodeList *next = current->next;
                delete current;
                current = next;
            }

            current = playlist2SongsByCount;
            while (current) {
                SongNodeList *next = current->next;
                delete current;
                current = next;
            }

            // Remove playlist2
            this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);
            return StatusType::SUCCESS;
        }
        catch (const std::bad_alloc &exc) {
            return StatusType::ALLOCATION_ERROR;
        }
    }

    // Both playlists have songs - need to merge
    try {
        // Convert song trees to linked lists
        SongNodeList *playlistOneByIdLinkedList = nullptr;
        SongNodeList *playlistTwoByIdLinkedList = nullptr;
        SongNodeList *playlistOneByPlayCountLinkedList = nullptr;
        SongNodeList *playlistTwoByPlayCountLinkedList = nullptr;
        SongNodeList *mergedSongsById = nullptr;
        SongNodeList *mergedSongsByPlayCount = nullptr;
        SongTreePlaylist *newSongsByIdTree = nullptr;
        PlayCountNode *newSongsByPlayCountTree = nullptr;

        // Get songs from both playlists
        playlistOneByIdLinkedList = playlistOne->getSongsByIdTree()->toLinkedList();
        playlistTwoByIdLinkedList = playlistTwo->getSongsByIdTree()->toLinkedList();

        if (!playlistOneByIdLinkedList && !playlistTwoByIdLinkedList) {
            return StatusType::SUCCESS; // Both lists are empty
        }

        // Merge arrays by song ID
        mergedSongsById = mergeSongLinkedListsById(playlistOneByIdLinkedList, playlistTwoByIdLinkedList);

        // Count merged songs
        int newSongsAmount = 0;
        SongNodeList *mergedSongsIterator = mergedSongsById;
        while (mergedSongsIterator) {
            newSongsAmount++;
            mergedSongsIterator = mergedSongsIterator->next;
        }

        // Create new song tree by ID
        newSongsByIdTree = new SongTreePlaylist(newSongsAmount);
        newSongsByIdTree->populateTree(mergedSongsById);

        // Get songs by play count
        playlistOneByPlayCountLinkedList = playlistOne->getAVLPlayCount()->toLinkedList();
        playlistTwoByPlayCountLinkedList = playlistTwo->getAVLPlayCount()->toLinkedList();

        // Merge arrays by play count
        mergedSongsByPlayCount = mergeSongLinkedListsByPlayCount(playlistOneByPlayCountLinkedList,
                                                                 playlistTwoByPlayCountLinkedList);

        // Create new play count tree
        newSongsByPlayCountTree = new PlayCountNode(newSongsAmount);
        // Make a copy of the merged list pointer, since it'll be modified during populateCountNodeTree
        SongNodeList *tempPlayCountList = mergedSongsByPlayCount;
        newSongsByPlayCountTree->populateCountNodeTree(tempPlayCountList);

        // Find the tail of the merged list
        SongNodeList *tail = mergedSongsById;
        while (tail && tail->next) {
            tail = tail->next;
        }

        // Update playlist1's data structures
        playlistOne->setSongsByIdTree(newSongsByIdTree);
        playlistOne->setAVLPlayCount(newSongsByPlayCountTree);
        playlistOne->setNumOfSongs(newSongsAmount);
        playlistOne->setListHead(mergedSongsById);
        playlistOne->setListTail(tail);

        // Clean up temporary lists
        SongNodeList *current = playlistOneByIdLinkedList;
        while (current) {
            SongNodeList *next = current->next;
            delete current;
            current = next;
        }

        current = playlistTwoByIdLinkedList;
        while (current) {
            SongNodeList *next = current->next;
            delete current;
            current = next;
        }

        current = playlistOneByPlayCountLinkedList;
        while (current) {
            SongNodeList *next = current->next;
            delete current;
            current = next;
        }

        current = playlistTwoByPlayCountLinkedList;
        while (current) {
            SongNodeList *next = current->next;
            delete current;
            current = next;
        }

        current = mergedSongsByPlayCount;
        while (current) {
            SongNodeList *next = current->next;
            delete current;
            current = next;
        }

        // Remove playlist2 from the tree
        this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);

        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &exc) {
        // Clean up any allocated memory before returning error
        return StatusType::ALLOCATION_ERROR;
    }
}
