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

// Function to clean up a linked list
void cleanupLinkedList(SongNodeList *list) {
    SongNodeList *current = list;
    while (current) {
        SongNodeList *next = current->next;
        delete current;
        current = next;
    }
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
        AVLPlaylist *newPlaylist = nullptr;

        if (playlist == nullptr) {
            playlist = new AVLPlaylist(playlistId);
        } else {
            newPlaylist = playlist->insert(playlist, playlistId);
            if (newPlaylist) playlist = newPlaylist;
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
    AVLAllSongs *newRoot = nullptr;

    try {
        newSongObject = new Song(songId, plays);

        if (this->songs == nullptr) {
            this->songs = new AVLAllSongs(songId, newSongObject);
        } else {
            newRoot = this->songs->insert(this->songs, songId, newSongObject);
            if (newRoot) {
                this->songs = newRoot; // Tree might have been rebalanced
            }
        }
        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &a) {
        delete newSongObject;
        return StatusType::ALLOCATION_ERROR;
    }
}

StatusType DSpotify::add_to_playlist(int playlistId, int songId) {
    if (songId <= 0 || playlistId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    if (!this->songs || !this->playlist) {
        return StatusType::FAILURE;
    }

    AVLAllSongs *relevantSongNode = this->songs->search(this->songs, songId);
    AVLPlaylist *relevantPlaylist = this->playlist->search(this->playlist, playlistId);

    if (!relevantSongNode || !relevantPlaylist || relevantPlaylist->playlist_ptr->getSongById(songId)) {
        return StatusType::FAILURE;
    }

    try {
        relevantPlaylist->playlist_ptr->addSong(relevantSongNode->song_ptr);
        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &a) {
        return StatusType::ALLOCATION_ERROR;
    }
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

    this->songs = this->songs->deleteNode(this->songs, songId);
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

// Create a deep copy of a linked list
SongNodeList *deepCopyLinkedList(SongNodeList *list) {
    if (!list)
        return nullptr;

    SongNodeList *newHead = nullptr;
    SongNodeList *newTail = nullptr;
    SongNodeList *current = list;

    try {
        while (current) {
            SongNodeList *newNode = new SongNodeList(current->songPtr);
            if (!newHead) {
                newHead = newTail = newNode;
            } else {
                newTail->next = newNode;
                newNode->prev = newTail;
                newTail = newNode;
            }
            current = current->next;
        }
        return newHead;
    }
    catch (const std::bad_alloc &) {
        cleanupLinkedList(newHead);
        throw;
    }
}

// create a linked list of songs by id without duplicates
SongNodeList *mergeSongLinkedListsById(SongNodeList *firstList, SongNodeList *secondList) {
    if (!firstList && !secondList)
        return nullptr;
    if (!firstList)
        return deepCopyLinkedList(secondList);
    if (!secondList)
        return deepCopyLinkedList(firstList);

    SongNodeList *head = nullptr;
    SongNodeList *tail = nullptr;

    SongNodeList *firstCurrent = firstList;
    SongNodeList *secondCurrent = secondList;

    try {
        while (firstCurrent && secondCurrent) {
            SongNodeList *newNode = nullptr;

            if (firstCurrent->songPtr->getSongId() < secondCurrent->songPtr->getSongId()) {
                newNode = new SongNodeList(firstCurrent->songPtr);
                firstCurrent = firstCurrent->next;
            } else if (firstCurrent->songPtr->getSongId() > secondCurrent->songPtr->getSongId()) {
                newNode = new SongNodeList(secondCurrent->songPtr);
                secondCurrent = secondCurrent->next;
            } else { // Equal IDs, duplicate song
                newNode = new SongNodeList(firstCurrent->songPtr);
                // Decrement the countConnectionPlaylist since this song will only be in one playlist after the merge
                secondCurrent->songPtr->decrementingSongFromPlaylist();
                firstCurrent = firstCurrent->next;
                secondCurrent = secondCurrent->next;
            }

            // Add the new node to our result list
            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }

        // Add any remaining nodes from first list
        while (firstCurrent) {
            SongNodeList *newNode = new SongNodeList(firstCurrent->songPtr);
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
            firstCurrent = firstCurrent->next;
        }

        // Add any remaining nodes from second list
        while (secondCurrent) {
            SongNodeList *newNode = new SongNodeList(secondCurrent->songPtr);
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
            secondCurrent = secondCurrent->next;
        }

        return head;
    }
    catch (const std::bad_alloc &) {
        // Clean up on allocation failure
        cleanupLinkedList(head);
        throw;
    }
}

// create a linked list of songs sorted by play count, and then song ID
SongNodeList *mergeSongLinkedListsByPlayCount(SongNodeList *firstList, SongNodeList *secondList) {
    if (!firstList && !secondList)
        return nullptr;
    if (!firstList)
        return deepCopyLinkedList(secondList);
    if (!secondList)
        return deepCopyLinkedList(firstList);

    // Both lists have elements - create a new merged list
    SongNodeList *head = nullptr;
    SongNodeList *tail = nullptr;

    SongNodeList *firstCurrent = firstList;
    SongNodeList *secondCurrent = secondList;

    try {
        // Merge the lists
        while (firstCurrent && secondCurrent) {
            SongNodeList *newNode = nullptr;

            if (firstCurrent->songPtr->getCountPlayed() < secondCurrent->songPtr->getCountPlayed()) {
                newNode = new SongNodeList(firstCurrent->songPtr);
                firstCurrent = firstCurrent->next;
            } else if (firstCurrent->songPtr->getCountPlayed() > secondCurrent->songPtr->getCountPlayed()) {
                newNode = new SongNodeList(secondCurrent->songPtr);
                secondCurrent = secondCurrent->next;
            } else { // Equal play counts, sort by song ID
                if (firstCurrent->songPtr->getSongId() <= secondCurrent->songPtr->getSongId()) {
                    newNode = new SongNodeList(firstCurrent->songPtr);
                    firstCurrent = firstCurrent->next;

                    // If both have the same song ID, advance both pointers
                    if (secondCurrent->songPtr->getSongId() == newNode->songPtr->getSongId()) {
                        secondCurrent = secondCurrent->next;
                    }
                } else {
                    newNode = new SongNodeList(secondCurrent->songPtr);
                    secondCurrent = secondCurrent->next;
                }
            }

            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
        }

        // Add any remaining nodes from first list
        while (firstCurrent) {
            SongNodeList *newNode = new SongNodeList(firstCurrent->songPtr);
            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            firstCurrent = firstCurrent->next;
        }

        // Add any remaining nodes from second list
        while (secondCurrent) {
            SongNodeList *newNode = new SongNodeList(secondCurrent->songPtr);
            if (!head) {
                head = tail = newNode;
            } else {
                tail->next = newNode;
                newNode->prev = tail;
                tail = newNode;
            }
            secondCurrent = secondCurrent->next;
        }

        return head;
    }
    catch (const std::bad_alloc &) {
        cleanupLinkedList(head);
        throw;
    }
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2) {
    if (playlistId1 <= 0 || playlistId2 <= 0 || playlistId1 == playlistId2) {
        return StatusType::INVALID_INPUT;
    }

    AVLPlaylist *playlistOneNode = playlist->search(playlist, playlistId1);
    AVLPlaylist *playlistTwoNode = playlist->search(playlist, playlistId2);
    if (!playlistOneNode || !playlistTwoNode) {
        return StatusType::FAILURE;
    }

    Playlist *playlistOne = playlistOneNode->playlist_ptr;
    Playlist *playlistTwo = playlistTwoNode->playlist_ptr;

    if (playlistTwo->getNumOfSongs() == 0) {
        this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);
        return StatusType::SUCCESS;
    } else if (playlistOne->getNumOfSongs() == 0) {
        // If playlist1 is empty, just copy all data structures from playlist2
        SongNodeList *playlist2Songs = nullptr;
        SongNodeList *playlist2SongsByCount = nullptr;
        SongTreePlaylist *newTree = nullptr;
        PlayCountNode *newCountTree = nullptr;

        try {
            // Create a deep copy of playlist2's song tree
            playlist2Songs = playlistTwo->getSongsByIdTree()->toLinkedList();
            newTree = new SongTreePlaylist(playlistTwo->getNumOfSongs());
            newTree->populateTree(playlist2Songs);

            // Create a deep copy of playlist2's play count tree
            playlist2SongsByCount = playlistTwo->getAVLPlayCount()->toLinkedList();
            newCountTree = new PlayCountNode(playlistTwo->getNumOfSongs());
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

            try {
                // Deep copy the song list
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

                // Update playlist1 with new list
                playlistOne->setListHead(head);
                playlistOne->setListTail(tail);

                // Clean up temporary lists
                cleanupLinkedList(playlist2Songs);
                cleanupLinkedList(playlist2SongsByCount);

                // Remove playlist2
                this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);
                return StatusType::SUCCESS;
            }
            catch (const std::bad_alloc &) {
                // Clean up the linked list we were creating
                cleanupLinkedList(head);
                throw;
            }
        }
        catch (const std::bad_alloc &exc) {
            // Clean up any allocated resources
            cleanupLinkedList(playlist2Songs);
            cleanupLinkedList(playlist2SongsByCount);
            delete newTree;
            delete newCountTree;
            return StatusType::ALLOCATION_ERROR;
        }
    }

    SongNodeList *playlistOneByIdLinkedList = nullptr;
    SongNodeList *playlistTwoByIdLinkedList = nullptr;
    SongNodeList *playlistOneByPlayCountLinkedList = nullptr;
    SongNodeList *playlistTwoByPlayCountLinkedList = nullptr;
    SongNodeList *mergedSongsById = nullptr;
    SongNodeList *mergedSongsByPlayCount = nullptr;
    SongTreePlaylist *newSongsByIdTree = nullptr;
    PlayCountNode *newSongsByPlayCountTree = nullptr;

    try {
        // First, clean up old linked list to avoid leaks
        SongNodeList *oldSongsList = playlistOne->getListHead();

        // Convert song trees to linked lists
        playlistOneByIdLinkedList = playlistOne->getSongsByIdTree()->toLinkedList();
        playlistTwoByIdLinkedList = playlistTwo->getSongsByIdTree()->toLinkedList();

        if (!playlistOneByIdLinkedList && !playlistTwoByIdLinkedList) {
            return StatusType::SUCCESS;
        }

        // Merge arrays by song ID, duplicates will be handled inside the merge function
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

        mergedSongsByPlayCount = mergeSongLinkedListsByPlayCount(playlistOneByPlayCountLinkedList,
                                                                 playlistTwoByPlayCountLinkedList);

        // Create new play count tree
        newSongsByPlayCountTree = new PlayCountNode(newSongsAmount);
        SongNodeList *tempPlayCountList = mergedSongsByPlayCount;
        newSongsByPlayCountTree->populateCountNodeTree(tempPlayCountList);

        // Find the tail of the merged list
        SongNodeList *tail = mergedSongsById;
        while (tail && tail->next) {
            tail = tail->next;
        }

        // Clean up existing list in playlist1
        cleanupLinkedList(oldSongsList);

        // Update playlist1's data structures with new merged structures
        playlistOne->setSongsByIdTree(newSongsByIdTree);
        playlistOne->setAVLPlayCount(newSongsByPlayCountTree);
        playlistOne->setNumOfSongs(newSongsAmount);
        playlistOne->setListHead(mergedSongsById);
        playlistOne->setListTail(tail);

        // Clean up temporary lists
        cleanupLinkedList(playlistOneByIdLinkedList);
        cleanupLinkedList(playlistTwoByIdLinkedList);
        cleanupLinkedList(playlistOneByPlayCountLinkedList);
        cleanupLinkedList(playlistTwoByPlayCountLinkedList);
        cleanupLinkedList(mergedSongsByPlayCount);

        // Note: mergedSongsById is now owned by playlistOne, so we don't free it

        // Remove playlist2 from the tree
        this->playlist = this->playlist->deleteNode(this->playlist, playlistId2);

        return StatusType::SUCCESS;
    }
    catch (const std::bad_alloc &exc) {
        cleanupLinkedList(playlistOneByIdLinkedList);
        cleanupLinkedList(playlistTwoByIdLinkedList);
        cleanupLinkedList(playlistOneByPlayCountLinkedList);
        cleanupLinkedList(playlistTwoByPlayCountLinkedList);
        cleanupLinkedList(mergedSongsById);
        cleanupLinkedList(mergedSongsByPlayCount);

        delete newSongsByIdTree;
        delete newSongsByPlayCountTree;

        return StatusType::ALLOCATION_ERROR;
    }
}
