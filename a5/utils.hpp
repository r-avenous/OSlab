#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <queue>
#include <set>
#define MAX_PRIORITY 10

using namespace std;

class Guest{

public:
    int id;
    int priority;
    int stay_time;
    // int is_evicted;

    Guest(){
        id = -1;
        priority = -1;
        stay_time = 0;
        // is_evicted = 0;
    }

    Guest& operator=(Guest &g){
        id = g.id;
        priority = g.priority;
        stay_time = g.stay_time;
        // is_evicted = g.is_evicted;
        return *this;
    }
};

typedef struct _room{
    int room_id;
    // bool is_occupied;
    // bool is_dirty;
    // int is_evicted;
    Guest guest;
    int time_occupied;
    int num_times_occupied;

    
}Room;

// bool operator<(const Room &r1, const Room &r2);
//     // return r1.guest.priority < r2.guest.priority;


// bool operator==(const Room &r1, const Room &r2);
//     // return r1.guest.id == r2.guest.id;

// auto RoomCMP = [](const Room &r1, const Room &r2){ return r1.guest.priority < r2.guest.priority;};
struct roomcmp
{
    bool operator() (Room r1, Room r2) const
    {
        return r1.guest.priority < r2.guest.priority;
    }
};

typedef struct _hotel{

    vector<Room> nondirty_and_empty_rooms;
    vector<Room> dirty_and_empty_rooms;
    set<Room, roomcmp> nondirty_and_occupied_rooms;
    set<Room, roomcmp> dirty_and_occupied_rooms;
    sem_t start_cleaning_sem;
    // vector<Room> nondirty_and_occupied_rooms;
    // vector<Room> dirty_and_occupied_rooms;
    // int occupancy;
    sem_t clean_rooms_sem;
    vector<int> is_evicted;
    // bool is_cleaning;
}Hotel;

#endif