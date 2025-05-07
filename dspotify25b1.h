// 
// 234218 Data Structures 1.
// Semester: 2025B (Spring).
// Wet Exercise #1.
// 
// The following header file contains all methods we expect you to implement.
// You MAY add private methods and fields of your own.
// DO NOT erase or modify the signatures of the public methods.
// DO NOT modify the preprocessors in this file.
// DO NOT use the preprocessors in your other code files.
// 

#ifndef DSPOTIFY25SPRING_WET1_H_
#define DSPOTIFY25SPRING_WET1_H_

#include "wet1util.h"

class DSpotify {
private:
    //
    // Here you may add anything you want
    //
    
public:
    // <DO-NOT-MODIFY> {
    DSpotify();

    virtual ~DSpotify();

    StatusType add_playlist(int playlistId);

    StatusType delete_playlist(int playlistId);

    StatusType add_song(int songId, int plays);

    StatusType add_to_playlist(int playlistId, int songId);

    StatusType delete_song(int songId);

    StatusType remove_from_playlist(int playlistId, int songId);

    output_t<int> get_plays(int songId);

    output_t<int> get_num_songs(int playlistId);

    output_t<int> get_by_plays(int playlistId, int plays);

    StatusType unite_playlists(int playlistId1, int playlistId2);
    // } </DO-NOT-MODIFY>
};

#endif // DSPOTIFY25SPRING_WET1_H_
