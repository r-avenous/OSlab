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
    int is_evicted;

    Guest(){
        id = -1;
        priority = -1;
        stay_time = 0;
        is_evicted = 0;
    }

    Guest& operator=(Guest &g){
        id = g.id;
        priority = g.priority;
        stay_time = g.stay_time;
        is_evicted = g.is_evicted;
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
    _room(){
        room_id = -1;
        // is_occupied = false;
        // is_dirty = false;
        // is_evicted = 0;
        Guest g = Guest();
        guest = g;
        time_occupied = 0;
        num_times_occupied = 0;
    }
    
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
    vector<Room> dirty_and_empty_rooms, dirtyRooms;
    set<Room, roomcmp> nondirty_and_occupied_rooms;
    set<Room, roomcmp> dirty_and_occupied_rooms;
    sem_t start_cleaning_sem, net_occ_sem;
    // vector<Room> nondirty_and_occupied_rooms;
    // vector<Room> dirty_and_occupied_rooms;
    // int occupancy;
    sem_t clean_rooms_sem;
    bool is_cleaning = false;
    Room getCleanRoom()
    {
        Room room = nondirty_and_empty_rooms.back();
        nondirty_and_empty_rooms.pop_back();
        return room;
    }
    void occupy(Room &room, Guest guest)
    {
        room.guest = guest;
        room.num_times_occupied++;
        room.time_occupied += guest.stay_time; 
        if(room.num_times_occupied==1)
        {
            nondirty_and_occupied_rooms.insert(room);
        }
        else
        {
            dirtyRooms.push_back(room);
        }
    }
    Room getLeastPriorityRoom()
    {
        if(nondirty_and_occupied_rooms.size()!=0)
        {
            return *nondirty_and_occupied_rooms.begin();
        }
        return Room();
    }
    int cleanRoom()
    {
        Room room = dirtyRooms.back();
        dirtyRooms.pop_back();
        int t = room.time_occupied;
        room.num_times_occupied = 0;
        room.time_occupied = 0;
        Guest g = Guest();
        room.guest = g;
        printf("Room %d is cleaned\n" , room.room_id);
        nondirty_and_empty_rooms.push_back(room);
        return t;
    }
}Hotel;

#endif