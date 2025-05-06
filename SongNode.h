struct SongNodeById {
    int songId;
    Song *song;
    SongNodeById *left;
    SongNodeById *right;

    SongNodeById(std::string id, Song *s) : songId(id), song(s), left(nullptr), right(nullptr) {};
};
