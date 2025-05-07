// You can edit anything you want in this file.
// However you need to implement all public DSpotify function, as provided below as a template

#include "dspotify25b1.h"


DSpotify::DSpotify(){

}

DSpotify::~DSpotify(){

}

StatusType DSpotify::add_playlist(int playlistId){
    return StatusType::FAILURE;
}

StatusType DSpotify::delete_playlist(int playlistId){
    return StatusType::FAILURE;
}

StatusType DSpotify::add_song(int songId, int plays){
    return StatusType::FAILURE;
}

StatusType DSpotify::add_to_playlist(int playlistId, int songId){
    return StatusType::FAILURE;
}

StatusType DSpotify::delete_song(int songId){
    return StatusType::FAILURE;
}

StatusType DSpotify::remove_from_playlist(int playlistId, int songId){
    return StatusType::FAILURE;
}

output_t<int> DSpotify::get_plays(int songId){
    return 0;
}

output_t<int> DSpotify::get_num_songs(int playlistId){
    return 0;
}

output_t<int> DSpotify::get_by_plays(int playlistId, int plays){
    return 0;
}

StatusType DSpotify::unite_playlists(int playlistId1, int playlistId2){
    return StatusType::FAILURE;
}
