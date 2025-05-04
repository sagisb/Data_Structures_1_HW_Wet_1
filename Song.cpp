class Song{
    int songId;
    int countPlayed;
    int countConnectionPlaylist;
public:
    Song(int songId) : songId(songId), countPlayed(0), countConnectionPlaylist(0){
        if(songId <= 0)
            //add throw something
    }
    int getSongId() const{
        return songId;
    }
    int getCountPlayed() const{
        return countPlayed;
    }
    int getCountPlaylist() const{
        return countConnectionPlaylist;
    }
    void increaseCountPlayed(){
        countPlayed++;
    }
    void increaseSongToPlaylist(){
        countConnectionPlaylist++;
    }
    void decrementingSongFromPlaylist(){
        countConnectionPlaylist--;
    }


};