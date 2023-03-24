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

    Guest(){
        id = -1;
        priority = -1;
        stay_time = 0;
    }

    Guest& operator=(Guest &g){
        id = g.id;
        priority = g.priority;
        stay_time = g.stay_time;
        return *this;
    }
};

typedef struct _room{
    int room_id;
    Guest guest;
    int time_occupied;
    int num_times_occupied;
    _room(){
        room_id = -1;
        Guest g = Guest();
        guest = g;
        time_occupied = 0;
        num_times_occupied = 0;
    }
    
}Room;

struct roomcmp
{
    bool operator() (Room r1, Room r2) const
    {
        return r1.guest.priority < r2.guest.priority;
    }
};

typedef struct _hotel{

    vector<Room> nondirty_and_empty_rooms, dirtyRooms;
    set<Room, roomcmp> nondirty_and_occupied_rooms;
    
    sem_t net_occ_sem, clean_rooms_sem;

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
            printf("Room [%d] is dirty\n" , room.room_id);
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
    int cleanRoom(int& room_id)
    {
        if(dirtyRooms.size()==0) return 0;
        Room room = dirtyRooms.back();
        room_id = room.room_id;

        dirtyRooms.pop_back();

        int t = room.time_occupied;
        room.num_times_occupied = 0;
        room.time_occupied = 0;
        Guest g = Guest();
        room.guest = g;
        printf("Room [%d] is being cleaned\n", room.room_id);
        nondirty_and_empty_rooms.push_back(room);
        return t;
    }
}Hotel;

#endif