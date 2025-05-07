// 
// 234218 Data Structures 1.
// Semester: 2025B (Spring).
// Wet Exercise #1.
// 
// The following main file is necessary to link and run your code.
// This file is READ ONLY: even if you submit something else, the compiler will use our file.
// 

#include "dspotify25b1.h"
#include <string>
#include <iostream>

using namespace std;

void print(string cmd, StatusType res);
void print(string cmd, output_t<int> res);

int main()
{
    
    int d1, d2;

    // Init
    DSpotify *obj = new DSpotify();
    
    // Execute all commands in file
    string op;
    while (cin >> op)
    {
        if (!op.compare("add_playlist")) {
            cin >> d1;
            print(op, obj->add_playlist(d1));
        } else if (!op.compare("delete_playlist")) {
            cin >> d1;
            print(op, obj->delete_playlist(d1));
        } else if (!op.compare("add_song")) {
            cin >> d1 >> d2;
            print(op, obj->add_song(d1, d2));
        } else if (!op.compare("add_to_playlist")) {
            cin >> d1 >> d2;
            print(op, obj->add_to_playlist(d1, d2));
        } else if (!op.compare("delete_song")) {
            cin >> d1;
            print(op, obj->delete_song(d1));
        } else if (!op.compare("remove_from_playlist")) {
            cin >> d1 >> d2;
            print(op, obj->remove_from_playlist(d1, d2));
        } else if (!op.compare("get_plays")) {
            cin >> d1;
            print(op, obj->get_plays(d1));
        } else if (!op.compare("get_num_songs")) {
            cin >> d1;
            print(op, obj->get_num_songs(d1));
        } else if (!op.compare("get_by_plays")) {
            cin >> d1 >> d2;
            print(op, obj->get_by_plays(d1, d2));
        } else if (!op.compare("unite_playlists")) {
            cin >> d1 >> d2;
            print(op, obj->unite_playlists(d1, d2));
        } else {
            cout << "Unknown command: " << op << endl;
            break;
        }
        // Verify no faults
        if (cin.fail()){
            cout << "Invalid input format" << endl;
            break;
        }
    }

    // Quit 
    delete obj;
    return 0;
}

// Helpers
static const char *StatusTypeStr[] =
{
    "SUCCESS",
    "ALLOCATION_ERROR",
    "INVALID_INPUT",
    "FAILURE"
};

void print(string cmd, StatusType res) 
{
    cout << cmd << ": " << StatusTypeStr[(int) res] << endl;
}

void print(string cmd, output_t<int> res)
{
    if (res.status() == StatusType::SUCCESS) {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << ", " << res.ans() << endl;
    } else {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}
